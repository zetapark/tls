#include<tcpip/website.h>
#include<database/mysqldata.h>

string base64_encode(vector<uint8_t>);
class Adnet : public WebSite
{
protected:
	SqlQuery sq;
	string id_;
	void process();

private:
	void index(), id_hit(), banner();
	std::string signup();
};

