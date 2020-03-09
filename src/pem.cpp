#include<iomanip>
#include<fstream>
#include<string>
#include<iostream>
#include<sstream>
#include"util/option.h"
#include"cert_util.h"
using namespace std;

int main(int ac, char **av) {
	CMDoption co{
		{"file", "pem file", "fullchain.pem"},
		{"private key", "extract private key", false},
	};
	if(!co.args(ac, av)) return 0;
	ifstream f(co.get<const char*>("file"));
	for(string s; (s = get_certificate_core(f)) != "";) {
		auto v = base64_decode(s);
		stringstream ss;
		for(uint8_t c : v) ss << c;
		auto jv = der2json(ss);
		cout << jv << endl;
		if(co.get<bool>("private key")) {
			auto ss = remove_colon(jv[0][2].asString());
			cout << der2json(ss);
		}
	}
//	if(jv[0].size() == 3) {
//		auto [K,e,sign] = get_pubkeys(jv);
//		cout << hex << "K : " << K << "\ne : " << e << "\nsign : " << sign << endl;
//	} else {
//		auto [K, e, sign] = get_keys(jv);
//		cout << hex << "K : " << K << "\ne : " << e << "\nsign : " << sign << endl;
//	}
}

