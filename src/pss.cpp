#include<iostream>
#include"sha1.h"
#include"pss.h"
#include"mpz.h"
#include"ecdsa.h"
using namespace std;

int main()
{
	mpz_class K{"0xa2ba40ee07e3b2bd2f02ce227f36a195024486e49c19cb41bbbdfbba98b22b0e577c2eeaffa20d883a76e65e394c69d4b3c05a1e8fadda27edb2a42bc000fe888b9b32c22d15add0cd76b3e7936e19955b220dd17d4ea904b1ec102b2e4de7751222aa99151024c7cb41cc5ea21d00eeb41f7c800834d2c6e06bce3bce7ea9a5"};
	mpz_class e{"0x010001"};
	mpz_class d{"0x050e2c3e38d886110288dfc68a9533e7e12e27d2aa56d2cdb3fb6efa990bcff29e1d2987fb711962860e7391b1ce01ebadb9e812d2fbdfaf25df4ae26110a6d7a26f0b810f54875e17dd5c9fb6d641761245b81e79f8c88f0e55a6dcd5f133abd35f8f4ec80adf1bf86277a582894cb6ebcd2162f1c7534f1f4947b129151b71"};
	uint8_t m[114] = {0x85, 0x9e, 0xef, 0x2f, 0xd7, 0x8a, 0xca, 0x00, 0x30,
		0x8b, 0xdc, 0x47, 0x11, 0x93, 0xbf, 0x55, 0xbf, 0x9d, 0x78, 0xdb, 0x8f,
		0x8a, 0x67, 0x2b, 0x48, 0x46, 0x34, 0xf3, 0xc9, 0xc2, 0x6e, 0x64, 0x78,
		0xae, 0x10, 0x26, 0x0f, 0xe0, 0xdd, 0x8c, 0x08, 0x2e, 0x53, 0xa5, 0x29,
		0x3a, 0xf2, 0x17, 0x3c, 0xd5, 0x0c, 0x6d, 0x5d, 0x35, 0x4f, 0xeb, 0xf7,
		0x8b, 0x26, 0x02, 0x1c, 0x25, 0xc0, 0x27, 0x12, 0xe7, 0x8c, 0xd4, 0x69,
		0x4c, 0x9f, 0x46, 0x97, 0x77, 0xe4, 0x51, 0xe7, 0xf8, 0xe9, 0xe0, 0x4c,
		0xd3, 0x73, 0x9c, 0x6b, 0xbf, 0xed, 0xae, 0x48, 0x7f, 0xb5, 0x56, 0x44,
		0xe9, 0xca, 0x74, 0xff, 0x77, 0xa5, 0x3c, 0xb7, 0x29, 0x80, 0x2f, 0x6e,
		0xd4, 0xa5, 0xff, 0xa8, 0xba, 0x15, 0x98, 0x90, 0xfc};
	uint8_t dest[128];

	RSA rsa{e, d, K};
	PSS<SHA1> pss{rsa};
	SHA1 sha;
	cout << hexprint("sha1 hash", sha.hash(m, m + 114)) << endl;
	auto a = pss.sign(m, 114);
	cout << a << endl;
	cout <<	pss.verify(a) << endl;
	char msg[] = "Example of ECDSA with P-256";
	SHA2 sha2;
	auto z = sha2.hash(msg, msg + sizeof(msg) - 1);//null character!!
	cout << hexprint("msg", z) << endl;
	
	EC_Field secp256r1_{
		mpz_class{"0xFFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC"},
		mpz_class{"0x5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B"}, 
		mpz_class{"0xFFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF"}
	};
	EC_Point G{
		mpz_class{"0x6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296"},
		mpz_class{"0x4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5"}, 
		secp256r1_
	};
	mpz_class H{"0xA41A41A12A799548211C410C65D8133AFDE34D28BDD542E4B680CF2899C8A8C4"};
	mpz_class n{"0xFFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551"};
	mpz_class d1{"0xC477F9F65C22CCE20657FAA5B2D1D8122336F851A508A1ED04E479C34985BF96"};
	mpz_class d2{"0xC9AFA9D845BA75166B5C215767B1D6934E50C3DB36E89B127B8A622B120F6721"};
 	mpz_class k{"0x7A1A7E52797FC8CAAA435D2A4DACE39158504BF204FBE19F14DBB427FAEE50AE"};
 	mpz_class k2{"0x882905F1227FD620FBF2ABF21244F0BA83D0DC3A9103DBBEE43A1FB858109DB4"};

	ECDSA ecdsa{G, n};
	cout << hex << ecdsa.mod_inv(k) << endl;
	auto P = ecdsa.sign(bnd2mpz(z.begin(), z.end()), d1);
	cout << hex << P.first << endl << P.second << endl;
	cout << ecdsa.verify(bnd2mpz(z.begin(), z.end()), P, G * d1) << endl;

	cout << "U" << endl << d1 * G << endl;	


	for(int i=0; i<300; i++) {
		auto m0 = random_prime(32);
		auto prv_k = random_prime(32);
		auto P0 = ecdsa.sign(m0, prv_k);
		if(char c; !ecdsa.verify(m0, P0, prv_k * G)) {
			cout << "false" << endl;
			cin >> c;
		}
	}
}

