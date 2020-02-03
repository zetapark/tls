#include<fstream>
#include<string>
#include<iostream>
#include<sstream>
#include"sha256.h"
#include"mpz.h"
#include"cert_util.h"
using namespace std;

static array<mpz_class, 3> get_pubkeys(Json::Value jv) {
	auto sign = str2mpz(jv[0][2].asString());
	auto ss = remove_colon(jv[0][0][6][1].asString());
	jv = der2json(ss);
	return {str2mpz(jv[0][0].asString()), str2mpz(jv[0][1].asString()), sign};
}

int main() {
	ifstream f("server-cert.pem");
	string s = get_certificate_core(f);
	auto v = base64_decode(s);
	SHA2 sha;
	int length = v[6] * 256 + v[7] + 4;
	cout << hexprint("hash", sha.hash(v.begin() + 4, v.begin() + length + 4)) << endl;
	stringstream ss;
	for(uint8_t c : v) ss << c;
	auto jv = der2json(ss);
	auto [K, e, sign] = get_pubkeys(jv);

	s = get_certificate_core(f);
	v = base64_decode(s);
	stringstream ss2;
	for(uint8_t c : v) ss2 << c;
	jv = der2json(ss2);
	auto [K2, e2, sign2] = get_pubkeys(jv);
	auto k = powm(sign, e2, K2);
	cout << hex << k << endl;
}
// 출력 : 1ffffffffffffffffffffffffffffffffffffffffffffffff
//ffffffffffffffffffffffffffffffffffffffffffffffffffffffff
//ffffffffffffffffffffffffffffffffffffffffffffffffffffffff
//ffffffffffffffffffffffffffffffffffffffffffffffffffffffff
//ffffffffffffffffffffffffffffffffffffffffffffffffffffffff
//ffffffffffffffffffffffffffffffffffffffffffffffffffffffff
//ffffffffffffffffffffffffffffffffffffffffffffffffffffffff
//ffffffffffffffffffff003031300d0609608648016503040201050004
//20374dbaf09c08e4df4c4eeb31ac1799676f39f4bc07993eeb10806bec
//72efca76

