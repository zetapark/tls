#include<string>
#include<istream>
#include<vector>
#include<json/json.h>
#include<gmpxx.h>

Json::Value der2json(std::istream& is);
std::string base64_encode(std::vector<unsigned char> v);
std::vector<unsigned char> base64_decode(std::string s);

std::string get_certificate_core(std::istream& is);
mpz_class str2mpz(std::string s);
Json::Value pem2json(std::istream& is);
std::stringstream remove_colon(std::string s);
