#include<iomanip>
#include<iostream>
#include<nettle/curve25519.h>
#include"diffie.h"
#include"mpz.h"
using namespace std;

uint8_t r[32], k[32], r2[32], nine[32];

int main()
{
	X25519 x;
	nine[0] = 9;
	for(int i=1; i<10; i++) {
		k[31] = i;
		curve25519_mul_g(r, k);
//		curve25519_mul(r2, k, nine);
		cout << i << endl;
		cout << hexprint("nettle", r) << endl;
//		cout << hexprint("r2----", r) << endl;
		cout << "mul :" << hex << i * x << endl; 
	}	
	mpz_class z;
	z = -3;
	cout << z % 5 << endl;
}

