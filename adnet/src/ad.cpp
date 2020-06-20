#include<iostream>
#include<fstream>
#include<util/option.h>
#include<tcpip/server.h>
#include<tcpip/website.h>
#include"ad.h"
using namespace std;

int main(int ac, char** av)
{
	CMDoption co{
		{"dir", "template directory", "www"},
		{"port", "listening port", 2001}
	};
	if(!co.args(ac, av)) return 0;

	Ad site;
	site.init(co.get<const char*>("dir"));
	site.add_header("request_ad.php", "Access-Control-Allow-Origin: *");
	Server sv{co.get<int>("port")};
	return sv.keep_start(site);
}

