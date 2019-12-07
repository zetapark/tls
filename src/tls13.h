#include<functional>
#include"sha256.h"
#include"hkdf.h"
#include"tls.h"

template<bool SV> class TLS13 : public TLS<SV>
{
public:
	std::string client_hello(std::string &&s = "");
	std::string server_hello(std::string &&s = "");
	bool handshake(std::function<std::string()> read_f, std::function<void(std::string)> write_f);
protected:
	HKDF<SHA2> hkdf_;
	mpz_class premaster_secret_;//inspect this to check tls version
	std::string client_ext();	
	std::string server_ext();
	bool client_ext(unsigned char *p);
	bool server_ext(unsigned char *p);
private:
	void protect_data(), protect_handshake();
	void set_aes(std::vector<uint8_t> salt, std::string client_label, std::string server_label);
	bool supported_group(unsigned char *p, int len);
	bool point_format(unsigned char *p, int len);
	bool sub_key_share(unsigned char *p);
	bool key_share(unsigned char *p, int len);
	bool supported_version(unsigned char *p, int len);
	void derive_keys(mpz_class premaster_secret);
};

