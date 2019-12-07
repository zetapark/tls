#include<iostream>
#include"server.h"//Server class
#include"website.h"//WebSite class
using namespace std;

int main(int ac, char** av)
{
	WebSite my_site{"site_html"};//directory name relative to your exe file
							//directory contains html files
	Server sv{2001};//port number
	cout << "opening port " << 2001 << endl;
	sv.start(my_site);//go infinite loop
}

