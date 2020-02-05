#include"sha1.h"
#include"mpz.h"
#include"pss.h"
#include"util/log.h"
using namespace std;

template class PSS<SHA2>;
template class PSS<SHA1>;

template<class HASH, int SLen>
vector<uint8_t> PSS<HASH, SLen>::mgf1(const uint8_t *seed, int sz, int len)
{//mask generation function mgf1(seed, seed size, mask length)
	vector<uint8_t> T;
	uint8_t seedNcounter[sz + 4];
	memcpy(seedNcounter, seed, sz);
	for(int i=0; T.size() < len; i++) {
		mpz2bnd(i, seedNcounter + sz, seedNcounter + sz + 4);
		auto a = sha_.hash(seedNcounter, seedNcounter + sz + 4);
		T.insert(T.end(), a.begin(), a.end());
	}
	T.resize(len);
	return T;
}

template<class HASH, int SLen>
vector<uint8_t> PSS<HASH, SLen>::emsa_pss_encode(const uint8_t *p, int sz, int len)
{//salt size should be same as output size
	auto a = sha_.hash(p, p + sz);
	uint8_t salt[sLen_];
	vector<uint8_t> M_dash{0, 0, 0, 0, 0, 0, 0, 0};
	mpz2bnd(random_prime(sLen_), salt, salt + sLen_);
	M_dash.insert(M_dash.end(), a.begin(), a.end());
	M_dash.insert(M_dash.end(), salt, salt + sLen_);
	a = sha_.hash(M_dash.begin(), M_dash.end());//H

	int db_size = len - hLen_ - 1;
	uint8_t DB[db_size] = {0, };
	memcpy(DB + db_size - sLen_, salt, sLen_);//8
	DB[db_size - sLen_ - 1] = 1;
	auto dbMask = mgf1(a.data(), a.size(), db_size);
	for(int i=0; i<db_size; i++) dbMask[i] ^= DB[i];//maskedDB
	dbMask.insert(dbMask.end(), a.begin(), a.end());
	dbMask.push_back(0xbc);
	return dbMask;
}

template<class HASH, int SLen>
mpz_class PSS<HASH, SLen>::sign(uint8_t *p, int sz) 
{
	auto v = emsa_pss_encode(p, sz, emLen_);
	return rsa_.sign(bnd2mpz(v.begin(), v.end()));
}

template<class HASH, int SLen>
bool PSS<HASH, SLen>::emsa_pss_verify(uint8_t *m, int msz, uint8_t *em, int emsz)
{
	auto mHash = sha_.hash(m, m+msz);
	if(em[emsz - 1] != 0xbc) return false;
	auto dbMask = mgf1(em + emsz - hLen_ - 1, hLen_, emLen_ - hLen_ - 1);
	for(int i=0; i<dbMask.size(); i++) dbMask[i] ^= em[i];//DB
	vector<uint8_t> M_dash = {0,0,0,0,0,0,0,0};
	M_dash.insert(M_dash.end(), mHash.begin(), mHash.end());
	M_dash.insert(M_dash.end(), dbMask.end() - sLen_, dbMask.end());//salt
	mHash = sha_.hash(M_dash.begin(), M_dash.end());
	if(equal(mHash.begin(), mHash.end(), em + emLen_ - hLen_ - 1)) return true;
	return false;
}

template<class HASH, int SLen>
bool PSS<HASH, SLen>::verify(mpz_class sign)
{
	mpz_class m = rsa_.encode(sign);
	uint8_t em[emLen_], p[k_];
	mpz2bnd(m, em, em + emLen_);
	mpz2bnd(sign, p, p + k_);
	return emsa_pss_verify(p, k_, em, emLen_);
}

template<class HASH, int SLen>
PSS<HASH, SLen>::PSS(RSA &r) : rsa_{r}
{
	modbit_ = mpz_sizeinbase(rsa_.K.get_mpz_t(), 2);
	k_ = mpz_sizeinbase(rsa_.K.get_mpz_t(), 256);
	emLen_ = (modbit_ - 1) % 8 == 0 ? k_ - 1 : k_;
}
