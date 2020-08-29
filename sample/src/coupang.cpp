#include<iostream>
#include<csignal>
#include<util/selenium.h>
#include<util/option.h>
using namespace std;

Selenium *p = nullptr;
void save_before_exit(int) {
	if(p) p->save_graph();
	exit(1);
}
void err(int) {
	throw 10;
}

int main(int ac, char **av)
{
	CMDoption co{
		{"executable_path", "phantomjs executable file", "./phantomjs"}
	};
	if(!co.args(ac, av)) return 0;

	signal(SIGINT, save_before_exit);
	signal(SIGKILL, save_before_exit);
	signal(SIGTERM, save_before_exit);//continue 

	Selenium sel{co.get<const char*>("executable_path")};
	p = &sel;
	sel.start_url("https://www.coupang.com");
	sel.output_file("coupang.txt");
	sel.graph_file("coupang.graph");
	sel.seller_tag("div");
	sel.seller_class("product-seller");
	sel.title_tag("h2");
	sel.title_class("prod-buy-header__title");
	sel.delay(0);
	sel.crawl(3);
}

