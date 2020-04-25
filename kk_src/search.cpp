#include"tcpip/server.h"
#include"database/mysqldata.h"
#include"tcpip/website.h"
using namespace std;

class MyWeb : public WebSite
{
protected:
	void process() {
		sq.connect("localhost", "kk", "kk", "kyungki");
		sq.select("가맹점", "where 상호 like '%" + nameNvalue_["name"] + "%' and 주소 like '%" 
				+ nameNvalue_["adress"] + "%' and 업종 like '%" + nameNvalue_["kind"] + "%' limit 100");
		string s;
		for(auto a : sq) {
			s += "<tr><td>" + a["상호"].asString() + "</td><td>" + a["주소"].asString()
				+ "</td><td>" + a["업종"].asString() + "</td><td>" + a["전화번호"].asString()
				+ "</td></tr>";
		}
		append("<tbody>", s);
	}
	SqlQuery sq;
};

int main(int ac, char **av)
{
	MyWeb f;
	f.init("kk_html");
	Server sv{atoi(av[1])};
	sv.start(f);
}

