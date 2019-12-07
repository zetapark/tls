#include<bitset>
#include<cassert>
#include<iomanip>
#include<iostream>
#include<nettle/curve25519.h>
#include"mpz.h"
#include"diffie.h"
using namespace std;

int main()
{
	X25519 x2;
	cout << x2 << endl;
	cout << x2 + x2 << endl;
	cout << x2 + x2 + x2 << endl;
	cout << x2 * 2 << endl;
	auto q1 = x2 * 10;
	auto q2 = x2 * 14;
	auto K1 = q2 * 10;
	auto K2 = q1 * 14;
	cout << hex << q1.x << endl << q2.x << endl << K1.x << endl << K2.x << endl;


	uint8_t kp[32], one[32], nine[32];
	for(auto &i : one) i = 0;
	for(auto &i : nine) i = 0;
	one[0] = 1; nine[0] = 9;
	curve25519_mul_g(kp, one);
	cout << hexprint("one", kp) << endl;
	curve25519_mul(kp, one, nine);
	cout << hexprint("nine", kp) << endl;

 	mpz_class a{"0x77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a"},
			  b{"0x5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb"},
			  pa{"0x8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a"},
			  pb{"0xde9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f"},
			  k{"0x4a5d9d5ba4ce2de1728e3bf480350f25e07e21c947d19e3376f09b3c1e161742"};

	mpz2bnd(a, one, one+32);
//	reverse(one, one+32);
	curve25519_mul_g(kp, one);
//	reverse(kp, kp+32);
	cout << hexprint("pa", kp);


	EC_Field f{-5, 8, 37};//y^2 = x^3 -5x + 8 mod 37
	EC_Point p1{6, 3, f}, p2{9, 10, f}, p3{1, 2, f};
	cout << dec << p1 + p2 << 5 * p1 << endl << 3 * p1 + 4 * p2 << endl;
	for(int i=0; i<100; i++) cout << dec << i * p3;

	EC_Field f2{2, 2, 17};
	EC_Point p11{5, 1, f2};
	cout << endl;
	for(int i=0; i<100; i++) cout << dec << i * p11;

	auto xA = p11 * 3;
	auto xB = p11 * 7;
	assert(xA * 7 == xB * 3);
	auto K = xA * 7;
	cout << endl << xA << xB << K << endl;

	bitset<128> bi{0b11100001};
	bi <<= 120;
	cout << bi << endl;

	cout << setbase(2) << 10;
	cout << bi[0] << endl;

	EC_Field secp{0, 7, mpz_class{"0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"}};
	EC_Point G{ mpz_class{"0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"},
		mpz_class{"0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"},
		secp};
	for(int i=1; i<10; i++) cout << hex << i * G << endl;
	cout << mpz_class{"112233445566778899112233445566778899"} * G << endl;
}

