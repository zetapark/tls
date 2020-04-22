#include<fstream>
#include<string>
#include"my_site.h"
#include"database/util.h"
using namespace std;

Site::Site()
{//SqlQuery destructor -> mysqlquery destructor nullify
	sq.connect("localhost", "site", "sitesite", "site");//sq prohibit destruct
}

void Site::process()
{
	cout << requested_document_ << endl;
	for(auto& a : nameNvalue_) cout << a.first << ':' << a.second << endl;
	if(requested_document_ == "index.html") index();
	else if(requested_document_ == "up.cgi") content_ = upload();
	else if(requested_document_ == "signup.html") signup();
}
void Site::index()
{
	if(nameNvalue_["title"] != "") {//from edit
		sq.select("bbs", "limit 1");
		sq.insert(nameNvalue_["title"], nameNvalue_["content"], sq.now(), 
			1, 1, 1, sq[0]["edit"].asInt() + 1, "zezeon@msn.com");
	}
	sq.select("bbs", "where num=1 and page=1 and comment_order=1 order by edit desc limit 1");
	swap("@TEXT", sq[0]["content"].asString());
	swap("@TITLE", sq[0]["title"].asString());
}

string Site::upload()
{
	ofstream f(nameNvalue_["filename"]);
	f << nameNvalue_["file"];
	return nameNvalue_["goods"] + nameNvalue_["desc"];
}

void Site::signup()
{
//	swap("GROUP", group);
//	swap("GROUP_DESC", group_desc);
//	swap("LOGO", logo);
	if(nameNvalue_.empty()) return;
	if(nameNvalue_["password"] != nameNvalue_["verify"])
		append("REPLACE\">", "password not match");
	else if(sq.select("users", "where email='" + nameNvalue_["email"] + "';"))
		append("REPLACE\">", "아이디가 이미 존재합니다.");
	else {//select will retrieve table structure, which makes inserting possible
		sq.insert(nameNvalue_["email"], nameNvalue_["username"], sq.encrypt(nameNvalue_["password"]), 1, sq.now());
		if(nameNvalue_["check"] != "") 
			id = nameNvalue_["email"], level = 1, name = nameNvalue_["username"];
		append("REPLACE\">", "가입완료<br><a href='index.html'>메인화면으로</a><br>");
	}

	const char *append_str[]//to remember user input
		= {"email", "password", "verify", "username"};
	for(string s : append_str) 
		append("id=\"" + s + '\"', " value='" + nameNvalue_[s] + '\'');
	cout << id << endl;
}
