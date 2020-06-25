#include<tcpip/website.h>
#include<database/mysqldata.h>

string base64_encode(vector<uint8_t>);
class Adnet : public WebSite
{
protected:
	SqlQuery sq;
	void process();
	void db_ip(std::string ip);

private:
	std::string id_, db_ip_;
	void index(), id_hit(), banner();
	std::string signup();
};

