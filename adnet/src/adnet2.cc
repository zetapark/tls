#include"adnet.h"
using namespace std;

void Adnet2::process()
{
	if(!sq.reconnect()) sq.connect(db_ip_, "adnet", "adnetadnet", "adnet");
	if(requested_document_ == "index.html") index();
	else if(requested_document_ == "login.php") login();
	else if(requested_document_ == "signup.php") content_ = this->signup();
	else if(requested_document_ == "forgot.html") forgot();
	else if(requested_document_ == "mypage.html") mypage();
	else if(requested_document_ == "index.css") indexcss();
	else if(requested_document_ == "design.php") design();
	else if(requested_document_ == "board.php") board();
	else if(requested_document_ == "lang.php") lang_++;
	else if(requested_document_ == "lang.js") content_ = langjs[lang_ % 2];
	else if(requested_document_ == "index.js") swap("@LOGGED", id_ == "" ? "false" : "true");
	else if(requested_document_ == "forgot.html") signin();
}

void Adnet2::index()
{
	if(nameNvalue_["command"] == "logout") id_ = "";
}

void Adnet2::login() 
{
	Adnet::index();
	content_ = id_ == "" ? "login failed" : "login succeeded";
}

void Adnet2::design() 
{
	if(id_ == "") content_ = "alert('login first');";
	else {
		Adnet::design();
		content_ = "alert('design request completed');";
	}
}

void Adnet2::indexcss()
{
	if(id_ != "") {
		swap("none", "block");//viewonlog : block
		swap("block", "none");//hideonlog : none
	}
}

