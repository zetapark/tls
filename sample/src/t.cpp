#include<fstream>
#include<sstream>
#include<cert_util.h>
#include<iostream>
#include<rsa.h>
using namespace std;

int main()
{
	mpz_class sign = 0xaeeed018cc6868ca5c22ef2d58a26b5c54f154df3c0ca9bc135baf30d83b83de1cb67b7b9e0e802f88c0907ea82c9d90963d0dc5d7aa3277149444b820ea9553cc6810bce6f3961d33fdac4a63e4442bf47410af21dd74c92ed0f6c8e292f8afa72da051287a18ec1fc373eaff082190fddd39eeb84f68a4feae78d5b9fd0d3a15ea80b8a538c468aa5915a2af8148d36d810adfe523b780c6ddac9fbf00d5bf942a7cb11cb149d6d7c40f56d0ba2fc3ff1574c772de50ac0b5d8f4c18d2a9655d4f86f699f5d8e31ec6c0adae2934fb544d70b51153e4cdc2e6e3688bfd3276893189ae8583f6d8840c5be267c365ecc115994c134b0d777389cbbe4679498b_mpz;
	ifstream f{"../privkey.pem"};

	auto ss = remove_colon(pem2json(f)[0][2].asString());
	auto jv = der2json(ss);
	auto K = str2mpz(jv[1].asString());
	auto e = str2mpz(jv[2].asString());
	auto d = str2mpz(jv[3].asString());
	RSA rsa{e, d, K};
	cout << hex << K << endl << e << endl << d << endl;

	cout << rsa.decode(rsa.encode(sign) ) << endl;
	cout << hex << rsa.decode(sign);


}
