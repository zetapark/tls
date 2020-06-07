
#include<iostream>
#include<fstream>
#include<util/option.h>
#include<tcpip/server.h>
#include<tcpip/website.h>
using namespace std;

class Ad : public WebSite
{
protected:
	SqlQuery sq;
	string id_;
	void process() {
		if(requested_document_ == "request_ad.php") {
			if(nameNvalue_["psw"] != nameNvalue_["psw-repeat"]) content_ = "password not match";
			else if(!sq.select("Users", "where email = '" + nameNvalue_["email"] + "'")) 
				content_ = "email already exist";
			else if(!sq.select("Users", "where id = '" + nameNvalue_["id"] + "'"))
				content_ = "id already exist";
			else {
				if(nameNvalue_["remember"] == "on") id_ = nameNvalue_["id"];
				SHA2 sha;
				auto a = sha.hash(nameNvalue_["psw"].cbegin(), nameNvalue_["psw"].cend());
				string enc = base64_encode({a.begin(), a.end()});
				sq.insert(nameNvalue_["email"], enc, 0, nameNvalue_["id"], 0, 0, 0, 0, "");
				content_ = "you are registered";
			}
		} else if(requested_document_ == "clicked.php") {//from login button
			sq.connect("localhost", "adnet", "adnetadnet", "adnet");
			swap("@LOGGED", id_ == "" ? "false" : "true");
			if(id_ != "") {
				sq.select("Users", "where email = '" + id_ "'");
				swap("@ID", sq[0]["id"]);
			}
		} else {
			sq.query("update Users set id_hit = id_hit + 1 where id = '" + requested_document_ + "'");
			content_ = fileNhtml_["index.html"];
			index();
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

