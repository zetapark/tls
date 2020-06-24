#include<sha256.h>
#include"adnet.h"
using namespace std;

void Adnet::process()
{
	if(requested_document_ == "signup.php") content_ = signup();
	else if(requested_document_ == "index.html") index();//from login button
	else id_hit();
}

string Adnet::signup()
{
	if(nameNvalue_["psw"] != nameNvalue_["psw-repeat"]) return "password not match";
	if(!sq.select("Users", "where email = '" + nameNvalue_["email"] + "'")) 
		return "email already exist";
	if(!sq.select("Users", "where id = '" + nameNvalue_["id"] + "'"))
		return "id already exist";
	if(nameNvalue_["remember"] == "on") id_ = nameNvalue_["id"];
	SHA2 sha;
	auto a = sha.hash(nameNvalue_["psw"].cbegin(), nameNvalue_["psw"].cend());
	string enc = base64_encode({a.begin(), a.end()});
	sq.insert(nameNvalue_["email"], enc, 0, nameNvalue_["id"], 0, 0, 0, 0, "");
	return "you are registered";
}

void Adnet::index()
{
	sq.connect("localhost", "adnet", "adnetadnet", "adnet");
	swap("@LOGGED", id_ == "" ? "false" : "true");
	if(id_ != "") {
		sq.select("Users", "where email = '" + id_ + "'");
		swap("@ID", sq[0]["id"].asString());
	}
}

void Adnet::id_hit()
{
	sq.query("update Users set click_induce = click_induce + 1 where id = '"
			+ requested_document_ + "'");
	content_ = fileNhtml_["index.html"];
	index();
}
