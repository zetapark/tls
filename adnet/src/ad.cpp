#include<iostream>
#include<fstream>
#include<util/option.h>
#include<tcpip/server.h>
#include<tcpip/website.h>
#include<csignal>
#include"ad.h"
using namespace std;

Ad site;

void handler(int)
{//save database before exit
	site.all_the_database_job();
}

int main(int ac, char** av)
{
	CMDoption co{
		{"dir", "template directory", "www"},
		{"port", "listening port", 2001}
	};
	if(!co.args(ac, av)) return 0;

	signal(SIGINT, handler);
	signal(SIGTERM, handler);
	site.init(co.get<const char*>("dir"));
	site.add_header("request_ad.php", "Access-Control-Allow-Origin: *");
	site.add_header("request_ad.php", "Access-Control-Allow-Headers: x-requested-with");
	site.add_header("adnet.js", "Access-Control-Allow-Origin: *");
	site.add_header("adnet.js", "Access-Control-Allow-Headers: x-requested-with");
	Server sv{co.get<int>("port")};
	return sv.keep_start(site);
}

