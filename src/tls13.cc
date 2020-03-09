#include<utility>
#include<nettle/curve25519.h>
#include<fstream>
#include"cert_util.h"
#include"ecdsa.h"
#include"tls13.h"
#include"pss.h"
using namespace std;

template class TLS13<true>;
template class TLS13<false>;
template<bool SV> map<array<uint8_t, 56>, SClient> TLS13<SV>::pskNclient_;

static string init_certificate()
{
	ifstream f("fullchain.pem");//openssl req -x509 -days 1000 -new -key key.pem -out cert.pem
	vector<unsigned char> r;
	for(string s; (s = get_certificate_core(f)) != "";) {
		auto v = base64_decode(s);
		r.push_back(0); r.push_back(0); r.push_back(0);//fill with length
		mpz2bnd(v.size(), r.end() - 3, r.end());
		r.insert(r.end(), v.begin(), v.end());
		r.push_back(0); r.push_back(0);//extension!!!
	}
	const int REQUESTED_CONTEXT = 0;
	vector<uint8_t> front{CERTIFICATE, 0,0,0, REQUESTED_CONTEXT, 0,0,0};
	mpz2bnd(r.size(), front.end() - 3, front.end());
	mpz2bnd(r.size() + 4, front.begin() + 1, front.begin() + 4);
	r.insert(r.begin(), front.begin(), front.end());
	return {r.begin(), r.end()};
}
static string certificate13 = init_certificate();

#pragma pack(1)

template<bool SV> string TLS13<SV>::client_ext() {
	struct Ext {
		uint8_t extension_length[2] = {0, 0};

		uint8_t supported_group[2] = {0, 10};//type
		uint8_t supported_group_length[2] = {0, 6};//length
		uint8_t support_group_list_length[2] = {0, 4};
		uint8_t secp256r1[2] = {0, 23};
		uint8_t x255[2] = {0, 29};
		
		uint8_t ec_point_format[2] = {0, 11};//type
		uint8_t ec_point_format_length[2] = {0, 2};//length
		uint8_t ec_length = 1;
		uint8_t non_compressed = 0;

		uint8_t key_share[2] = {0, 51};//type
		uint8_t key_share_length[2] = {0, 107};//length
		uint8_t client_key_share_len[2] = {0, 105};

		uint8_t secp256r1_key[2] = {0, 23};
		uint8_t key_length[2] = {0, 65};
		uint8_t type = 4;
		uint8_t x[32], y[32];

		uint8_t x25519[2] = {0, 29};
		uint8_t key_length2[2] = {0, 32};
		uint8_t x2[32];

		uint8_t supported_version[2] = {0, 0x2b};
		uint8_t supported_version_length[2] = {0, 5};
		uint8_t supported_version_list_length = 4;
		uint8_t supported_versions[4] = {3, 4, 3, 3};//TLS 1.3, TLS 1.2

		uint8_t psk_mode[2] = {0, 0x2d};
		uint8_t psk_mode_length[2] = {0, 2};
		uint8_t psk_mode_llength = 1;
		uint8_t psk_with_ecdhe = 1;

		uint8_t signature_algorithm[2] = {0, 13};
		uint8_t signature_algorithm_length[2] = {0, 6};
		uint8_t signature_alg_len[2] = {0, 4};
		uint8_t signature[4] = {4, 1, 4, 3};//sha256 rsa, ecdsa sha 256
	} ext;
	mpz2bnd(this->P_.x, ext.x, ext.x + 32);
	mpz2bnd(this->P_.y, ext.y, ext.y + 32);
	mpz2bnd(sizeof(Ext) - 2, ext.extension_length, ext.extension_length + 2);
	mpz2bnd(this->prv_key_, prv_, prv_ + 32);
	curve25519_mul_g(ext.x2, prv_);
	return struct2str(ext);
}

template<bool SV> bool TLS13<SV>::supported_group(unsigned char *p, int len)
{//return true when support secp256r1, len = ext leng, p point at the start of actual ext
	for(int i=2; i<len; i+=2) if(*(p+i) == 0 && *(p+i+1) == 23) return true;
	return false;
}

template<bool SV> bool TLS13<SV>::point_format(unsigned char *p, int len)
{
	for(int i=1; i<len; i++) if(*(p+i) == 0) return true;
	return false;
}

template<bool SV> bool TLS13<SV>::sub_key_share(unsigned char *p)
{
	if(*p == 0 && *(p+1) == 23 && *(p+4) == 4) {
		EC_Point Q{bnd2mpz(p + 5, p + 37), bnd2mpz(p + 37, p + 69), this->secp256r1_};
		premaster_secret_ = (Q * this->prv_key_).x;
		return true;
	} else if(*p == 0 && *(p+1) == 29) {
		uint8_t q[32];
		curve25519_mul(q, prv_, p + 4);
		premaster_secret_ = bnd2mpz(q, q+32);
		this->P_.x = -1;
		return true;
	} else return false;
}

template<bool SV> bool TLS13<SV>::key_share(unsigned char *p, int len)
{
	len = *p++ * 0x100 + *p++;
	for(unsigned char *q = p; p < q + len; p += p[2] * 0x100 + p[3] + 4)
		if(sub_key_share(p)) return true;
	return false;
}

template<bool SV> bool TLS13<SV>::supported_version(unsigned char *p, int len) 
{
	for(int i=1; i<len; i+=2) if(*(p+i) == 3 && *(p+i+1) == 4) return true;
	return false;
}

template<bool SV> bool TLS13<SV>::psk(unsigned char *p, int len)
{
	struct {
		uint8_t identities_sz[2] = {0, 21};
		uint8_t ticket_sz[2] = {0, 15};
		uint8_t ticket[15];
		uint8_t obfuscated_ticket_age[4];
		uint8_t binder_sz[2];
		uint8_t bind_sz[2];
		uint8_t binder[32];
	} h;
	int id_sz = *p++ * 0x100 + *p++;
	for(unsigned char *q = p; p < q + id_sz;) {
		int ticket_sz = *p++ * 0x100 + *p++;
		vector<uint8_t> ar{p, p += ticket_sz};
		if(auto it = pskNclient_.find(ar); it != pskNclient_.end() &&
				chrono::system_clock::now() < it->second.issue_time + 9 * 256s) 
			sclient_ = it->second;
	}

	return true;
}

template<bool SV> bool TLS13<SV>::client_ext(unsigned char *p) //buggy
{//check extension and return if it is capable of negotiating with us
	int total_len = *p++ * 0x100 + *p++;
	bool check_ext[5] = {false,};
	for(unsigned char *q = p; p < q + total_len;) {
		int type = *p++ * 0x100 + *p++;
		int len = *p++ * 0x100 + *p++;
		switch(type) {
			case 10: check_ext[0] = supported_group(p, len); break;
			case 11: check_ext[1] = point_format(p, len); break;
			case 43: check_ext[2] = supported_version(p, len); break;
			case 45: check_ext[3] = true; break;
			case 51: check_ext[4] = key_share(p, len); break;
			case 41: psk(p, len); break;
		}
		p += len;
	}
	for(int i=0; i<5; i++) if(check_ext[i] == false) return false;
	return true;
}

template<bool SV> string TLS13<SV>::server_ext() {
	struct Ext {
		uint8_t extension_length[2] = {0, 79};

		uint8_t supported_version[2] = {0, 43};
		uint8_t supported_version_length[2] = {0, 2};
		uint8_t support[2] = {3, 4};
	} ext;
	struct {
		uint8_t key_share[2] = {0, 51};
		uint8_t key_share_length[2] = {0, 69};
		uint8_t type[2] = {0, 23};
		uint8_t key_length[2] = {0, 65};
		uint8_t point_type = 4;
		uint8_t x[32], y[32];
	} secp;
	struct {
		uint8_t key_share[2] = {0, 51};
		uint8_t key_share_length[2] = {0, 36};
		uint8_t type[2] = {0, 29};
		uint8_t key_length[2] = {0, 32};
		uint8_t x[32];
	} x25519;
	if(this->P_.x == -1) {
		curve25519_mul_g(x25519.x, prv_);
		ext.extension_length[1] = 46;
		return struct2str(ext) + struct2str(x25519);
	} else {
		mpz2bnd(this->P_.x, secp.x, secp.x + 32);
		mpz2bnd(this->P_.y, secp.y, secp.y + 32);
		return struct2str(ext) + struct2str(secp);
	}
}

template<bool SV> string TLS13<SV>::encrypted_extension()
{
	struct H {
		uint8_t enc_ext_type = 8;
		uint8_t total_len[3] = {0, 0, 10};
		uint8_t ext_len[2] = {0, 8};
		uint8_t supported_group[2] = {0, 10};
		uint8_t len[2] = {0, 4};
		uint8_t group[4] = {0, 0x1d, 0, 0x17};
	} h;
	string r = struct2str(h);
	this->accumulated_handshakes_ += r;
	return r;
}

template<bool SV> void TLS13<SV>::protect_handshake()
{//call after server hello
	hkdf_.zero_salt();
	uint8_t psk[HASH::output_size] = {0,}, pre[32];
	auto early_secret = hkdf_.extract(psk, HASH::output_size);
	hkdf_.salt(&early_secret[0], early_secret.size());
	auto a = hkdf_.derive_secret("derived", "");
	hkdf_.salt(&a[0], a.size());
	mpz2bnd(premaster_secret_, pre, pre + 32);
	auto handshake_secret = hkdf_.extract(pre, 32);
	finished_key_ = set_aes(handshake_secret, "c hs traffic", "s hs traffic");
	hkdf_.salt(&handshake_secret[0], handshake_secret.size());
	a = hkdf_.derive_secret("derived", "");
	hkdf_.salt(&a[0], a.size());
	this->master_secret_ = hkdf_.extract(psk, HASH::output_size);
}

template<bool SV> string TLS13<SV>::finished(string &&s)
{//handle message without tls header
	struct H {
		uint8_t finished = 0x14;
		uint8_t length[3] = { 0, 0, 32};
	} h;

	hkdf_.salt(finished_key_[s == "" ? SV : !SV].data(), HASH::output_size);//define use
	HASH sha;//this is cipher suite hash algorithm
	auto a = sha.hash(this->accumulated_handshakes_.begin(),
			this->accumulated_handshakes_.end());
	a = hkdf_.hash(a.begin(), a.end());
	string fin = struct2str(h) + string{a.begin(), a.end()};
	this->accumulated_handshakes_ += fin;

//	if(SV == (s == "")) protect_data();//SV send, clietn receive 
	if(s == "") return fin;
	else return s == fin ? "" : this->alert(2, 51);
}

template<bool SV> void TLS13<SV>::protect_data()
{//call after serverfinished, set resumption master secret
	set_aes(this->master_secret_, "c ap traffic", "s ap traffic");
	hkdf_.salt(&this->master_secret_[0], this->master_secret_.size());
	resumption_master_secret_ = hkdf_.derive_secret("res master", this->accumulated_handshakes_);
}

template<bool SV> string TLS13<SV>::new_session_ticket()
{
	struct {
		uint8_t new_session_ticket = 4;
		uint8_t size[3] = {0, 0, HASH::output_size + 45};
		uint8_t ticket_lifetime_in_sec[4] = {0, 0, 9, 0};
		uint8_t ticket_age_add[4];
		uint8_t ticket_nonce_size = 16;
		uint8_t ticket_nonce[16];
		uint8_t ticket_size[2] = {0, 16};
		uint8_t ticket_id[16];
		uint8_t extension[2] = {0, 0};
	} h;
	mpz2bnd(random_prime(4), h.ticket_age_add, h.ticket_age_add + 4);
	mpz2bnd(random_prime(h.ticket_nonce_size),
			h.ticket_nonce, h.ticket_nonce + h.ticket_nonce_size);
	memcpy(h.ticket_id, h.ticket_nonce, h.ticket_nonce_size);
	hkdf_.salt(&resumption_master_secret_[0], resumption_master_secret_.size());
	auto binder = hkdf_.expand_label("resumption", 
			{h.ticket_nonce, h.ticket_nonce+10}, HASH::output_size);
	std::vector<uint8_t> ar{h.ticket_id, h.ticket_id + 16};
	sclient_.issue_time = chrono::system_clock::now();
	if(!sclient_.sp_client)
		sclient_.sp_client = make_shared<MClient>("localhost", inport_);
	pskNclient_[ar] = sclient_;
	return struct2str(h);
}

template<bool SV> array<vector<uint8_t>, 2>
TLS13<SV>::set_aes(vector<uint8_t> salt, string cl, string sv) {
	this->enc_seq_num_ = 0; this->dec_seq_num_ = 0;
	hkdf_.salt(&salt[0], salt.size());
	array<vector<unsigned char>, 2> secret, finished_key;
	secret[0] = hkdf_.derive_secret(cl, this->accumulated_handshakes_);
	secret[1] = hkdf_.derive_secret(sv, this->accumulated_handshakes_);
	for(int i=0; i<2; i++) {
		hkdf_.salt(&secret[i][0], secret[i].size());
		auto key = hkdf_.expand_label("key", "", 16);
		auto iv = hkdf_.expand_label("iv", "", 12);
		this->aes_[i].key(&key[0]);
		this->aes_[i].iv(&iv[0], 0, iv.size());
		finished_key[i] = hkdf_.expand_label("finished", "", HASH::output_size);
		sclient_.key[i] = move(key);
		sclient_.iv[i] = move(iv);
	}
	return finished_key;
}

template<bool SV> bool TLS13<SV>::server_ext(unsigned char *p) 
{//debug 
	int total_len = *p++ * 0x100 + *p++;
	for(uint8_t *q = p; p < q + total_len;) {
		int type = *p++ * 0x100 + *p++;
		int length = *p++ * 0x100 + *p++;
		if(type == 51 && sub_key_share(p)) return true;
		p += length;
	}
	return false;
}

template<bool SV> string TLS13<SV>::client_hello(string &&s)
{
	if constexpr(SV) {//is not returning correct ext_start
		unsigned char *p = (unsigned char*)&s[43];//session id length
		memcpy(echo_id_, p+1, *p);//copy session id
		p += *p + 1;
		int cipher_suite_len = *p++ * 0x100 + *p++;
		p += cipher_suite_len;
		p += *p + 1;//compression length
		int ext_start = p - (unsigned char*)&s[0];
		string r = TLS<SV>::client_hello(forward<string>(s));
		return s.size() > ext_start && client_ext(p) ? "" : r;
	} else {
		string hello = TLS<SV>::client_hello();
		this->accumulated_handshakes_ = "";
		string ext = client_ext();
		int hello_size = static_cast<uint8_t>(hello[3]) * 0x100 + static_cast<uint8_t>(hello[4]) + ext.size();
		mpz2bnd(hello_size, &hello[3], &hello[5]);//tls length
		mpz2bnd(hello_size - 4, &hello[6], &hello[9]);//handshake length
		return this->accumulate(hello + ext);
	}
}

template<bool SV> string TLS13<SV>::server_hello(string &&s)
{
	if constexpr(SV) {
		string tmp = this->accumulated_handshakes_;
		string hello = TLS<SV>::server_hello();
		if(!premaster_secret_) return hello;
		memcpy(&hello[44], echo_id_, 32);//echo session id
		hello[76] = 19; hello[77] = 1;//TLS AES128 GCM SHA256
		this->accumulated_handshakes_ = tmp;
		string ext = server_ext();
		int hello_size = static_cast<uint8_t>(hello[3]) * 0x100 + static_cast<uint8_t>(hello[4]) + ext.size();
		mpz2bnd(hello_size, &hello[3], &hello[5]);//tls length
		mpz2bnd(hello_size - 4, &hello[6], &hello[9]);//handshake length
		return this->accumulate(hello + ext);
	} else {
		string s2 = s;
		string r = TLS<SV>::server_hello(move(s2));
		return s.size() > 80 && server_ext((uint8_t*)&s[79]) ? "" : r;
	}
}

template<bool SV> string TLS13<SV>::server_certificate13()
{
	this->accumulated_handshakes_ += certificate13;
	return certificate13;
}

template<bool SV> string TLS13<SV>::certificate_verify()
{
	SHA2 sha;
	auto a = sha.hash(this->accumulated_handshakes_.begin(),
					  this->accumulated_handshakes_.end());
	string t;
	for(int i=0; i<64; i++) t += ' ';
	t += "TLS 1.3, server CertificateVerify";
	t += (uint8_t)0x0;
	t.insert(t.end(), a.begin(), a.end());

	struct {
		uint8_t type = 0x0f;
		uint8_t length[3] = {0, 1, 4};
		uint8_t signature[2] = {8, 4};//4 3 : ecdsa sha256, 8 4 : RSA SHA256 PSS 
		uint8_t len[2] = {1, 0};
		uint8_t sign[256];
	} h;

	auto v = pss_encode({t.begin(), t.end()}, mpz_sizeinbase(this->rsa_.K.get_mpz_t(), 2) - 1);
	mpz2bnd(this->rsa_.sign(bnd2mpz(v.begin(), v.end())), h.sign, h.sign + 256);
	t = struct2str(h);
	this->accumulated_handshakes_ += t;
	return t;
}

template<bool SV> shared_ptr<MClient>
TLS13<SV>::handshake(function<optional<string>()> read_f, function<void(string)> write_f)
{//handshake according to compromised version
	string s; optional<string> a;
	switch(1) { case 1://to use break
	if constexpr(SV) {
		if(s = this->alert(2, 0); !(a = read_f()) || 
				(s = client_hello(move(*a))) != "") break;
		if(s = server_hello(); premaster_secret_) {
			protect_handshake();
			s += this->change_cipher_spec();
			string t = encrypted_extension();
			t += server_certificate13();
			t += certificate_verify();
			t += finished();
			string tmp = this->accumulated_handshakes_;//save after server finished
			s += encode(move(t), 22);//first condition true:read error->alert(2, 0)
			write_f(s); //second condition true->error message of function v
			if(s = this->alert(2, 0); !(a = read_f())
					|| (s = this->change_cipher_spec(move(*a)))!="") break;
			if(s = this->alert(2, 0); !(a = read_f()) || !(a = this->decode(move(*a))) ||
					(protect_data(), false) || (s = finished(move(*a))) != "") break;
			write_f(encode(new_session_ticket() + new_session_ticket(), HANDSHAKE));
		} else {
			s += this->server_certificate();
			s += this->server_key_exchange();
			s += this->server_hello_done();	
			write_f(s);
			if(s = this->alert(2, 0); !(a = read_f()) ||
					(s = this->client_key_exchange(move(*a))) != "") break;
			if(s = this->alert(2, 0); !(a = read_f()) ||
					(s = this->change_cipher_spec(move(*a))) != "") break;
			if(s = this->alert(2, 0); !(a = read_f()) ||
					(s = TLS<SV>::finished(move(*a))) != "") break;
			s = this->change_cipher_spec();
			s += TLS<SV>::finished();
			write_f(move(s));//empty s
		} 
	} else {
		write_f(client_hello());
		if(a = read_f(); !a || (s = server_hello(move(*a))) != "") break;
		if(premaster_secret_) {
			protect_handshake();//should prepend header?
			if(s = this->alert(2, 0); !(a = read_f()) ||
					(s = this->change_cipher_spec(move(*a))) != "") break;
			if(s = this->alert(2, 0); !(a = read_f()) || !(a = this->decode(move(*a)))) break;
			else this->accumulated_handshakes_ += *a;
			string tmp = this->accumulated_handshakes_;
			s = this->change_cipher_spec();
			s += this->encode(finished());
			write_f(move(s));
			this->accumulated_handshakes_ = tmp;
			protect_data();
		} else {
			if(s = this->alert(2, 0); !(a = read_f()) ||
					(s = this->server_certificate(move(*a))) != "") break;
			if(s = this->alert(2, 0); !(a = read_f()) ||
					(s = this->server_key_exchange(move(*a))) != "") break;
			if(s = this->alert(2, 0); !(a = read_f()) ||
					(s = this->server_hello_done(move(*a))) != "") break;
			s = this->client_key_exchange();
			s += this->change_cipher_spec();
			s += TLS<SV>::finished();
			write_f(move(s));//empty s
			if(s = this->alert(2, 0); !(a = read_f()) ||
					(s = this->change_cipher_spec(move(*a))) != "") break;
			if(s = this->alert(2, 0); !(a = read_f()) ||
					(s = TLS<SV>::finished(move(*a))) != "") break;
		} 
	}//if constexpr
	}//switch
	if(s != "") write_f(s);//send alert message
	return sclient_.sp_client;
}

struct TLS_header {
	uint8_t content_type = HANDSHAKE;  // 0x17 for Application Data, 0x16 handshake
	uint8_t version[2] = {0x03, 0x03};      // 0x0303 for TLS 1.2
	uint8_t length[2] = {0, 4};       //length of encrypted_data, 4 : handshake size
	void set_length(int k) { length[0] = k / 0x100; length[1] = k % 0x100; }
	int get_length() { return length[0] * 0x100 + length[1]; }
};

template<bool SV> string TLS13<SV>::encode(string &&s, int type)
{
	return premaster_secret_ ? encode13(forward<string>(s), type) :
		TLS<SV>::encode(forward<string>(s), type);
}

template<bool SV> optional<string> TLS13<SV>::decode(string &&s)
{
	return premaster_secret_ ? decode13(forward<string>(s)) :
		TLS<SV>::decode(forward<string>(s));
}

template<bool SV> string TLS13<SV>::encode13(string &&s, int type)
{
	uint8_t seq[8];
	TLS_header h1;
	h1.content_type = 23;

	mpz2bnd(this->enc_seq_num_++, seq, seq + 8);
	const size_t chunk_size = (1 << 14) - 64;//cut string into 2^14
	string frag = s.substr(0, chunk_size) + string{type};
	h1.set_length(frag.size() + 16);

	uint8_t *p = (uint8_t*)&h1;
	this->aes_[SV].aad(p, 5);
	p = (uint8_t*)frag.data();
	this->aes_[SV].xor_with_iv(seq);
	auto tag = this->aes_[SV].encrypt(p, frag.size());
	this->aes_[SV].xor_with_iv(seq);
	frag += string{tag.begin(), tag.end()};
	string s2 = struct2str(h1) + frag;
	if(s.size() > chunk_size) s2 += encode(s.substr(chunk_size));
	return s2;
}

template<bool SV> optional<string> TLS13<SV>::decode13(string &&s) 
{
	struct H {
		TLS_header h1;
		unsigned char encrypted_msg[];
	} *p = (H*)s.data();
	uint8_t seq[8];

	if(int type = this->get_content_type(s).first; type != APPLICATION_DATA) {
		this->alert(this->alert(2, 10));
		return {};//alert case
	}
	mpz2bnd(this->dec_seq_num_++, seq, seq + 8);
	int msg_len = p->h1.get_length() - 16;//tag length 16
	
	this->aes_[!SV].aad((uint8_t*)p, 5);
	this->aes_[!SV].xor_with_iv(seq);
	auto auth = this->aes_[!SV].decrypt(p->encrypted_msg, msg_len);
	this->aes_[!SV].xor_with_iv(seq);
	if(equal(auth.begin(), auth.end(), p->encrypted_msg + msg_len)) {
		string r{p->encrypted_msg, p->encrypted_msg + msg_len};
		while(r.back() == 0) r.pop_back();
		if(r.back() == ALERT) {
			this->alert(this->alert(r[0], r[1]));
			return {};
		}
		r.pop_back();
		return r;
	} else {
		this->alert(this->alert(2, 20));
		return {};//bad record mac
	}
}
#pragma pack()	
