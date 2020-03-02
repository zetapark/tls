#pragma once
#include<map>
#include<string>
#include"website.h"
using namespace std;

class BootStrapSite : public WebSite
{//class that makes writing bootstrap elements easy
public:
	BootStrapSite(std::string dir = "www");

protected:
	static std::string carousel(std::vector<std::string> img, std::vector<std::string> desc, std::vector<std::string> href);
};

