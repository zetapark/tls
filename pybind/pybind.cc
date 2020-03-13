#include<pybind11/pybind11.h>
#include<pybind11/stl.h>
#include"tcpip/server.h"
#include"src/tls.h"
#include"wrapper.h"
#include"database/mysqldata.h"
using namespace std;
using namespace pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(tls_crypt, m) {
	m.doc() = "TLS python module";
	m.def("base64_encode", &base64_encode);
	m.def("base64_decode", &base64_decode);
	m.def("pem2json", &pemtojson);
	class_<PyAES>(m, "AES")
		.def(init<>())
		.def("key", &PyAES::key)
		.def("iv", &PyAES::iv)
		.def("decrypt", &PyAES::decrypt)
		.def("encrypt", &PyAES::encrypt)
		.def("aad", &PyAES::aad)
		;
	class_<PySHA256>(m, "SHA256")
		.def(init<>())
		.def("hash", &PySHA256::hash);
	class_<PySHA1>(m, "SHA1")
		.def(init<>())
		.def("hash", &PySHA1::hash);
	class_<PyHMAC>(m, "HMAC")
		.def(init<>())
		.def("key", &PyHMAC::key)
		.def("hash", &PyHMAC::hash)
		;
	class_<PyPRF>(m, "PRF")
		.def(init<>())
		.def("label", &PyPRF::label)
		.def("seed", &PyPRF::seed)
		.def("secret", &PyPRF::secret)
		.def("get_n_byte", &PyPRF::get_n_byte)
		;
//	class_<PyDiffie>(m, "DiffieHellman")
//		.def(init<int>(), "bit"_a = 1024)
//		.def(init<int_, int_, int_>())
//		.def("set_yb", &PyDiffie::set_yb)
//		.def_property_readonly("K", &PyDiffie::get_K)
//		.def_property_readonly("p", &PyDiffie::get_p)
//		.def_property_readonly("g", &PyDiffie::get_g)
//		.def_property_readonly("ya", &PyDiffie::get_ya)
//		.def_property_readonly("yb", &PyDiffie::get_yb)
//		;
//	class_<PyRSA>(m, "RSA") //		.def(init<int>(), "key_size"_a = 1024)
//		.def(init<int>(), "bit"_a = 1024)
//		.def(init<int_, int_, int_>())
//		.def("encode", &PyRSA::encode)
//		.def("decode", &PyRSA::decode)
//		.def("sign", &PyRSA::decode)
//		;
//	class_<PyClient>(m, "Client")
//		.def(init<std::string, int>(), "ip"_a = "127.0.0.1", "port"_a = 2001)
//		.def("send", &PyClient::send)
//		.def("recv", &PyClient::recv)
//		;
//	class_<PyTLSClient>(m, "TLS_client")
//		.def(init<std::string, int>(), "ip"_a = "127.0.0.1", "port"_a = 4433)
//		.def("send", &PyTLSClient::send)
//		.def("recv", &PyTLSClient::recv)
//		;
//	class_<PyTLS>(m, "TLS")
//		.def(init<>())
//		.def("client_hello", &PyTLS::to_vector_func<&TLS<false>::client_hello>)
//		.def("server_hello", &PyTLS::to_vector_func<&TLS<false>::server_hello>)
//		.def("server_certificate", &PyTLS::to_vector_func<&TLS<false>::server_certificate>)
//		.def("server_key_exchange", &PyTLS::to_vector_func<&TLS<false>::server_key_exchange>)
//		.def("server_hello_done", &PyTLS::to_vector_func<&TLS<false>::server_hello_done>)
//		.def("client_key_exchange", &PyTLS::to_vector_func<&TLS<false>::client_key_exchange>)
//		.def("change_cipher_spec", &PyTLS::to_vector_func<&TLS<false>::change_cipher_spec>)
//		.def("finished", &PyTLS::to_vector_func<&TLS<false>::finished>)
//		.def("encode", &PyTLS::encode)
//		.def("decode", &PyTLS::to_vector_func<&TLS<false>::decode>)
//		.def("alert", &PyTLS::alert)
//		.def("support_dhe", &PyTLS::support_dhe)
//		;
//	class_<PyHTTPSCLient>(m, "HTTPS_Client")
//		.def(init<std::string, int>(), "ip"_a = "127.0.0.1", "port"_a = 4433)
//		.def("send", &PyHTTPSCLient::pysend)
//		.def("recv", &PyHTTPSCLient::pyrecv)
//		;
//	class_<PySQL>(m, "SqlQuery")
//		.def(init<>())
//		.def("select", &PySQL::select)
//		.def("insert", &PySQL::insert)
//		.def("connect", &PySQL::connect)
//		.def("columns", &PySQL::column)
//		.def("encrypt", &SqlQuery::encrypt)
//		.def("now", &SqlQuery::now)
//		.def("show_tables", &SqlQuery::show_tables)
//		;
}

