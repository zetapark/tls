#include<tcpip/website.h>
#include<tcpip/server.h>
#include<util/option.h>
using namespace std;

struct MySite : WebSite
{
protected:
	void process(std::string) {
		if(requested_document_ == "index.html");
	}
};

int main(int ac, char** av)
{
	CMDoption co{
		{"dir", "template directory", "Bikin"},
		{"port", "listening port", 2001}
	};
	if(!co.args(ac, av)) return 0;

	MySite site;
	site.init(co.get<const char*>("dir"));
	Server sv{co.get<int>("port")};
	return sv.start(site);
}

