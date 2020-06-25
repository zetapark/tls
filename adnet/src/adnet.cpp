#include<util/option.h>
#include<tcpip/server.h>
#include"adnet.h"
using namespace std;

int main(int ac, char** av)
{
	CMDoption co{
		{"dir", "template directory", "www"},
		{"port", "listening port", 2001},
		{"database", "database ip", "localhost"}
	};
	if(!co.args(ac, av)) return 0;

	Adnet site;
	site.init(co.get<const char*>("dir"));
	site.db_ip(co.get<const char*>("database"));
	Server sv{co.get<int>("port")};
	return sv.start(site);
}

