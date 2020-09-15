#include"adnet.h"
using namespace std;

const char *langcss[2] = {".eng { display : none; }", ".kor { display : none; }"};
void Adnet2::process()
{
	if(!sq.reconnect()) sq.connect(db_ip_, "adnet", "adnetadnet", "adnet");
	if(requested_document_ == "index.html") index();
	else if(requested_document_ == "login.php") login();
	else if(requested_document_ == "signup.php") content_ = signup();
	else if(requested_document_ == "design.php") design();
	else if(requested_document_ == "mypage.html") mypage();
	else if(requested_document_ == "index.css") indexcss();
	else if(requested_document_ == "board.php") board();
	else if(requested_document_ == "lang.php") lang_++;
	else if(requested_document_ == "lang.css") content_ = langcss[lang_ % 2];
	else if(requested_document_ == "forgot.php") content_ = forgot();
	else if(requested_document_ == "option.php") pref(), content_ = "option saved";
}

void Adnet2::mypage()
{
	this->banner();
	this->preference();
}

void Adnet2::index()
{
	if(nameNvalue_["command"] == "logout") id_ = "";
}

void Adnet2::design() 
{
	if(id_ == "") content_ = "login first";
	else {
		Adnet::design();
		content_ = "design request completed";
	}
}

void Adnet2::login() 
{
	Adnet::index();
	content_ = id_ == "" ? "login failed" : "login succeeded";
}

void Adnet2::indexcss()
{
	if(id_ != "") {
		swap("none", "block");//viewonlog : block
		swap("block", "none");//hideonlog : none
	}
}
