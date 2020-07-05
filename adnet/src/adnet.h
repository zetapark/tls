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
	std::string id_, db_ip_, pwd_, change_id_;
	int key_ = 10000;
	void index(), id_hit(), banner();
	std::string signup(), forgot();
};

