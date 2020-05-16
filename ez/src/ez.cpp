#include<iostream>
#include<fstream>
#include<util/option.h>
#include<tcpip/server.h>
#include<tcpip/website.h>
using namespace std;

string car;

class MySite : public WebSite
{
protected:
	void process() {
		if(requested_document_ == "result_view") {
			content_ = "<html><h2>";
			for(auto [a, b] : nameNvalue_) content_ += a + ':' + b + "<br>";
			content_ += "</h2></html>";
		} else if(requested_document_ == "korean") content_ = language = "korean";
		else if(requested_document_ == "english") content_ = language = "english";
		else if(requested_document_ == "tutorial.js") if(language == "korean") swap("kor", "eng");
		else if(requested_document_ == "index.html") swap("@CAROUSEL", car);
		cout << requested_document_ << endl;
	}
	string language = "korean";
};

int main(int ac, char** av)
{
	CMDoption co{
		{"dir", "template directory", "www"},
		{"port", "listening port", 2001}
	};
	if(!co.args(ac, av)) return 0;

	ifstream f("../carousel.txt");
	int n; string s; vector<string> v[3];
	f >> n; getline(f, s);
	for(int i=0; i<n; i++) for(int j=0; j<3; j++) {
		getline(f, s);
		v[j].push_back(s);
	}
	car = carousel(v[0], v[1], v[2]);

	MySite site;
	site.init(co.get<const char*>("dir"));
	Server sv{co.get<int>("port")};
	return sv.start(site);
}
