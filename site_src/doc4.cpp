#include<iostream>
#include"tcpip/server.h"//Server class
#include"tcpip/website.h"//WebSite class
using namespace std;

class My_site : public WebSite
{
public:
	My_site() : WebSite{"site_html"} {}
protected:
	void process() {
		if(requested_document_ == "post-test") 
			for(auto [a, b] : nameNvalue_) content_ += a + ':' + b;
	}
};

int main(int ac, char** av)
{
	My_site my_site;//directory name relative to your exe file
							//directory contains html files
	Server sv{2000};//port number
	cout << "opening port " << 2000 << endl;
	sv.start(my_site);//go infinite loop
}



