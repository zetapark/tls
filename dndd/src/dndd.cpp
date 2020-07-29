#include<util/option.h>
#include<tcpip/server.h>
#include"dndd.h"
using namespace std;


int main(int ac, char** av)
{
	CMDoption co{
		{"dir", "template directory", "www"},
		{"port", "listening port", 2001}
	};
	if(!co.args(ac, av)) return 0;

	DnDD f;
	f.init(co.get<const char*>("dir"));
	Server sv{co.get<int>("port")};
	return sv.start(f);
}
