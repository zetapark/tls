#include<tcpip/website.h>
#include<tcpip/server.h>
using namespace std;

struct MyWebsite : WebSite
{
protected:
	void process(std::string) {
		if(requested_document_ == "index.html")
			append("<body>", "Hello this is from Server");
	}
};

int main(int ac, char** av)
{
	MyWebsite f;
	f.init("www");//template directory
	Server sv{2004};//opening port
	sv.start(f);
}
