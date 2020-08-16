#include<pybind11/pybind11.h>

class Selenium 
{
public:
	Selenium(std::string phantom_path);
	~Selenium();
	auto import(const char *package);
	std::string get_url(std::string url);
protected:
	pybind11::object webdriver, drv, bs, json;

};
