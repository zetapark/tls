#include<csignal>
#include<thread>
#include<functional>
#include<pybind11/embed.h>
#include<pybind11/functional.h>
#include<pybind11/pybind11.h>
#include<pybind11/stl.h>
#include<iostream>
#include<fstream>
#include"graph.h"

using namespace std;
namespace py = pybind11;

Graph<string> grp;
ofstream f{"crawl.txt", fstream::app};//crawl data write
void save_before_exit(int) {
	f.close();
	ofstream g{"graph.sav"};
	g << grp;
	exit(1);
}

void insert_data(string parent, vector<string> v) {
	f << v[0] << '\n' << v[1] << "\n\n";
	for(int i=2; i<v.size(); i++) {
		if(!grp.find_vertex(v[i])) {
			grp.insert_vertex(v[i]);
			grp.insert_edge(parent, v[i], 0);
		}
	}
}

int main(int ac, char **av)
{
	py::scoped_interpreter guard{};
	auto webdriver = py::module::import("selenium.webdriver");
	auto drv = webdriver.attr("PhantomJS")();
	auto json = py::module::import("json").attr("loads");
	auto bs = py::module::import("bs4").attr("BeautifulSoup");

	auto get_url = [&](string url) {//return string
		cout << url << endl;
		this_thread::sleep_for(5s);
		static int i = 0;
		if(i++ == 20) {
			i = 0;
			drv.attr("close")();
			drv = webdriver.attr("PhantomJS")();
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
		py::str seller = soup.attr("find")("div", json("{\"class\" : \"bsellerinfo\"}"));
		r.push_back(seller.cast<string>());
		auto result = soup.attr("find_all")("a");
		for(auto a : result)
			if(auto link = a.attr("get")("href"); !link.is_none()) r.push_back(link.cast<string>());
		for(auto a : r) cout << a << endl;
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

	function<void(Vertex<string>*, int, bool)> crawl = [&](Vertex<string> *v, int depth,
			bool read) {
		if(depth == 0) return;
		if(read) insert_data(v->data, get_titleNsellerNlinks(get_url(v->data)));
		for(auto e = v->edge; e; e = e->edge) {
			auto w = e->weight;
			e->weight = 1;//if error -> pass
			crawl(e->vertex, depth - 1, w == 0i);
		}
	};

	signal(SIGINT, save_before_exit);
	signal(SIGKILL, save_before_exit);
	signal(SIGTERM, save_before_exit);
	ifstream g{"graph.sav"};
	g >> grp;
	try {
		crawl(grp.data(), 3, !grp.data()->edge);
	} catch(exception &e) { cerr << e.what(); }
	catch(...) { }
	save_before_exit(1);
}

