#include<iostream>
#include"tcpip/server.h"//Server class
#include"tcpip/website.h"//WebSite class
using namespace std;

int main(int ac, char** av)
{
	WebSite my_site;
	my_site.init("site_html");//directory name relative to your exe file
							//directory contains html files
	Server sv{2000};//port number
	cout << "opening port " << 2000 << endl;
	sv.start(my_site);//go infinite loop
}

