#pragma once
#include<array>
#include<gmpxx.h>
#include<type_traits>
#include<cassert>
#include<vector>
#include<nettle/aes.h>
#include<nettle/cbc.h>
#include<nettle/gcm.h>
#include"aes.h"
#define Encryption true
#define Decryption false

//정의부
template<bool Enc, int B = 128> class nAES
{
public:
	void key(const mpz_class key);
	void key(const unsigned char* key);
	void iv(const mpz_class iv);
	void iv(const unsigned char* iv);
	template<typename It>
	std::vector<unsigned char> encrypt(const It begin, const It end) {
		const int sz = end - begin;
		assert(sz % 16 == 0);
		std::vector<unsigned char> result(sz);
		cbc_encrypt(&aes_, (B == 128 ?
					(nettle_cipher_func*)aes128_encrypt :
					(nettle_cipher_func*)aes256_encrypt),
					16, iv_, sz, (uint8_t*)&result[0], 
					(const unsigned char*)&*begin);
		return result;
	}
	template<typename It>
	std::vector<unsigned char> decrypt(const It begin, const It end) {
		const int sz = end - begin;
		assert(sz % 16 == 0);
		std::vector<unsigned char> result(sz);
		cbc_decrypt(&aes_, (B == 128 ?
					(nettle_cipher_func*)aes128_decrypt :
					(nettle_cipher_func*)aes256_decrypt),
					16, iv_, sz, (uint8_t*)&result[0],
					(const unsigned char*)&*begin);
		return result;
	}
protected:
	typename std::conditional<B == 128, aes128_ctx, aes256_ctx>::type aes_;
	unsigned char iv_[16], key_[32];
private:
	void key();
};

class NettleAes128
{
public:
	void key(const unsigned char *p);
	void iv(const unsigned char *p);
	void iv(const unsigned char *p, int from, int sz);
	std::array<unsigned char, 16> encrypt(unsigned char *p, int sz);
	std::array<unsigned char, 16> decrypt(unsigned char *p, int sz);
	void xor_with_iv(const unsigned char *p);
	void aad(const unsigned char *p, int sz);
protected:
	gcm_aes128_ctx ctx_;
	std::vector<uint8_t> aad_;
	unsigned char iv_[12];
};
