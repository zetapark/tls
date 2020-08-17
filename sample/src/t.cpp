#include<chrono>
#include<fstream>
#include<sstream>
#include<cert_util.h>
#include<mpz.h>
#include<iostream>
#include<rsa.h>
using namespace std;
using namespace std::chrono;

int main()
{
	mpz_class sign = 0x8579f6d9ff1d9caf6bb399aaefbcdbc8c0ff3badbb5e3779ebc5f1bd8510d7ddaef345c60aa3d5976894d2463f785e05cde0e6ac950a5534ae831371405b39fb5a80ee01fd38beadaf6fb0d5569d0494792d007194d57a48bb99f1dbb9842b3f7cfe1dfb64acc0f78d06b1d9bff5a5b19045d08044309a37e7a56e0329a1745d7c9edddd2c3439c3074388c7fd3b84a9a836db77cd12d2dfee616e1e724f24963251bc7e4bd837b4edcdc663a717a8fc568a330b63b844f67f68b03bb40ca8de12d3800581a3b2b32e3dd2ff60e5b73c7edf8d4ef50924fdee6ba399b4a6aa504e02e20f24c67396f9a1c1888bdee87908bf02e822f7a9da26462d43c1d96b2b_mpz;
	ifstream f{"../privkey.pem"};

	auto ss = remove_colon(pem2json(f)[0][2].asString());
	auto jv = der2json(ss);
	auto K = str2mpz(jv[1].asString());
	auto e = str2mpz(jv[2].asString());
	auto d = str2mpz(jv[3].asString());
	cout << hex << e << endl << d << endl << K << endl;
	RSA rsa{e, d, K};

	auto r = rsa.decode(sign);
	unsigned char p[52];
	mpz2bnd(r, p, p + 52);
	cout << hex << r << endl;
	cout << asctime(gmtime((time_t*)p))  << endl;
	cout << p + 8 << endl;
	//cout << asctime(gmtime((&st.time))) << endl << hexprint("pass", st.c) << endl;
}
