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
	mpz_class sign = 0xb77ee943a05d91d44728bb98b8e0b2b64431d14c98e044b0dfd6ca9fd6f89b5859e1f862f0a93bbc809aa2313e9a911ac72247d6e7893ede9e9f6e34ec379c1b43a70ecfee53f5df4498c5d56861d44166377894881400606f72f71c2e115611d5d90433e920c432d57046a072f3c19288910fd0569dd6bca176d84db264627b3f16893367ce402f25ac13d7f2461b53def0a470dd39f760cd8f6ed93e76ad20c89522f34c150b87504af50ee921abb1ec25b3675ca1f7be3b0ec473d5de730575f2fff05a479919b4c93a0feec243ff0640b565b7c90e863d78e0f233366597e21a3bccbec7c439fe7fcb0b80d0bdd93f0a5b011e47a9b09f9d3e7ac7a34e6e_mpz;
	ifstream f{"../privkey.pem"};

	auto ss = remove_colon(pem2json(f)[0][2].asString());
	auto jv = der2json(ss);
	auto K = str2mpz(jv[1].asString());
	auto e = str2mpz(jv[2].asString());
	auto d = str2mpz(jv[3].asString());
	RSA rsa{e, d, K};

	auto r = rsa.decode(sign);
	unsigned char p[40];
	mpz2bnd(r, p, p + 40);
	for(int i=1; i<=40; i++) cout << i%10 << i%10;
	cout << hex << r << endl;
	for(int i=0; i<40; i++) cout << hex << +p[i];
	cout << asctime(localtime((time_t*)p))  << endl;
	//cout << asctime(gmtime((&st.time))) << endl << hexprint("pass", st.c) << endl;
}
