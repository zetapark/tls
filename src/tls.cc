#include<cstring>
#include<iostream>
#include<fstream>
#include"util/log.h"
#include"mpz.h"
#include"sha256.h"
#include"cert_util.h"
#include"prf.h"
#include"tls.h"
using namespace std;

mpz_class get_prvkey(istream& is);

//static member initialization
static mpz_class ze, zd, zK;//used in TLS constructor
static vector<unsigned char> to_byte(int k, int sz)
{
	vector<unsigned char> v(sz);
	mpz2bnd(k, v.begin(), v.end());
	return v;
}
static string init_certificate()
{//this will run before main -> use for initialization
	ifstream f2("key.pem");//generated with openssl genrsa 2048 > key.pem
	ifstream f("cert.pem");//openssl req -x509 -days 1000 -new -key key.pem -out cert.pem
	auto [K, e, d] = get_keys(f2);
	zK = K; ze = e; zd = d;
	vector<vector<unsigned char>> vv;

	for(string s; (s = get_certificate_core(f)) != "";) {
		vector<unsigned char> v;
		for(unsigned char c : base64_decode(s)) v.push_back(c);
		vv.push_back(v);
	}

	deque<unsigned char> r;
	for(const auto &v : vv) {
		for(unsigned char c : to_byte(v.size(), 3)) r.push_back(c);
		for(unsigned char c : v) r.push_back(c);
	}
	for(int i=0; i<2; i++) {
		auto v = to_byte(r.size(), 3);//total certificate size and 
		r.insert(r.begin(), v.begin(), v.end());//handshake header size
	}
	r.push_front(CERTIFICATE);//handshake type
	auto v = to_byte(r.size(), 2);
	r.insert(r.begin(), v.begin(), v.end());//tls header size
	r.push_front(3); r.push_front(3); r.push_front(HANDSHAKE);//tls header

	return {r.begin(), r.end()};
}
template<bool SV> string TLS<SV>::certificate_ = init_certificate();
template<bool SV> RSA TLS<SV>::rsa_{ze, zd, zK};
template class TLS<true>;//server
template class TLS<false>;//client

template<bool SV>
pair<int, int> TLS<SV>::get_content_type(const string &s)
{
	uint8_t *p = (uint8_t*)s.data();
	return {p[0], p[5]};
}
template<bool SV>
void TLS<SV>::generate_signature(unsigned char* pub_key, unsigned char* sign)
{
	unsigned char a[64 + 69];
	memcpy(a, client_random_.data(), 32);
	memcpy(a + 32, server_random_.data(), 32);
	memcpy(a + 64, pub_key, 69);
	//		auto b = server_mac_.hash(a, a + 70 + 3 * DH_KEY_SZ);
	SHA5 sha;
	auto b = sha.hash(a, a + 69);
	std::deque<unsigned char> dq{b.begin(), b.end()};
	unsigned char d[] = {0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
		0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04};
	dq.push_front(dq.size());
	dq.insert(dq.begin(), d, d + 16);
	dq.push_front(dq.size());
	dq.push_front(0x30);
	dq.push_front(0x00);
	while(dq.size() < 254) dq.push_front(0xff);
	dq.push_front(0x01);
	dq.push_front(0x00);
	//		3031300d060960864801650304020105000420
	//		3051300d060960864801650304020305000440		
	//		1ffff padding should be added in front of b;
	auto z = rsa_.sign(bnd2mpz(dq.begin(), dq.end()));//SIGPE
	mpz2bnd(z, sign, sign + 256);
}

template<bool SV>
void TLS<SV>::derive_keys(mpz_class premaster_secret)
{//K distribution OK, master secret derivation err
	unsigned char pre[32], rand[64];
	LOGD << hex << premaster_secret << endl;
	mpz2bnd(premaster_secret, pre, pre + 32);
	PRF<SHA2> prf;
	prf.secret(pre, pre + 32);
	memcpy(rand, client_random_.data(), 32);
	memcpy(rand + 32, server_random_.data(), 32);
	LOGD << hexprint("pre", pre) << endl;
	LOGD << hexprint("server random", server_random_) << endl;
	LOGD << hexprint("client random", client_random_) << endl;
	prf.seed(rand, rand + 64);
	prf.label("master secret");
	master_secret_ = prf.get_n_byte(48);
	LOGD << hexprint("master secret", master_secret_) << endl;//ok
	prf.secret(master_secret_.begin(), master_secret_.end());
	memcpy(rand, server_random_.data(), 32);
	memcpy(rand + 32, client_random_.data(), 32);
	prf.seed(rand, rand + 64);
	LOGD << hexprint("server random", server_random_) << endl;
	LOGD << hexprint("client random", client_random_) << endl;
	prf.label("key expansion");
	auto v = prf.get_n_byte(40);
	aes_[0].key(&v[0]);
	aes_[1].key(&v[16]);
	aes_[0].iv(&v[32], 0, 4);
	aes_[1].iv(&v[40], 0, 4);
	LOGD << hexprint("expanded keys", v) << endl;//different client, server
}
/*********
To generate the key material, compute
key_block = PRF(SecurityParameters.master_secret,
"key expansion",
SecurityParameters.server_random +
SecurityParameters.client_random);
until enough output has been generated.
 Then, the key_block is
partitioned as follows:
client_write_MAC_key[SecurityParameters.mac_key_length]
server_write_MAC_key[SecurityParameters.mac_key_length]
client_write_key[SecurityParameters.enc_key_length]
server_write_key[SecurityParameters.enc_key_length]
client_write_IV[SecurityParameters.fixed_iv_length]
server_write_IV[SecurityParameters.fixed_iv_length]
Currently, the client_write_IV and server_write_IV are only generated
for implicit nonce techniques as described in Section 3.2.1 of
[AEAD].
Implementation note: The currently defined cipher suite which
requires the most material is AES_256_CBC_SHA256. It requires 2 x 32
byte keys and 2 x 32 byte MAC keys, for a total 128 bytes of key
material.
************/
/******
 Then, the key_block is
partitioned as follows:
client_write_MAC_key[SecurityParameters.mac_key_length] 20
server_write_MAC_key[SecurityParameters.mac_key_length]
client_write_key[SecurityParameters.enc_key_length] 16
server_write_key[SecurityParameters.enc_key_length]
client_write_IV[SecurityParameters.fixed_iv_length] 16
server_write_IV[SecurityParameters.fixed_iv_length]
Currently, the client_write_IV and server_write_IV are only generated
for implicit nonce techniques as described in Section 3.2.1 of
[AEAD].
Implementation note: The currently defined cipher suite which
requires the most material is AES_256_CBC_SHA256. It requires 2 x 32
byte keys and 2 x 32 byte MAC keys, for a total 128 bytes of key
material.

Immediately after sending a ChangeCipherSpec message, the client will send an encrypted Handshake Finished message to ensure the server is able to understand the agreed-upon encryption. The message will contain a hash of all previous handshake messages, along with the string “client finished”. This is very important because it verifies that no part of the handshake has been tampered with by an attacker. It also includes the random bytes that were sent by the client and server, protecting it from replay attacks where the attacker pretends to be one of the parties.

Once received by the server, the server will acknowledge with its own ChangeCipherSpec message, followed immediately by its own Finished message verifying the contents of the handshake.

Note: if you have been following along in Wireshark, there appears to be a bug with Client/Server Finish messages when using AES_GCM that mislabels them.
Application Data

Finally, we can begin to transmit encrypted data! It may seem like a lot of work, but that is soon to pay off. The only remaining step is to discuss how the data is encrypted with AES_GCM, an AEAD cipher.

First, we generate a MAC, key, and IV for both the client and the server using our master secret and the PRF definition from earlier.

key_data = PRF(master_secret, "key expansion", server_random + client_random);

Since we are using 128-bit AES with SHA-256, we’ll pull out the following key data:

// client_write_MAC_key = key_data[0..31]
// server_write_MAC_key = key_data[32..63]
client_write_key = key_data[64..79]
server_write_key = key_data[80..95]
client_write_IV = key_data[96..99]
server_write_IV = key_data[100..103]

For AEAD ciphers like GCM, we don’t need the MAC keys, but we offset them anyways. The client and server also get different keys to prevent a replay attack where a client message it looped back to it.

We also construct additional_data and an 8-byte nonce, both of which are sent with the encrypted data. In the past, it was thought that the nonce could be either random or just a simple session counter. However, recent research found many sites using random nonces for AES_GCM were vulnerable to nonce reuse attacks, so it’s best to just use an incrementing counter tied to the session.

additional_data = sequence_num + record_type + tls_version + length
nonce = <random_8_bytes>

Finally, we can encrypt our data with AES GCM!

encrypted = AES_GCM(client_write_key, client_write_IV+nonce, <DATA>, additional_data)

and the server can read it with

<DATA> = AES_GCM(client_write_key, client_write_IV+nonce, encrypted, additional_data)


******/

#pragma pack(1)
struct TLS_header {
	uint8_t content_type = HANDSHAKE;  // 0x17 for Application Data, 0x16 handshake
	uint8_t version[2] = {0x03, 0x03};      // 0x0303 for TLS 1.2
	uint8_t length[2] = {0, 4};       //length of encrypted_data, 4 : handshake size
	void set_length(int k) { length[0] = k / 0x100; length[1] = k % 0x100; }
	int get_length() { return length[0] * 0x100 + length[1]; }
} ;
/*********************************
Record Protocol format

The TLS Record header comprises three fields, necessary to allow the higher layer to be built upon it:

    Byte 0: TLS record type

    Bytes 1-2: TLS version (major/minor)

    Bytes 3-4: Length of data in the record (excluding the header itself). The maximum supported is 16384 (16K).

             record type (1 byte)
            /
           /    version (1 byte major, 1 byte minor)
          /    /
         /    /         length (2 bytes)
        /    /         /
     +----+----+----+----+----+
     |    |    |    |    |    |
     |    |    |    |    |    | TLS Record header
     +----+----+----+----+----+


     Record Type Values       dec      hex
     -------------------------------------
     CHANGE_CIPHER_SPEC        20     0x14
     ALERT                     21     0x15
     HANDSHAKE                 22     0x16
     APPLICATION_DATA          23     0x17


     Version Values            dec     hex
     -------------------------------------
     SSL 3.0                   3,0  0x0300
     TLS 1.0                   3,1  0x0301
     TLS 1.1                   3,2  0x0302
     TLS 1.2                   3,3  0x0303
 *********************/
struct Handshake_header {
	uint8_t handshake_type;
	uint8_t length[3] = {0,0,0};
	void set_length(int k) {
		length[0] = k / 0x10000;
		length[1] = (k % 0x10000) / 0x100;
		length[2] = k % 0x100;
	}
	int get_length() { return length[0] * 0x10000 + length[1] * 0x100 + length[0]; }
} ;
/***********************
Handshake Protocol format

This is the most complex subprotocol within TLS. The specification focuses primarily on this, since it handles all the machinery necessary to establish a secure connection. The diagram below shows the general structure of Handshake Protocol messages. There are 10 handshake message types in the TLS specification (not counting extensions), so the specific format of each one will be described below.

                           |
                           |
                           |
         Record Layer      |  Handshake Layer
                           |                                  |
                           |                                  |  ...more messages
  +----+----+----+----+----+----+----+----+----+------ - - - -+--
  | 22 |    |    |    |    |    |    |    |    |              |
  |0x16|    |    |    |    |    |    |    |    |message       |
  +----+----+----+----+----+----+----+----+----+------ - - - -+--
    /               /      | \    \----\-----\                |
   /               /       |  \         \
  type: 22        /        |   \         handshake message length
                 /              type
                /
           length: arbitrary (up to 16k)


   Handshake Type Values    dec      hex
   -------------------------------------
   HELLO_REQUEST              0     0x00
   CLIENT_HELLO               1     0x01
   SERVER_HELLO               2     0x02
   CERTIFICATE               11     0x0b
   SERVER_KEY_EXCHANGE       12     0x0c
   CERTIFICATE_REQUEST       13     0x0d
   SERVER_DONE               14     0x0e
   CERTIFICATE_VERIFY        15     0x0f
   CLIENT_KEY_EXCHANGE       16     0x10
   FINISHED                  20     0x14
*******************/
struct Hello_header {
	uint8_t version[2] = {0x03, 0x03};//length is from here
	uint8_t random[32];
	uint8_t session_id_length = 32;
	uint8_t session_id[32];
};
template<bool SV> string TLS<SV>::accumulate(const string &s)
{
	accumulated_handshakes_ += s.substr(sizeof(TLS_header));
	return s;
}
template<bool SV> string TLS<SV>::client_hello(string&& s)
{//return desired id
	struct H {
		TLS_header h1;
		Handshake_header h2;
		Hello_header h3;
		uint8_t cipher_suite_length[2] = {0, 2};
		uint8_t cipher_suite[2] = {0xc0, 0x2f};//ECDHE RSA AES128 GCM SHA256
		uint8_t compression_length = 1;
		uint8_t compression_method = 0;//none
/*		uint8_t extension_length[2] = {0, 0};
		uint8_t secp256r1[] = {0, 0xa, 0, 4, 0, 2, 0, 0x17};
		uint8_t ec_format_uncompressed[] = {0, 0xb, 0, 2, 1, 0};
		uint8_t rsa_pkcs_sha512[] = {0, 0xd, 0, 4, 0, 2, 6, 1};
		uint8_t tls_version[] = {0, 0x2b, 0, 5, 4, 3, 4, 3, 3};//tls 1.3 1.2
		uint8_t key_share[] = {0, 0x33, 0, 71, 0, 69, 0, 0x17, 0, 65, 4};//
		uint8_t x[32], y[32];*/
	} r;
	if constexpr(!SV) {//if client
		r.h2.handshake_type = CLIENT_HELLO;
		r.h1.set_length(sizeof(r) - sizeof(TLS_header));
		r.h2.set_length(sizeof(r) - sizeof(TLS_header) - sizeof(Handshake_header));
		mpz2bnd(random_prime(32), r.h3.random, r.h3.random + 32);
		memcpy(client_random_.data(), r.h3.random, 32);//unix time + 28 random
		return accumulate(struct2str(r));
	} else {//server
		if(get_content_type(s) != pair{HANDSHAKE, CLIENT_HELLO}) return alert(2, 10);
		accumulate(s);
		H *p = (H*)s.data();
		memcpy(client_random_.data(), p->h3.random, 32);//unix time + 28 random
		int len = 0x100 * p->cipher_suite_length[0] + p->cipher_suite_length[1];
		for(int i=0; i<len; i+=2) 
			if(p->cipher_suite[i] == 0xc0 && p->cipher_suite[i+1] == 0x2f)
				return "";
		return alert(2, 40);
	}
}
/*****************
ClientHello: This message typically begins a TLS handshake negotiation. It is sent with a list of client-supported cipher suites, for the server to pick the best suiting one (preferably the strongest), a list of compression methods, and a list of extensions. It gives also the possibility to the client of restarting a previous session, through the inclusion of a SessionId field.

     |
     |
     |
     |  Handshake Layer
     |
     |
- ---+----+----+----+----+----+----+------+----+----------+--------+-----------+----------+
     |  1 |    |    |    |    |    |32-bit|    |max 32-bit| Cipher |Compression|Extensions|
     |0x01|    |    |    |  3 |  1 |random|    |session Id| Suites |  methods  |          |
- ---+----+----+----+----+----+----+------+----+----------+--------+-----------+----------+
  /  |  \    \---------\    \----\             \       \
 /       \        \            \                \   SessionId
record    \     length        SSL/TLS            \
length     \                  version         SessionId
            type: 1       (TLS 1.0 here)       length



CipherSuites

+----+----+----+----+----+----+
|    |    |    |    |    |    |
|    |    |    |    |    |    |
+----+----+----+----+----+----+
  \-----\   \-----\    \----\
     \         \          \
      length    cipher Id  cipherId


Compression methods (no practical implementation uses compression)

+----+----+----+
|    |    |    |
|  0 |  1 |  0 |
+----+----+----+
  \-----\    \
     \        \
 length: 1    cmp Id: 0


Extensions

+----+----+----+----+----+----+----- - -
|    |    |    |    |    |    |
|    |    |    |    |    |    |...extension data
+----+----+----+----+----+----+----- - -
  \-----\   \-----\    \----\
     \         \          \
    length    Extension  Extension data
                 Id          length

***************************/
template<bool SV> string TLS<SV>::server_hello(string &&s)
{
	struct H {
		TLS_header h1;
		Handshake_header h2;
		Hello_header h3;
		uint8_t cipher_suite[2] = {0xc0, 0x2f};
		uint8_t compression = 0;
		//uint8_t extension_length[2] = {0, 0};
	} r;
	if constexpr(SV) {
		r.h1.length[1] = sizeof(Hello_header) + sizeof(Handshake_header) + 3;
		r.h2.length[2] = sizeof(Hello_header) + 3;
		r.h2.handshake_type = SERVER_HELLO;
		mpz2bnd(random_prime(32), server_random_.begin(), server_random_.end());
		memcpy(r.h3.random, server_random_.data(), 32);
		memcpy(r.h3.session_id, session_id_.data(), 32);
		return accumulate(struct2str(r));
	} else {
		if(get_content_type(s) != pair{HANDSHAKE, SERVER_HELLO}) 
			return alert(2, 10);
		accumulate(s);
		H *p = (H*)s.data();
		memcpy(server_random_.data(), p->h3.random, 32);
		memcpy(session_id_.data(), p->h3.session_id, 32);
		if(p->cipher_suite[0] == 0xc0 && p->cipher_suite[1] == 0x2f) return "";
		else return alert(2, 40);
	}
}
/**************
(00,33)DHE-RSA-AES128-SHA : 128 Bit Key exchange: DH, encryption: AES, MAC: SHA1.
(00,67)DHE-RSA-AES128-SHA256 : 128 Bit Key exchange: DH, encryption: AES, MAC: SHA256.
(00,39)DHE-RSA-AES256-SHA : 256 Bit Key exchange: DH, encryption: AES, MAC: SHA1.
(00,6b)DHE-RSA-AES256-SHA256 : 256 Bit Key exchange: DH, encryption: AES, MAC: SHA256.

ServerHello: The ServerHello message is very similar to the ClientHello message, with the exception that it only includes one CipherSuite and one Compression method. If it includes a SessionId (i.e. SessionId Length is > 0), it signals the client to attempt to reuse it in the future.

     |
     |
     |
     |  Handshake Layer
     |
     |
- ---+----+----+----+----+----+----+----------+----+----------+----+----+----+----------+
     |  2 |    |    |    |    |    |  32byte  |    |max 32byte|    |    |    |Extensions|
     |0x02|    |    |    |  3 |  1 |  random  |    |session Id|    |    |    |          |
- ---+----+----+----+----+----+----+----------+----+----------+--------------+----------+
  /  |  \    \---------\    \----\               \       \       \----\    \
 /       \        \            \                  \   SessionId      \  Compression
record    \     length        SSL/TLS              \ (if length > 0)  \   method
length     \                  version           SessionId              \
            type: 2       (TLS 1.0 here)         length            CipherSuite
****************/
template<bool SV> string TLS<SV>::server_certificate(string&& s)
{
	if constexpr(SV) return accumulate(certificate_);
	else {
		if(get_content_type(s) != pair{HANDSHAKE, CERTIFICATE}) 
			return alert(2, 10);
		accumulate(s);
		struct H {
			TLS_header h1;
			Handshake_header h2;
			uint8_t certificate_length[2][3];//total len + first cert len
			unsigned char certificate[];//first cert
		} *p = (H*)s.data();
		std::stringstream ss;
		uint8_t *q = p->certificate_length[1];
		for(int i=0, j = *q * 0x10000 + *(q+1) * 0x100 + *(q+2); i < j; i++) 
			ss << noskipws << p->certificate[i];//first certificate
		Json::Value jv;
		try {
			jv = der2json(ss);
		} catch(const char *e) {
			cerr << "certificate error : " << e << '\n';
			return alert(2, 44);
		}
//		LOGD << jv << endl;//LOG cannot be used!!
		auto [K, e, sign] = get_pubkeys(jv);

		LOGD << "K : " << K << endl;
		LOGD << "e : " << e << endl;
		LOGD << "sign : " << sign << endl;
//		*plog << jv << std::endl << std::hex << powm(sign, e, K) << std::endl;
		rsa_.K = K; rsa_.e = e;
		return "";
	}
}

/************************
Structure of this message:

opaque ASN.1Cert<1..2^24-1>;

struct {
	ASN.1Cert certificate_list<0..2^24-1>;
} Certificate;

Certificate: The body of this message contains a chain of public key certificates. Certificate chains allows TLS to support certificate hierarchies and PKIs (Public Key Infrastructures).

     |
     |
     |
     |  Handshake Layer
     |
     |
- ---+----+----+----+----+----+----+----+----+----+----+-----------+---- - -
     | 11 |    |    |    |    |    |    |    |    |    |           |
     |0x0b|    |    |    |    |    |    |    |    |    |certificate| ...more certificate
- ---+----+----+----+----+----+----+----+----+----+----+-----------+---- - -
  /  |  \    \---------\    \---------\    \---------\
 /       \        \              \              \
record    \     length      Certificate    Certificate
length     \                   chain         length
            type: 11           length
***************************/
/************************
CertificateRequest: It is used when the server requires client identity authentication. Not commonly used in web servers, but very important in some cases. The message not only asks the client for the certificate, it also tells which certificate types are acceptable. In addition, it also indicates which Certificate Authorities are considered trustworthy.

     |
     |
     |
     |  Handshake Layer
     |
     |
- ---+----+----+----+----+----+----+---- - - --+----+----+----+----+-----------+-- -
     | 13 |    |    |    |    |    |           |    |    |    |    |    C.A.   |
     |0x0d|    |    |    |    |    |           |    |    |    |    |unique name|
- ---+----+----+----+----+----+----+---- - - --+----+----+----+----+-----------+-- -
  /  |  \    \---------\    \    \                \----\   \-----\
 /       \        \          \ Certificate           \        \
record    \     length        \ Type 1 Id        Certificate   \
length     \             Certificate         Authorities length \
            type: 13     Types length                         Certificate Authority
                                                                      length
*********************/
template<bool SV> string TLS<SV>::server_key_exchange(string&& s)
{
	struct H {
		TLS_header h1;
		Handshake_header h2;
		uint8_t named_curve = 3,
				secp256r[2] = {0, 0x17},
				key_length = 65,
				uncommpressed = 4,
				x[32], y[32];
		uint8_t signature_hash = 6, //SHA512
				signature_sign = 1, //rsa
				signature_length[2] = {1, 0}, sign[256];
		/*enum { none(0), md5(1), sha1(2), sha224(3), sha256(4), sha384(5), sha512(6), (255) } HashAlgorithm;
		  enum { anonymous(0), rsa(1), dsa(2), ecdsa(3), (255) } SignatureAlgorithm;*/
	} r;

	if constexpr(SV) {
		r.h1.set_length(sizeof(r) - sizeof(TLS_header));
		r.h2.set_length(sizeof(r) - sizeof(TLS_header) - sizeof(Handshake_header));
		r.h2.handshake_type = SERVER_KEY_EXCHANGE;
		mpz2bnd(P_.x, r.x, r.x+32);
		mpz2bnd(P_.y, r.y, r.y+32);
		generate_signature(&r.named_curve, r.sign);
		return accumulate(struct2str(r));
	} else {
		if(get_content_type(s) != pair{HANDSHAKE, SERVER_KEY_EXCHANGE})
			return alert(2, 10);
		accumulate(s);
		const H *p = reinterpret_cast<const H*>(s.data());
		EC_Point Y{bnd2mpz(p->x, p->x+32), bnd2mpz(p->y, p->y+32), secp256r1_};
		derive_keys((Y * prv_key_).x);
		return "";
	}	
}
/************************
ServerKeyExchange: This message carries the keys exchange algorithm parameters that the client needs from the server in order to get the symmetric encryption working thereafter. It is optional, since not all key exchanges require the server explicitly sending this message. Actually, in most cases, the Certificate message is enough for the client to securely communicate a premaster key with the server. The format of those parameters depends exclusively on the selected CipherSuite, which has been previously set by the server via the ServerHello message.

     |
     |
     |
     |  Handshake Layer
     |
     |
- ---+----+----+----+----+----------------+
     | 12 |    |    |    |   algorithm    |
     |0x0c|    |    |    |   parameters   |
- ---+----+----+----+----+----------------+
  /  |  \    \---------\
 /       \        \
record    \     length
length     \
            type: 12

struct {
select (KeyExchangeAlgorithm) {
	case dh_anon:
		ServerDHParams params;
	case dhe_dss:
	case dhe_rsa:
		ServerDHParams params;
		digitally-signed struct {
			opaque client_random[32];
			opaque server_random[32];
			ServerDHParams params;
		} signed_params;
	case rsa:
	case dh_dss:
	case dh_rsa:
	struct {} ;
		 message is omitted for rsa, dh_dss, and dh_rsa 
		 may be extended, e.g., for ECDH -- see [TLSECC] 
};
} ServerKeyExchange;
params
The server’s key exchange parameters.
signed_params
For non-anonymous key exchanges, a signature over the server’s
key exchange parameters.
*********************/
template<bool SV> string TLS<SV>::server_hello_done(string&& s)
{
	struct {
		TLS_header h1;
		Handshake_header h2;
	} r;
	if constexpr(SV) {
		r.h2.handshake_type = SERVER_DONE;
		return accumulate(struct2str(r));
	} else {
		if(get_content_type(s) != pair{HANDSHAKE, SERVER_DONE}) return alert(2,10);
		accumulate(s);
		return "";
	}
}
/*****************************
ServerHelloDone: This message finishes the server part of the handshake negotiation. It does not carry any additional information.

     |
     |
     |
     |  Handshake Layer
     |
     |
- ---+----+----+----+----+
     | 14 |    |    |    |
   4 |0x0e|  0 |  0 |  0 |
- ---+----+----+----+----+
  /  |  \    \---------\
 /       \        \
record    \     length: 0
length     \
            type: 14
*********************/
template<bool SV> string TLS<SV>::change_cipher_spec(string &&s)
{
	struct {
		TLS_header h1;
		uint8_t spec = 1;
	} r;
	r.h1.content_type = CHANGE_CIPHER_SPEC;
	r.h1.length[1] = 1;
	return s == "" ? struct2str(r) : "";
}

template<bool SV> string TLS<SV>::client_key_exchange(string&& s)//16
{//return client_aes_key + server_aes_key
	struct H {
		TLS_header h1;
		Handshake_header h2;
		uint8_t len = 65;
		uint8_t uncommpressed = 4;
		uint8_t x[32], y[32];
	} r;

	if constexpr(SV) {
		if(get_content_type(s) != pair{HANDSHAKE, CLIENT_KEY_EXCHANGE}) 
			return alert(2, 10);
		accumulate(s);
		H* p = (H*)s.data();
		EC_Point Y{bnd2mpz(p->x, p->x+32), bnd2mpz(p->y, p->y+32), secp256r1_};
		derive_keys((Y * prv_key_).x);
		return "";
	} else {
		r.h2.handshake_type = 16;
		r.h1.set_length(sizeof(H) - sizeof(TLS_header));
		r.h2.set_length(sizeof(H) - sizeof(TLS_header) - sizeof(Handshake_header));
		mpz2bnd(P_.x, r.x, r.x+32);
		mpz2bnd(P_.y, r.y, r.y+32);
		return accumulate(struct2str(r)); 
	}
}
/*****************************
ClientKeyExchange: It provides the server with the necessary data to generate the keys for the symmetric encryption. The message format is very similar to ServerKeyExchange, since it depends mostly on the key exchange algorithm picked by the server.

     |
     |
     |
     |  Handshake Layer
     |
     |
- ---+----+----+----+----+----------------+
     | 16 |    |    |    |   algorithm    |
     |0x10|    |    |    |   parameters   |
- ---+----+----+----+----+----------------+
  /  |  \    \---------\
 /       \        \
record    \     length
length     \
            type: 16

until enough output has been generated.
**********************/
template<bool SV> string TLS<SV>::decode(string &&s)
{
	struct H {
		TLS_header h1;
		uint8_t iv[8];
		unsigned char m[];
	} *p = (H*)s.data();
	struct {
		uint8_t seq[8];
		TLS_header h1;
	} header_for_mac;

	LOGD << hexprint("decoding", s) << endl;

	mpz2bnd(dec_seq_num_++, header_for_mac.seq, header_for_mac.seq + 8);
	header_for_mac.h1 = p->h1;
	int msg_len = p->h1.get_length() - sizeof(H::iv) - 16;//tag length 16
	header_for_mac.h1.set_length(msg_len);
//	header_for_mac.h1.set_length(content.size());
	string t = struct2str(header_for_mac);
	LOGD << hexprint("aad from decode", t) << endl;
	aes_[!SV].aad({t.begin(), t.end()});
//	assert(auth == mac_[!SV].hash(t.begin(), t.end()));//verify auth

	aes_[!SV].iv(p->iv, 4, 8);
	auto auth = aes_[!SV].decrypt(p->m, msg_len);//here key value is changed(the other key?)
	LOGD << hexprint("calculated auth", auth) << endl;
	LOGD << hexprint("attached auth", vector<unsigned char>{p->m + msg_len, p->m + msg_len + 16}) << endl;
	LOGD << "decoded : " << p->m << endl;
	if(equal(auth.begin(), auth.end(), p->m + msg_len)) LOGI << "mac verified" << endl;
	else LOGE << "mac error" << endl;
	return {p->m, p->m + msg_len};
}
/***********************
ApplicationData Protocol format

The mission of this protocol is to properly encapsulate the data coming from the Application Layer of the network stack, so it can seamlessly be handled by the underlying protocol (TCP) without forcing changes in any of those layers. The format of the messages in this protocols follows the same structure as the previous ones.

                           |
                           |
                           |
         Record Layer      |  ApplicationData Layer (encrypted)
                           |
                           |
  +----+----+----+----+----+----+----+--- - - - - - - --+---------+
  | 23 |    |    |    |       length-delimited data     |         |
  |0x17|    |    |    |    |    |    |                  |   MAC   |
  +----+----+----+----+----+----+----+--- - - - - - - --+---------+
    /               /      |
   /               /       |
  type: 23        /        |
                 /
                /
           length: arbitrary (up to 16k)
******************/
template<bool SV> string TLS<SV>::encode(string &&s, int type)
{
	struct {
		TLS_header h1;
		uint8_t iv[8];
	} header_to_send;
	struct {
		uint8_t seq[8];
		TLS_header h1;
	} header_for_mac;
	header_for_mac.h1.content_type = header_to_send.h1.content_type = type;

	mpz2bnd(enc_seq_num_++, header_for_mac.seq, header_for_mac.seq + 8);
	const size_t chunk_size = (1 << 14) - 64;//cut string into 2^14
	int len = min(s.size(), chunk_size);
//	int block_len = (len/16 + 2) * 16;//consider auth tag
	header_for_mac.h1.set_length(len);
	string frag = s.substr(0, len);
//	string s2 = struct2str(header_for_mac) + frag;
//	auto verify = mac_[SV].hash(s2.begin(), s2.end());
//	LOGD << hexprint("mac verify", verify) << endl;
//	frag += string{verify.begin(), verify.end()};//add authentication
//	while(frag.size() != block_len) frag += (char)(block_len - len - 21);//padding

	mpz2bnd(random_prime(8), header_to_send.iv, header_to_send.iv + 8);
	aes_[SV].iv(header_to_send.iv, 4, 8);
	auto aad = struct2str(header_for_mac);
	LOGD << hexprint("aad", aad) << endl;
	aes_[SV].aad({aad.begin(), aad.end()});
	auto tag = aes_[SV].encrypt(reinterpret_cast<unsigned char*>(&frag[0]), frag.size());
	LOGD << hexprint("auth tag", tag) << endl;
	frag += string{tag.begin(), tag.end()};
	header_to_send.h1.set_length(sizeof(header_to_send.iv) + frag.size());
	string s2 = struct2str(header_to_send) + frag;
	LOGD << hexprint("sending", s2) << endl;
	if(s.size() > chunk_size) s2 += encode(s.substr(chunk_size));
	return s2;
}
/***************
The MAC is generated as:
MAC(MAC_write_key, seq_num + TLSCompressed.type + TLSCompressed.version +
	TLSCompressed.length + TLSCompressed.fragment);
where "+" denotes concatenation.

struct {
	opaque IV[SecurityParameters.record_iv_length];//16
	block-ciphered struct {
		opaque content[TLSCompressed.length];//len
		opaque MAC[SecurityParameters.mac_length];//20
		uint8 padding[GenericBlockCipher.padding_length];//pad
		uint8 padding_length;//1
	};
} GenericBlockCipher;
*********************/
template<bool SV> string TLS<SV>::finished(string &&s)
{//finished message to send(s == "") and receive(s == recv())
	PRF<SHA2> prf; SHA2 sha;
	prf.secret(master_secret_.begin(), master_secret_.end());
	auto h = sha.hash(accumulated_handshakes_.cbegin(), accumulated_handshakes_.cend());
	prf.seed(h.begin(), h.end());
	const char *label[2] = {"client finished", "server finished"};
	prf.label(label[s == "" ? SV : !SV]);
	auto v = prf.get_n_byte(12);
	LOGD << hexprint("finished", v) << endl;

	Handshake_header hh;
	hh.handshake_type = FINISHED;
	hh.set_length(12);
	
	string msg = struct2str(hh) + string{v.begin(), v.end()};
	accumulated_handshakes_ += msg;
	
	if(s == "") return encode(move(msg), HANDSHAKE);
	else if(decode(move(s)) != msg) return alert(2, 51);
	else return "";
}
/***********************
Finished: This message signals that the TLS negotiation is complete and the CipherSuite is activated. It should be sent already encrypted, since the negotiation is successfully done, so a ChangeCipherSpec protocol message must be sent before this one to activate the encryption. The Finished message contains a hash of all previous handshake messages combined, followed by a special number identifying server/client role, the master secret and padding. The resulting hash is different from the CertificateVerify hash, since there have been more handshake messages.

|
|
|
|  Handshake Layer
|
|
- ---+----+----+----+----+----------+
| 20 |    |    |    |  signed  |
|0x14|    |    |    |   hash   |
- ---+----+----+----+----+----------+
/  |  \    \---------\
/       \        \
record    \     length
length     \
type: 20

change cipher spec after
The Finished messages have contents which are computed with, again, the PRF.
The contents of the Finished message are 12 bytes obtained by invoking the PRF with, as "secret" input, the master secret; the "seed" is the hash of all previous handshake messages.
The "label" differs depending on who, between the client and the server, sends that specific Finished message.
Since the Finished messages are sent after the ChangeCipherSpec, they are encrypted and MACed, using the algorithms and keys which have just been negotiated.
In that sense, the keys and nonces (the client and server randoms) are involved multiple times.
 *********************/
template<bool SV> string TLS<SV>::alert(uint8_t level, uint8_t desc)
{//encrypted send => encode(alert(2, 20).substr(sizeof(TLS_header)), 0x15)
	struct {
		TLS_header h1;
		uint8_t alert_level;
		uint8_t alert_desc;
	} h;
	h.h1.content_type = ALERT;
	h.alert_level = level;
	h.alert_desc = desc;
	h.h1.set_length(2);
	return struct2str(h);
}
template<bool SV> int TLS<SV>::alert(string &&s)
{//alert received
	struct H {
		TLS_header h1;
		uint8_t alert_level;
		uint8_t alert_desc;
	} *p = (H*)s.data();
	int level, desc;
	if(p->h1.get_length() == 2) {
		level = p->alert_level;
		desc = p->alert_desc;
	} else {//encrypted
		s = decode(move(s));
		level = static_cast<uint8_t>(s[0]);
		desc = static_cast<uint8_t>(s[1]);
	}
	switch(desc) {//s reuse
		case 0: s = "close_notify(0)"; break;
		case 10: s = "unexpected_message(10)"; break;
		case 20: s = "bad_record_mac(20)"; break;
		case 21: s = "decryption_failed_RESERVED(21)"; break;
		case 22: s = "record_overflow(22)"; break;
		case 30: s = "decompression_failure(30)"; break;
		case 40: s = "handshake_failure(40)"; break;
		case 41: s = "no_certificate_RESERVED(41)"; break;
		case 42: s = "bad_certificate(42)"; break;
		case 43: s = "unsupported_certificate(43)"; break;
		case 44: s = "certificate_revoked(44)"; break;
		case 45: s = "certificate_expired(45)"; break;
		case 46: s = "certificate_unknown(46)"; break;
		case 47: s = "illegal_parameter(47)"; break;
		case 48: s = "unknown_ca(48)"; break;
		case 49: s = "access_denied(49)"; break;
		case 50: s = "decode_error(50)"; break;
		case 51: s = "decrypt_error(51)"; break;
		case 60: s = "export_restriction_RESERVED(60)"; break;
		case 70: s = "protocol_version(70)"; break;
		case 71: s = "insufficient_security(71)"; break;
		case 80: s = "internal_error(80)"; break;
		case 90: s = "user_canceled(90)"; break;
		case 100: s = "no_renegotiation(100)"; break;
		case 110: s = "unsupported_extension(110)"; break;
	}
	if(level == 1) LOGI << s << endl;
	else if(level == 2) LOGI << s << endl;
	return desc;
}

//template<bool SV> void
//TLS<SV>::handshake(function<string(void)> read_f, function<void(string)> write_f)
//{
//	string s;
//	if constexpr(SV) {
//		switch(CLIENT_HELLO) {
//		case CLIENT_HELLO:
//			if(s = client_hello(read_f()); s != "") {//error -> alert return
//				write_f(s);					LOGE << "handshake failed" << endl;
//				break;
//			} else 							LOGI << "client hello" << endl;
//		case SERVER_HELLO:
//			s = server_hello(); 			LOGI << "server hello" << endl;
//			s += is_tls12() ? //if tls13, encode starts here
//				server_certificate() : encode(server_certificate(), HANDSHAKE);
//											LOGI << "server certificate" << endl;
//			if(is_tls12()) {
//				s += server_key_exchange();	LOGI << "server key exchange" << endl;
//				s += server_hello_done();	LOGI << "server hello done" << endl;
//			} else s += finished();//finished is already encoded
//			write_f(s);
//		case CLIENT_KEY_EXCHANGE:
//			if(is_tls12()) {
//				if(s = client_key_exchange(read_f()); s != "") {
//					write_f(s);				LOGE<<"client key exchange failed"<<endl;
//					break;
//				} else 						LOGI << "client key exchange" << endl;
//				change_cipher_spec(read_f());LOGI << "change cipher spec" << endl;
//			}
//			if(s = finished(read_f()); s != "") {
//				write_f(s); 				LOGE << "decrypt error" << endl;
//				break;
//			} else 							LOGI << "client finished" << endl;
//		case CHANGE_CIPHER_SPEC:
//			if(is_tls12()) {
//				s = change_cipher_spec(); 	LOGI << "change cipher spec" << endl;
//				s += finished(); 			LOGI << "server finished" << endl;
//				write_f(s);
//			}
//		}
//	} else {
//		switch(CLIENT_HELLO) {
//		case CLIENT_HELLO:
//			write_f(client_hello());
//		case SERVER_HELLO:
//			if(s = server_hello(read_f()); s != "") {//error -> alert return
//				write_f(s);					LOGE << "handshake failed" << endl;
//				break;
//			} else 							LOGI << "server hello" << endl;
//			if(is_tls12()) server_certificate(read_f());
//			else server_certificate(decode(read_f()));
//			if(is_tls12()) {
//				server_key_exchange(read_f());	LOGI << "server key exchange" << endl;
//				server_hello_done(read_f());LOGI << "server hello done" << endl;
//			} else finished(read_f());//finished is already encoded
//		case CLIENT_KEY_EXCHANGE:
//			if(is_tls12()) {
//				write_f(client_key_exchange());	LOGE<<"client key exchange failed"<<endl;
//				write_f(change_cipher_spec());	LOGI << "change cipher spec" << endl;
//			}
//			write_f(finished());			LOGI << "client finished" << endl;
//		case CHANGE_CIPHER_SPEC:
//			if(is_tls12()) {
//				change_cipher_spec(read_f());LOGI << "change cipher spec" << endl;
//				finished(read_f()); 		LOGI << "server finished" << endl;
//			}
//		}
//
//	}
//}
#pragma pack()
