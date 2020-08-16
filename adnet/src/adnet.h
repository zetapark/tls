#include<tcpip/website.h>
#include<database/mysqldata.h>

string base64_encode(vector<uint8_t>);
class Adnet : public WebSite
{
public:
	void db_ip(std::string ip);

protected:
	SqlQuery sq;
	void process();

private:
	std::string id_, db_ip_, pwd_, change_id_, email_;
	int key_, verify_code_, lang_ = 0;
	void index(), id_hit(), banner(), pref(), preference(), search(), design(), board();
	std::string signup(), forgot(), email_check(), recommend();
	std::string langjs[2] = {"$(function() { $('.eng').hide(); });",
													 "$(function() { $('.kor').hide(); });"};
};

template<class S> static std::string struct2str(const S &s)
{
	return std::string{(const char*)&s, sizeof(s)};
}
