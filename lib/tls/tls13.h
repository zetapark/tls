#include<functional>
#include<memory>
#include<mutex>
#include"sha256.h"
#include"hkdf.h"
#include"tls.h"
#include"tcpip/server.h"
#include"psk.h"
#define HASH SHA2

template<bool SV> class TLS13 : public TLS<SV>
{
public:
	TLS13();
	std::string client_hello(std::string &&s = "");
	std::string server_hello(std::string &&s = "");
	std::optional<std::shared_ptr<MClient>> handshake(std::function<std::optional<std::string>()>
			read_f, std::function<void(std::string)> write_f);
	std::string finished(std::string &&s = "");
	std::string certificate_verify();
	std::optional<std::string> decode(std::string &&s);
	std::string encode(std::string &&s, int type = APPLICATION_DATA);
	std::string server_certificate13();
	std::string new_session_ticket();
	std::tuple<std::string, std::shared_ptr<MClient>> new_session(std::string ip, int port, int fd);
	bool is_tls13();
	std::pair<std::vector<uint8_t>, std::vector<uint8_t>> new_session_ticket(std::string s);
	static void init_certificate(const char *certpem);
protected: 
	HKDF<HASH> hkdf_;
	mpz_class premaster_secret_;//inspect this to check tls version
	std::string client_ext();	
	std::string server_ext();
	std::string encrypted_extension();
	bool client_ext(unsigned char *p);
	bool server_ext(unsigned char *p);
	static std::string certificate13_;
private:
	SClient sclient_;
	int selected_psk_ = -2;
	uint8_t prv_[32], echo_id_[32];
	static std::string ecdsa_certificate_;
	void protect_handshake();
	std::array<std::vector<uint8_t>, 2> set_aes(std::vector<uint8_t> salt,
			std::string client_label, std::string server_label);
	std::array<std::vector<uint8_t>, 2> finished_key_;
	std::vector<uint8_t> psk_, resumption_master_secret_;
	std::vector<std::vector<uint8_t>> ticket_id_;
	bool supported_group(unsigned char *p, int len);
	bool point_format(unsigned char *p, int len);
	bool sub_key_share(unsigned char *p);
	bool key_share(unsigned char *p, int len);
	bool supported_version(unsigned char *p, int len);
	int psk(unsigned char *p, int len);
	void derive_keys(mpz_class premaster_secret);
	std::optional<std::string> decode13(std::string &&s);
	std::string encode13(std::string &&s, int type = APPLICATION_DATA);
};
