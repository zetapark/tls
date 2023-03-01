#include<fstream>
#include<iostream>
#include<iomanip>
#include"diffie.h"
#include"ecdsa.h"
#include"sha1.h"
#include"cert_util.h"
using namespace std;

int main()
{
	EC_Field secp256k{0, 7, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F_mpz};
	EC_Point G{0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798_mpz,
		0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8_mpz,
		secp256k};
	mpz_class n = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141_mpz;

	auto P = 3 * G;
	EC_Point Q = (n - 3) * G;
	cout << hex << 3 * G << endl;
	cout << P + Q << endl;
	cout << "---------------------" << endl;

	EC_Field secp256r1{0xFFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC_mpz,
    0x5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B_mpz,
    0xFFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF_mpz};
	EC_Point G2{0x6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296_mpz,
    0x4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5_mpz, secp256r1};
	mpz_class n2 = 0xFFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551_mpz;
	mpz_class d = 0xfd8961b427cec83cfd443cbd98ca8084273d5d2d62e45b59fb106819b6abad_mpz;
	auto Q2 = G2 * d;
	ECDSA ecdsa{G2, n2};
	auto signature = ecdsa.sign(0xff, d);
	cout << d << endl;
	cout << signature.first << endl << signature.second << endl;
	cout << ecdsa.verify(0xff, signature, Q2) << endl;
	
	cout << "---------------------" << endl;
	SHA1 sha;
	const char *p = "abc";
	for(auto c : sha.hash(p, p+3)) cout << hex << +c;


	cout << "---------------------" << endl;
	ifstream f{"cert.pem"};
	cout << pem2json(f);
}

