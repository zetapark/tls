#include<util/option.h>
#include<util/log.h>
#include"tls13.h"
#include"https.h"
using namespace std;

int main(int ac, char** av) 
{//return 1 : error, 0 : ok
	CMDoption co{
		{"port", "listening port", 4433},
		{"log filter", "log filter setting, use lower case to disable log", "tdIWEF"},
		{"conf", "config file", "../eZ.conf"},
		{"cert", "rsa certificate pem file", "../fullchain.pem"},
		{"key", "rsa private key pem file", "../privkey.pem"}
	};
	if(!co.args(ac, av)) return 0;

	Log::get_instance()->set_log_filter(co.get<const char*>("log filter"));
	TLS<true>::init_certificate(co.get<const char*>("cert"), co.get<const char*>("key"));
	TLS13<true>::init_certificate(co.get<const char*>("cert"));
	Middle sv{co.get<int>("port")};
	sv.read_config(co.get<const char*>("conf"));
	return sv.start();
}


