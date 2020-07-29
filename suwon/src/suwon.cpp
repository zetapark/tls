#include<mutex>
#include<util/log.h>
#include<util/option.h>
#include<tcpip/server.h>
#include<database/mysqldata.h>
#include<tcpip/website.h>
using namespace std;

class MyWeb : public WebSite
{
protected:
	void process() {
		if(string s; requested_document_ == "index.html") {
			sq.connect("192.168.0.3", "kk", "kk", "kyungki");
			sq.select("가맹점", "where 상호 like '%" + nameNvalue_["name"] + "%' and 주소 like '%" +
					nameNvalue_["adress"] + "%' and 업종 like '%" + nameNvalue_["kind"] + "%' limit 100");
			for(auto a : sq) {
				s += "<tr><td>" + a["상호"].asString() + "</td><td>" + a["주소"].asString()
					+ "</td><td>" + a["업종"].asString() + "</td><td>" + a["전화번호"].asString()
					+ "</td></tr>";
			}
			append("<tbody>", s);
			LOGD << s << endl;
		}
	}
	SqlQuery sq;
};

int main(int ac, char **av)
{
	CMDoption co{
		{"dir", "template directory", "www"},
		{"port", "listening port", 2001}
	};
	if(!co.args(ac, av)) return 0;

	MyWeb f;
	f.init(co.get<const char*>("dir"));
	Server sv{co.get<int>("port")};
	sv.keep_start(f);
}

