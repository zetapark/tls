#include<vector>
#include"sha256.h"
#include"rsa.h"

template<class HASH = SHA2> std::vector<uint8_t> mgf1(std::vector<uint8_t> msg, int len);
template<class HASH = SHA2> std::vector<uint8_t>
pss_encode(std::vector<uint8_t> msg, int rsa_modulus_length_in_octet);
template<class HASH = SHA2> bool pss_verify(std::vector<uint8_t> msg, std::vector<uint8_t> sign);

//template<class HASH = SHA2, int SLen = HASH::output_size> class PSS
//{
//public:
//	PSS(RSA &r);
//	mpz_class sign(uint8_t *p, int sz);
//	bool verify(mpz_class sign);
//
//protected:
//	HASH sha_;
//	RSA &rsa_;
//	int modbit_, k_, emLen_; 
//
//private:
//	const int hLen_ = HASH::output_size, sLen_ = SLen;
//	std::vector<uint8_t> emsa_pss_encode(const uint8_t *p, int sz, int len);
//	std::vector<uint8_t> mgf1(const uint8_t *p, int sz, int len);
//	bool emsa_pss_verify(uint8_t *m, int msz, uint8_t *em, int emsz);
//};
