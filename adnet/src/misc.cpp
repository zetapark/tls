#include<util/option.h>
#include<tcpip/server.h>
#include"misc.h"
using namespace std;

int main(int ac, char** av)
{
	CMDoption co{
		{"dir", "template directory", "www"},
		{"port", "listening port", 2001}
	};
	if(!co.args(ac, av)) return 0;

	Misc site;
	site.init(co.get<const char*>("dir"));
	site.add_header("leave_message", "Access-Control-Allow-Origin: *");
	Server sv{co.get<int>("port")};
	return sv.keep_start(site);
}

