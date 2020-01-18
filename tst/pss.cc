#include<iostream>
#include"catch.hpp"
#define private public
#define protected public
#include"src/pss.h"
#undef private
#undef protected
#include"src/mpz.h"
#include"src/ecdsa.h"
using namespace std;

TEST_CASE("mgf1") {
	uint8_t seed[] = "bar", a[50];
	RSA rsa{128};
	PSS pss{rsa};
	mpz_class z{"0x382576a7841021cc28fc4c0948753fb8312090cea942ea4c4e735d10dc724b155f9f6069f289d61daca0cb814502ef04eae1"};
	mpz2bnd(z, a, a+50);
	auto b = pss.mgf1(seed, 3, 50);
	REQUIRE(equal(b.begin(), b.end(), a));
}

TEST_CASE("ECDSA") {
	EC_Field secp256r1{
		0xFFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC_mpz,
		0x5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B_mpz, 
		0xFFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF_mpz
	};
	EC_Point G{
		0x6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296_mpz,
		0x4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5_mpz, 
		secp256r1
	};
	mpz_class d = random_prime(31);
	mpz_class n{"0xFFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551"};
	auto Q = d * G;
	ECDSA ecdsa{G, n};
	char message[] = "This is a Test message";
	SHA2 sha;
	auto m = sha.hash(message, message + sizeof(message) - 1);
	auto z = bnd2mpz(m.begin(), m.end());
	auto sign = ecdsa.sign(z, d);
	REQUIRE(ecdsa.verify(z, sign, Q));
}
