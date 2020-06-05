#include<iostream>
#include<fstream>
#include<util/option.h>
#include<tcpip/server.h>
#include<tcpip/website.h>
using namespace std;

class Adnet : public WebSite
{
protected:
	SqlQuery sq;
	string id_;
	void process() {
		if(requested_document_ == "check.php") {
			if(nameNvalue_["psw"] != nameNvalue_["psw-repeat"]) content_ = "password not match";
			else if(!sq.select("Users", "where email = '" + nameNvalue_["email"] + "'")) 
				content_ = "email already exist";
			else if(!sq.select("Users", "where webpage_id = '" + nameNvalue_["webpage-id"] + "'"))
				content_ = "webpage id already exist";
			else {
				id_ = nameNvalue_["email"];
				SHA2 sha;
				auto a = sha.hash(nameNvalue_["psw"].cbegin(), nameNvalue_["psw"].cend());
				string enc = base64_encode({a.begin(), a.end()});
				sq.insert(nameNvalue_["email"], enc, 0, nameNvalue_["name"], nameNvalue_["webpage-id"],
						0, 0, 0, 0);
				content_ = "you are registered";
			}
		} else if(requested_document_ == "index.html") {//from login button
			sq.connect("localhost", "adnet", "adnetadnet", "adnet");
			if(id_ != "") {
				sq.select("Users", "where email = '" + id_ "'");
			}

		} 
	}
};

int main(int ac, char** av)
{
	CMDoption co{
		{"dir", "template directory", "www"},
		{"port", "listening port", 2001}
	};
	if(!co.args(ac, av)) return 0;

	Adnet site;
	site.init(co.get<const char*>("dir"));
	Server sv{co.get<int>("port")};
	return sv.keep_start(site);
}

