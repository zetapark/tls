#include<pybind11/functional.h>
#include<pybind11/stl.h>
#include<fstream>
#include<thread>
#include<chrono>
#include<database/util.h>
#include"selenium.h"
using namespace std;
namespace py = pybind11;

Selenium::Selenium(string phantom_path) 
{
	webdriver = py::module::import("selenium.webdriver");
	drv = webdriver.attr("Firefox")();//py::arg("executable_path") = phantom_path_ = phantom_path);
	json = py::module::import("json").attr("loads");
	bs = py::module::import("bs4").attr("BeautifulSoup");
}

Selenium::~Selenium()
{
	drv.attr("quit")();
	save_graph();
}

void Selenium::title_tag(string s) { title_tag_ = s; }
void Selenium::title_class(string s) { title_class_ = s; }
void Selenium::seller_tag(string s) { seller_tag_ = s; }
void Selenium::seller_class(string s) { seller_class_ = s; }
void Selenium::output_file(string s) { output_file_ = s; }
void Selenium::graph_file(string s) { graph_file_ = s; }
void Selenium::start_url(string s) { start_url_ = s; }
void Selenium::delay(int i) { delay_ = i; }

string Selenium::get_url(string url) 
{//get html document of url using selenium
	if(stoi(psstm("free | awk '{print $7}'")) < 300'000) {
		drv.attr("close")();
		drv.attr("quit")();
		drv = webdriver.attr("Firefox")();//py::arg("executable_path") = phantom_path_);
	}
	cout << url << endl;
	if(url[0] == '/') url = "https:" + url;
	//this_thread::sleep_for(delay_ * 1s);
	drv.attr("set_page_load_timeout")(30);
	try {
		drv.attr("get")(url);
	} catch(const exception &e) {
		cerr << "error at " << url << '\n' << e.what() << '\n';
		return "";
	}
	return drv.attr("page_source").cast<string>();
}

vector<string> Selenium::extract(string doc) const 
{//extract title and seller info and links from html document
	auto soup = bs(doc, "lxml");
	vector<string> r;
	auto title = soup.attr("find")(title_tag_, json("{\"class\" : \"" + title_class_ + "\" }"));
	cout << title << endl;
	r.push_back(title.is_none() ? "" : title.attr("get_text")().cast<string>());
	auto seller = soup.attr("find")(seller_tag_, json("{\"class\" : \"" + seller_class_ + "\"}"));
	r.push_back(seller.is_none() ? "" : seller.attr("get_text")().cast<string>());
	auto result = soup.attr("find_all")("a");
	for(auto a : result)
		if(auto link = a.attr("get")("href"); !link.is_none()) r.push_back(link.cast<string>());
	return r;
}

void Selenium::crawl(int depth)
{//top function of crawl, read and write graph file
	{
		ifstream f{graph_file_};
		if(f.is_open()) f >> grp_;
		else grp_.insert_vertex(start_url_);
	}
	crawl(0, depth, grp_.vertexes[0].edges.empty());
	save_graph();
}

void Selenium::save_graph()
{//use signal to save before Ctrl-C or crash
	ofstream f{graph_file_};
	f << grp_;
}

void Selenium::insert_data(string link)
{//write data of a url page and construct link graph
	auto v = extract(get_url(link));
	ofstream f{output_file_, fstream::app};
	f << v[0] << v[1];
	for(int i=2; i<v.size(); i++) {
		if(grp_.find_vertex(v[i]) == -1) {//if not exist
			grp_.insert_vertex(v[i]);
			grp_.insert_edge(link, v[i], 0);
		}
	}
}

void Selenium::crawl(int idx, int depth, bool read)
{//recursively crawl links
	if(depth == 0) return;
	string link = grp_.vertexes[idx].data;
	if(read) insert_data(link);
	for(auto &e : grp_.vertexes[idx].edges) {
		auto w = e.weight;
		e.weight = 1;//check visit
		crawl(e.index, depth - 1, w == 0i);
	}
}
