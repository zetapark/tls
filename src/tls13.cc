#include<utility>
#include"tls13.h"
using namespace std;

template class TLS13<true>;
template class TLS13<false>;

#pragma pack(1)

template<bool SV> string TLS13<SV>::client_ext() {
	struct Ext {
		uint8_t extension_length[2] = {0, 0};

		uint8_t supported_group[2] = {0, 10};//type
		uint8_t supported_group_length[2] = {0, 4};//length
		uint8_t support_group_list_length[2] = {0, 2};
		uint8_t secp256r1[2] = {0, 23};
		
		uint8_t ec_point_format[2] = {0, 11};//type
		uint8_t ec_point_format_length[2] = {0, 2};//length
		uint8_t ec_length = 1;
		uint8_t non_compressed = 0;

		uint8_t key_share[2] = {0, 51};//type
		uint8_t key_share_length[2] = {0, 71};//length
		uint8_t client_key_share_len[2] = {0, 69};
		uint8_t secp256r1_key[2] = {0, 23};
		uint8_t key_length[2] = {0, 65};
		uint8_t type = 4;
		uint8_t x[32], y[32];

		uint8_t supported_version[2] = {0, 0x2b};
		uint8_t supported_version_length[2] = {0, 5};
		uint8_t supported_version_list_length = 4;
		uint8_t supported_versions[4] = {3, 4, 3, 3};//TLS 1.3, TLS 1.2

		uint8_t psk_mode[2] = {0, 0x2d};
		uint8_t psk_mode_length[2] = {0, 2};
		uint8_t psk_mode_llength = 1;
		uint8_t psk_with_ecdhe = 1;

		uint8_t signature_algorithm[2] = {0, 13};
		uint8_t signature_algorithm_length[2] = {0, 4};
		uint8_t signature_alg_len[2] = {0, 2};
		uint8_t signature[2] = {6, 1};//sha512 rsa
	} ext;
	mpz2bnd(this->P_.x, ext.x, ext.x + 32);
	mpz2bnd(this->P_.y, ext.y, ext.y + 32);
	mpz2bnd(sizeof(Ext) - 2, ext.extension_length, ext.extension_length + 2);
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

		uint8_t key_share[2] = {0, 51};
		uint8_t key_share_length[2] = {0, 69};
		uint8_t type[2] = {0, 23};
		uint8_t key_length[2] = {0, 65};
		uint8_t point_type = 4;
		uint8_t x[32], y[32];
	} ext;
	mpz2bnd(this->P_.x, ext.x, ext.x + 32);
	mpz2bnd(this->P_.y, ext.y, ext.y + 32);
	return struct2str(ext);
}

template<bool SV> void TLS13<SV>::protect_handshake()
{//call after server hello
	hkdf_.zero_salt();
	uint8_t psk[SHA2::output_size] = {0,}, pre[32];
	auto early_secret = hkdf_.extract(psk, SHA2::output_size);
	hkdf_.salt(&early_secret[0], early_secret.size());
	auto a = hkdf_.derive_secret("derived", "");
	hkdf_.salt(&a[0], a.size());
	mpz2bnd(premaster_secret_, pre, pre + 32);
	auto handshake_secret = hkdf_.extract(pre, 32);
	set_aes(handshake_secret, "c hs traffic", "s hs traffic");
	hkdf_.salt(&handshake_secret[0], handshake_secret.size());
	a = hkdf_.derive_secret("derived", "");
	hkdf_.salt(&a[0], a.size());
	this->master_secret_ = hkdf_.extract(psk, SHA2::output_size);
}

template<bool SV> void TLS13<SV>::protect_data()
{//call after serverfinished
	set_aes(this->master_secret_, "c ap traffic", "s ap traffic");
}

template<bool SV>
void TLS13<SV>::set_aes(vector<uint8_t> salt, string cl, string sv) {
	hkdf_.salt(&salt[0], salt.size());
	vector<unsigned char> secret[2];
	secret[0] = hkdf_.derive_secret(cl, this->accumulated_handshakes_);
	secret[1] = hkdf_.derive_secret(sv, this->accumulated_handshakes_);
	for(int i=0; i<2; i++) {
		hkdf_.salt(&secret[i][0], secret[i].size());
		auto key = hkdf_.expand_label("key", "", 16);
		auto iv = hkdf_.expand_label("iv", "", 4);
		this->aes_[i].key(&key[0]);
		this->aes_[i].iv(&iv[0], 0, iv.size());
	}
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
		int ext_start = static_cast<uint8_t>(s[76]) * 0x100 + static_cast<uint8_t>(s[77]) + 80;//cipher suite length + 80
		string r = TLS<SV>::client_hello(forward<string>(s));
		return s.size() > ext_start && client_ext((uint8_t*)&s[ext_start]) ? "" : r;
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

template<bool SV> bool
TLS13<SV>::handshake(function<string()> read_f, function<void(string)> write_f)
{//handshake according to compromised version
	string s;
	switch(1) { case 1://to use break
		if constexpr(SV) {
			if(s = client_hello(read_f()); s != "") break;
			if(s = this->server_hello(); premaster_secret_) {
				protect_handshake();
				s += this->encode(this->server_certificate(), HANDSHAKE);
				s += this->finished();//finished is already encoded
				write_f(s);
				string tmp = this->accumulated_handshakes_;
				if(s = this->finished(read_f()); s != "") break;//client finished
				this->accumulated_handshakes_ = tmp;
				protect_data();
			} else {
				s += this->server_certificate();
				s += this->server_key_exchange();
				s += this->server_hello_done();	
				write_f(s);
				if(s = this->client_key_exchange(read_f()); s != "") break;
				if(s = this->change_cipher_spec(read_f()); s != "") break;
				if(s = this->finished(read_f()); s != "") break;
				s = this->change_cipher_spec();
				s += this->finished();
				write_f(move(s));//empty s
			} 
		} else {
			write_f(client_hello());
			if(s = this->server_hello(read_f()); s != "") break;
			if(premaster_secret_) {
				protect_handshake();//should prepend header?
				if(s = this->server_certificate(this->decode(read_f())); s != "") break;
				if(s = this->finished(read_f()); s != "") break;
				string tmp = this->accumulated_handshakes_;
				write_f(this->finished());
				this->accumulated_handshakes_ = tmp;
				protect_data();
			} else {
				if(s = this->server_certificate(read_f()); s != "") break;
				if(s = this->server_key_exchange(read_f());	s != "") break;
				if(s = this->server_hello_done(read_f()); s != "") break;
				s = this->client_key_exchange();
				s += this->change_cipher_spec();
				s += this->finished();
				write_f(move(s));//empty s
				if(s = this->change_cipher_spec(read_f()); s != "") break;
				if(s = this->finished(read_f()); s != "") break;
			} 
		}
	}
	if(s != "") {
		write_f(s);//send alert message
		return false;
	} else return true;
}
#pragma pack()	
