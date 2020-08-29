#include<util/option.h>
#include<tcpip/server.h>
#include"biz.h"
using namespace std;


int main(int ac, char** av)
{
	CMDoption co{
		{"dir", "template directory", "www"},
		{"port", "listening port", 2001},
		{"database", "database ip", "localhost"}
	};
	if(!co.args(ac, av)) return 0;

	Biz f;
	f.db_ip(co.get<const char*>("database"));
	f.init(co.get<const char*>("dir"));
	Server sv{co.get<int>("port")};
	return sv.start(f);
}
