#include<csignal>
#include<selenium.h>
using namespace std;

Selenium sel{"/home/zeta/crawl/phantomjs"};

void save_before_exit(int) {
	sel.save_graph();
	exit(1);
}

int main()
{
	signal(SIGINT, save_before_exit);
	signal(SIGKILL, save_before_exit);
	signal(SIGTERM, save_before_exit);

	sel.start_url("https://www.coupang.com");
	sel.output_file("coupang.txt");
	sel.graph_file("coupang.graph");
	sel.seller_tag("table");
	sel.seller_class("prod-deliverty-return-policy-table");
	sel.title_tag("h2");
	sel.title_class("prod-buy-header__title");
	sel.crawl(3);
}

