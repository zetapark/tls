#include<pybind11/embed.h>
#include<pybind11/functional.h>
#include<pybind11/stl.h>
#include"selenium.h"
using namespace std;
namespace py = pybind11;

Selenium::Selenium(string phantom_path) 
{
	py::initialize_interpreter();
	webdriver = py::module::import("selenium.webdriver");
	drv = webdriver.attr("PhantomJS")(py::arg("executable_path") = phantom_path);
	json = py::module::import("json").attr("loads");
	bs = py::module::import("bs4").attr("BeautifulSoup");
}

Selenium::~Selenium()
{
	drv.attr("quit")();
	py::finalize_interpreter();
}

auto Selenium::import(const char *p)
{
	return py::module::import(p);
}

string Selenium::get_url(string url)
{
	drv.attr("get")(url);
	return drv.attr("page_source").cast<string>();
}
