#include<iostream>
#include<array>
#include"aes.h"
#include"naes.h"
#include"mpz.h"
using namespace std;

uint8_t key[32] = {0,}, iv[16] = {0,}, aad[32] = {0,}, src[64], src3[64], src2[64];
int main()
{
	for(int i=0; i<64; i++) src3[i] = i;
	NettleAes128 aes, aes2;
	GCM<AES> gcm;
	gcm.key(key);
	gcm.iv(iv);
	gcm.iv(src3, 4, 8);
	gcm.xor_with_iv(src3);
	gcm.aad(aad, 32);
	auto a = gcm.encrypt(src, 64);
	gcm.xor_with_iv(src3);
	cout << hexprint("src", src) << endl;
	aes.key(key);
	aes.iv(iv);
	aes.iv(src3, 4, 8);
	aes.xor_with_iv(src3);
	aes.aad(aad, 32);//aad position matter
	auto b = aes.encrypt(src2, 64);
	aes.xor_with_iv(src3);
	cout << hexprint("src2", src2) << endl;
	//assert(a == b);
	cout << hexprint("a", a) << endl;
	cout << hexprint("b", b) << endl;
	//cout << hexprint("dgst", dgst) << endl;
	
//	gcm.iv(
}
