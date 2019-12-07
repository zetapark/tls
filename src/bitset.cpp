#include<cstring>
#include<iostream>
#include<iomanip>
#include"tls.h"
using namespace std;

int main() {
	TLS<true> server; TLS<false> client;
	server.client_hello(client.client_hello());
	client.server_hello(server.server_hello());
	client.server_certificate(server.server_certificate());
	client.server_key_exchange(server.server_key_exchange());
	client.server_hello_done(server.server_hello_done());
	server.client_key_exchange(client.client_key_exchange());
	server.change_cipher_spec(client.change_cipher_spec());
	server.finished(client.finished());
	client.change_cipher_spec(server.change_cipher_spec());
	client.finished(server.finished());//error
}
//void doub(unsigned char *p) {
//	bool bit1 = p[15] & 1;
//	for(int i=15; i>0; i--) p[i] = (p[i] >> 1) | (p[i-1] << 7) ;
//	p[0] >>= 1;
//	unsigned char c[16] = {0b11100001,};
//	if(bit1) for(int i=0; i<16; i++) p[i] ^= c[i];
//}
//void gf_mul(unsigned char *x, unsigned char *y)
//{//x = x * y
//	unsigned char z[16];
//	for(int i=0; i<16; i++) {
//		for(int j=0; j<8; j++) {
//			if(y[15-i] & (1<<j)) for(int k=0; k<16; k++) z[k] ^= x[k];
//			doub(x);
//		}
//	}
//	memcpy(z, x, 16);
//}
//
//int main() {
//	unsigned char ar[16] = {200,0,2,0,}, br[16] = {3, 2,}, cr[16] = {200,0,2,0,};
//	gf_mul(ar, br);
//	cout << hexprint("ar", ar) << endl;
//}
//int main() {
//	PRF<SHA2> prf;
//
//	mpz_class pre{"0xb7ae314639d66241247141db705b5d8573fc545ba83671380955b513042efae7"};
//	mpz_class rand{"0x68bbc97b168199de2496c062fd46e93a47dc9f9f12f074283e1b420d748a61cb85077072c2ce5701c6171e79c2786c7f551ff7898aaf516a145664d435f75de1"};
//	unsigned char secret[32], seed[64];
//	mpz2bnd(pre, secret, secret + 32);
//	mpz2bnd(rand, seed, seed + 64);
//	prf.secret(secret, secret + 32);
//	prf.seed(seed, seed + 64);
//	prf.label("master secret");
//	cout << hexprint("master secret", prf.get_n_byte(48)) << endl;
//
//	HMAC<SHA2> h1, h2;
//	h1.key(secret, secret + 32);
//	cout << hexprint("hash hmac", h1.hash(seed, seed + 64)) << endl;
//	cout << hexprint("hash hmac", h1.hash(seed, seed + 64)) << endl;
//	
//	const string data[] = {
//		"Sample message for keylen=blocklen",
//		"Sample message for keylen<blocklen",
//		"Sample message for keylen=blocklen",
//		"Sample message for keylen<blocklen, with truncated tag"
//	};
//	const char *key[] = {
//		"0x000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021\
//			22232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F",
//		"0x000102030405060708090A0B0C0D0E0F10111213",
//		"0x000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021\
//			22232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F4041424\
//			34445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F60616263",
//		"0x000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021\
//			22232425262728292A2B2C2D2E2F30"
//	};
//	const char *result[] = {"0x5FD596EE78D5553C8FF4E72D266DFD192366DA29",
//							"0x4C99FF0CB1B31BD33F8431DBAF4D17FCD356A807",
//							"0x2D51B2F7750E410584662E38F133435F4C4FD42A",
//							"0xFE3529565CD8E28C5FA79EAC9D8023B53B289D96"};
//
//	int data_len[] = {34, 34, 34, 54};
//	int key_len[] = {64, 20, 100, 49};
//	unsigned char nkey[100], nresult[32];
//
//	HMAC<SHA1> hmac;
//	for(int i=0; i<4; i++) {
//		mpz2bnd(mpz_class{key[i]}, nkey, nkey + key_len[i]);
//		mpz2bnd(mpz_class{result[i]}, nresult, nresult + 20);
//		hmac.key(nkey, nkey + key_len[i]);
//		auto a = hmac.hash(data[i].begin(), data[i].end());
//		assert(equal(a.begin(), a.end(), nresult));
//	}
//	auto a = hmac.hash(data[3].begin(), data[3].end());
//	assert(equal(a.begin(), a.end(), nresult));
//	assert(hmac.hash(data[2].begin(), data[2].end()) == hmac.hash(data[2].begin(), data[2].end()));
//}
//

