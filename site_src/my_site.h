#include"database/mysqldata.h"
#include"tcpip/website.h"

class Site : public WebSite
{
public:
	Site();

protected:
	SqlQuery sq;
	void process();
	string id, name;
	int level;

private:
	void index(), signup();
	string upload();
};

