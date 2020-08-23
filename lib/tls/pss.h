#include<vector>
#include"sha256.h"

template<class HASH = SHA2> std::vector<uint8_t> mgf1(std::vector<uint8_t> msg, int len);
template<class HASH = SHA2> std::vector<uint8_t>
pss_encode(std::vector<uint8_t> msg, int rsa_modulus_length_in_octet);
template<class HASH = SHA2> bool pss_verify(std::vector<uint8_t> msg, std::vector<uint8_t> sign);
