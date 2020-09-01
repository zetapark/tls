#pragma once
#include<pybind11/pybind11.h>
#include"tls/hkdf.h"
#include<tcpip/server.h>
#include"tls/prf.h"
#include"tls/hmac.h"
#include"tls/sha256.h"
#include"tls/sha1.h"
#include"tls/naes.h"
#include"tls/cert_util.h"
#include"tcpip/website.h"
#include<database/mysqldata.h>

std::string pemtojson(std::string filename);
std::string dertojson(std::vector<uint8_t> v);

struct PyAES : NettleAes128
{
	void key(pybind11::int_);
	void iv(pybind11::int_);
	void aad(std::vector<unsigned char> v);
	std::pair<std::vector<unsigned char>, std::array<unsigned char, 16>>
		decrypt(std::vector<unsigned char> m);
	std::pair<std::vector<unsigned char>, std::array<unsigned char, 16>>
		encrypt(std::vector<unsigned char> m);
	void xor_with_iv(std::vector<uint8_t> v);
};

struct PySHA1 : public SHA1
{
	auto hash(std::vector<unsigned char> v) {
		return SHA1::hash(v.cbegin(), v.cend());
	}
};

struct PySHA256 : public SHA2
{
	auto hash(std::vector<unsigned char> v) {
		return SHA2::hash(v.cbegin(), v.cend());
	}
};

struct PyHMAC : public HMAC<SHA2>
{
	void key(std::vector<unsigned char> v) {
		HMAC<SHA2>::key(v.cbegin(), v.cend());
	}
	auto hash(std::vector<unsigned char> v) {
		return HMAC<SHA2>::hash(v.cbegin(), v.cend());
	}
};

struct PyPRF : public PRF<SHA2>
{
	void secret(std::vector<unsigned char> v);
	void seed(std::vector<unsigned char> v);
};

struct PyHKDF : HKDF<SHA2>
{
	std::vector<uint8_t> extract(std::vector<uint8_t> v);
	void salt(std::vector<uint8_t> v);
	std::array<uint8_t, 32> hash(std::vector<uint8_t> v);
	std::vector<uint8_t> derive_secret(std::string label, std::vector<uint8_t> msg);
};

struct PyX25519
{
	pybind11::int_ mul_g(pybind11::int_ n);
	pybind11::int_ mul(pybind11::int_ n, pybind11::int_ p);
};

//
//struct PyDiffie : public DHE
//{
//	PyDiffie(int bit = 2048);
//	PyDiffie(pybind11::int_ p, pybind11::int_ g, pybind11::int_ ya);
//	pybind11::int_ set_yb(pybind11::int_ pub_key);
//	pybind11::int_ get_p(), get_g(), get_K(), get_ya(), get_yb();
//};
//
//struct PyRSA : public RSA
//{
//	PyRSA(int bit = 1024);
//	PyRSA(pybind11::int_ e, pybind11::int_ d, pybind11::int_ K);
//	pybind11::int_ encode(pybind11::int_ m), decode(pybind11::int_ m);
//};
//
struct PyClient : Client
{
	PyClient(std::string ip, int port);
	void send(std::vector<unsigned char> v);
	std::vector<unsigned char> recv();
};

struct PyTLSClient : PyClient 
{
	PyTLSClient(std::string ip, int port);
	int get_full_length(const std::string &s);
};

//class PyHTTPSCLient : Client
//{
//public:
//	PyHTTPSCLient(std::string ip, int port);
//	std::string pyrecv();
//	void pysend(std::string s);
//protected:
//	TLS<false> t;
//	int get_full_length(const std::string &s);
//};
//
//struct PyTLS : TLS<false>
//{
//	template<std::string (TLS<false>::*FP)(std::string&&)>
//	std::vector<unsigned char> to_vector_func(std::vector<unsigned char> s) {
//		std::vector<unsigned char> v; std::string t;
//		for(unsigned char c : s) t += c;
//		for(unsigned char c : (this->*FP)(move(t))) v.push_back(c);
//		return v;
//	}
//	std::vector<unsigned char> encode(std::vector<unsigned char> s);
//	std::string alert(std::vector<unsigned char> s);
//};
//
//struct PySQL : SqlQuery
//{
//	std::string select(std::string table, std::string where);
//	bool insert(std::vector<std::string> v);
//	bool connect(std::string ip, std::string user, std::string pass, std::string db);
//	std::vector<std::tuple<std::string, int, std::string>> column();
//};


struct Publicist : WebSite
{
	using WebSite::content_;//how to make pybind not to inspect utf-8 validity
	using WebSite::requested_document_;
	using WebSite::swap;
	using WebSite::append;
	using WebSite::process;
	using WebSite::nameNvalue_;
};

struct Trampoline : WebSite
{
	void process() override { PYBIND11_OVERLOAD(void, WebSite, process, ); }
};

