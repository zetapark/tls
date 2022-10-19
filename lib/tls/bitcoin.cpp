#include<iostream>
#include<iomanip>
#include"diffie.h"
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
	
//h = 1

}
