#include<csignal>
#include<thread>
#include<functional>
#include<pybind11/embed.h>
#include<pybind11/functional.h>
#include<pybind11/pybind11.h>
#include<pybind11/stl.h>
#include<iostream>
#include<fstream>
#include<util/option.h>
#include"cpp_graph.h"

using namespace std;
namespace py = pybind11;

Graph<string> grp;
ofstream f{"auction.txt", fstream::app};//crawl data write
void save_before_exit(int) {
	f.close();
	{
		ofstream g{"auction_link.graph"};
		g << grp;
	}
	exit(1);
}

void insert_data(string parent, vector<string> v) {
	f << v[0] << v[1];
	for(int i=2; i<v.size(); i++) {
		if(grp.find_vertex(v[i]) == -1) {//if not exist
			grp.insert_vertex(v[i]);
			grp.insert_edge(parent, v[i], 0);
		}
	}
}

int main(int ac, char **av)
{
	CMDoption co{
		{"executable_path", "selenium executable path", "/home/zeta/crawl/phantomjs"}
	};
	if(!co.args(ac, av)) return 0;

	py::scoped_interpreter guard{};
	auto webdriver = py::module::import("selenium.webdriver");
	auto drv = webdriver.attr("PhantomJS")(py::arg("executable_path") = co.get<const char*>("executable_path"));
	auto json = py::module::import("json").attr("loads");
	auto bs = py::module::import("bs4").attr("BeautifulSoup");

	auto get_url = [&](string url) {//return string
		cout << url << endl;
		this_thread::sleep_for(5s);
		static int i = 0;
		if(i++ == 20) {
			i = 0;
			drv.attr("close")();
			drv = webdriver.attr("PhantomJS")(py::arg("executable_path") = co.get<const char*>("executable_path"));
		}
		drv.attr("get")(url);
		return drv.attr("page_source").cast<string>();
	};

	auto get_auction = [&](string doc) {
		auto soup = bs(doc, "lxml");
		vector<string> r;
		auto title = soup.attr("find")("h1", json("{\"class\" : \"itemtit\" }"));
		cout << title << endl;
		r.push_back(title.is_none() ? "" : title.attr("get_text")().cast<string>());
		auto seller = soup.attr("find")("div", json("{\"class\" : \"bsellerinfo\"}"));
		cout << seller << endl;
		r.push_back(seller.is_none() ? "" : seller.attr("get_text")().cast<string>());
		auto result = soup.attr("find_all")("a");
		for(auto a : result)
			if(auto link = a.attr("get")("href"); !link.is_none()) r.push_back(link.cast<string>());
		return r;
	};

	auto get_titleNsellerNlinks = [&](string doc) {//return vector<string>
		auto soup = bs(doc, "lxml");//gmarket
		vector<string> r;
		auto title = soup.attr("find")("h1", json("{\"class\" : \"itemtit\" }"));
		cout << title << endl;
		r.push_back(title.is_none() ? "" : title.attr("get_text")().cast<string>());
		py::str seller = soup.attr("find")("dl", json("{\"class\" : \"exchange_data seller_data\"}"));
		r.push_back(seller.is_none() ? "" : seller.cast<string>());
		auto result = soup.attr("find_all")("a");
		for(auto a : result)
			if(auto link = a.attr("get")("href"); !link.is_none()) r.push_back(link.cast<string>());
		for(auto a : r) cout << a << endl;
		return r;
	};

	function<void(int, int, bool)> crawl = [&](int idx, int depth, bool read) {
		if(depth == 0) return;
		string link = grp.vertexes[idx].data;
		if(read) insert_data(link, get_auction(get_url(link)));
		for(auto &e : grp.vertexes[idx].edges) {
			auto w = e.weight;
			e.weight = 1;
			crawl(e.index, depth - 1, w == 0i);
		}
	};

	signal(SIGINT, save_before_exit);
	signal(SIGKILL, save_before_exit);
	signal(SIGTERM, save_before_exit);
	ifstream g{"auction_link.graph"};
	g >> grp;
	try {
		crawl(0, 3, grp.vertexes[0].edges.empty());
	} catch(exception &e) { cerr << e.what(); }
	catch(...) { }
	save_before_exit(1);
}


