#include<iostream>
#include<fstream>
#include<util/option.h>
#include<tcpip/server.h>
#include<tcpip/website.h>
using namespace std;

class Misc : public WebSite
{
protected:
	void process() {
		if(requested_document_ == "leave_message") {
			string s;
			for(const char &c : nameNvalue_["content"]) {
				s += c;
				if(c == '\'') s += "\\''";
			}
			string cmd = "mailx zeta@zeta2374.com -r " + nameNvalue_["email"] + " -s '"
					+ nameNvalue_["title"] + "' <<< '" + s + "'";
			system(cmd.data());
			content_ = "mail sent";
		} 
	}
};

int main(int ac, char** av)
{
	CMDoption co{
		{"dir", "template directory", "www"},
		{"port", "listening port", 2001}
	};
	if(!co.args(ac, av)) return 0;

	Misc site;
	site.init(co.get<const char*>("dir"));
	site.add_header("leave_message", "Access-Control-Allow-Origin: https://www.zeta2374.com/email.html");
	Server sv{co.get<int>("port")};
	return sv.keep_start(site);
}
