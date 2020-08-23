#include<cstring>
#include"mpz.h"
#include"naes.h"
using namespace std;

template class nAES<true, 128>;
template class nAES<true, 256>;
template class nAES<false, 128>;
template class nAES<false, 256>;
//구현부
template<bool Enc, int B> void nAES<Enc, B>::key(const mpz_class keyy)
{
	mpz2bnd(keyy, key_, key_+ B / 8);
	key();
}

template<bool Enc, int B> void nAES<Enc, B>::key(const unsigned char* keyy)
{
	memcpy(key_, keyy, B / 8);
	key();
}

template<bool Enc, int B> void nAES<Enc, B>::key()
{
	if constexpr(Enc) {
		if constexpr(B == 128) aes128_set_encrypt_key(&aes_, key_);
		else aes256_set_encrypt_key(&aes_, key_);
	} else {
		if constexpr(B == 128) aes128_set_decrypt_key(&aes_, key_);
		else aes256_set_decrypt_key(&aes_, key_);
	}
}

template<bool Enc, int B> void nAES<Enc, B>::iv(const mpz_class iv)
{
	mpz2bnd(iv, iv_, iv_+16);
}

template<bool Enc, int B> void nAES<Enc, B>::iv(const unsigned char* iv)
{
	memcpy(iv_, iv, 16);
}


void NettleAes128::iv(const unsigned char *p)
{
	memcpy(iv_, p, 12);
}

void NettleAes128::key(const unsigned char *p)
{
	gcm_aes128_set_key(&ctx_, p);
}

void NettleAes128::iv(const unsigned char *p, int from, int sz)
{
	memcpy(iv_ + from, p, sz);
}

std::array<unsigned char, 16> NettleAes128::encrypt(unsigned char *p, int sz)
{
	array<unsigned char, 16> r;
	gcm_aes128_set_iv(&ctx_, 12, iv_);
	gcm_aes128_update(&ctx_, aad_.size(), &aad_[0]);
	gcm_aes128_encrypt(&ctx_, sz, p, p);
	gcm_aes128_digest(&ctx_, 16, &r[0]);
	return r;
}

std::array<unsigned char, 16> NettleAes128::decrypt(unsigned char *p, int sz)
{
	array<unsigned char, 16> r;
	gcm_aes128_set_iv(&ctx_, 12, iv_);
	gcm_aes128_update(&ctx_, aad_.size(), &aad_[0]);
	gcm_aes128_decrypt(&ctx_, sz, p, p);
	gcm_aes128_digest(&ctx_, 16, &r[0]);
	return r;
}

void NettleAes128::xor_with_iv(const unsigned char *p)
{
	for(int i=0; i<4; i++) iv_[i] ^= 0;
	for(int i=0; i<8; i++) iv_[4 + i] ^= p[i];
}

void NettleAes128::aad(const unsigned char *p, int sz)
{
	aad_ = {p, p + sz};
}
