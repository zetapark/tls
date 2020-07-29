#include<random>
#include<fstream>
#include<sha256.h>
#include<util/log.h>
#include"adnet.h"
using namespace std;

void Adnet::process()
{
	if(!sq.reconnect()) sq.connect(db_ip_, "adnet", "adnetadnet", "adnet");
	if(requested_document_ == "signup.php") content_ = signup();
	else if(requested_document_ == "index.html") index();//from login button
	else if(requested_document_ == "banner.html") banner();
	else if(requested_document_ == "preference.html") preference();
	else if(requested_document_ == "forgot.php") content_ = forgot();
	else if(requested_document_ == "emailcheck.php") content_ = email_check();
	else if(requested_document_ == "recommend.php") content_ = recommend();
	else if(requested_document_ == "pref.php") pref();
	else if(requested_document_ == "lang.php") lang_++;
	else if(requested_document_ == "lang.js") content_ = langjs[lang_ % 2];
//	else if(requested_document_.find('.') == string::npos) id_hit();//adnet.zeta2374.com/techlead
}

void Adnet::db_ip(string ip)
{
	db_ip_ = ip;
}

void Adnet::banner()
{
	for(int i=0; i<4; i++) swap("@ID", id_);
	if(nameNvalue_["leaderfilename"] != "") {
		ofstream f{"banner/" + id_ + "-leader.jpg"};
		f << nameNvalue_["leader"];
	}
	if(nameNvalue_["mobilefilename"] != "") {
		ofstream f{"banner/" + id_ + "-mobile.jpg"};
		f << nameNvalue_["mobile"];
	}
	if(nameNvalue_["squarefilename"] != "") {
		ofstream f{"banner/" + id_ + "-square.jpg"};
		f << nameNvalue_["square"];
	}
	if(nameNvalue_["skyfilename"] != "") {
		ofstream f{"banner/" + id_ + "-sky.jpg"};
		f << nameNvalue_["sky"];
	}
	if(string s = nameNvalue_["link"]; s != "")
		sq.query("update Users set link = '" + s + "' where id = '" + id_ + "'");
	if(string s = nameNvalue_["desc"]; s != "")
		sq.query("update Users set description = '" + s + "' where id = '" + id_ + "'");

	if(id_ != "" && sq.select("Users", "where id = '" + id_ + "'")) {
		prepend("<ul>", sq[0]["description"].asString());
		swap(":</li>", ": " + sq[0]["click_induce"].asString() + "</li>");
		swap(":</li>", ": " + sq[0]["show_induce"].asString() + "</li>");
		swap(":</li>", ": " + sq[0]["my_banner_show"].asString() + "</li>");
		swap(":</li>", ": " + sq[0]["my_banner_click"].asString() + "</li>");
		swap(":</li>", ": " + sq[0]["link"].asString() + "</li>");
		swap(":</li>", ": " + sq[0]["point"].asString() + "</li>");
	}
}

string Adnet::signup()
{
	if(nameNvalue_["psw"] != nameNvalue_["psw-repeat"]) return "password not match";
	if(stoi(nameNvalue_["num"]) != verify_code_) return "verification code not match";
	if(nameNvalue_["email"] != email_) return "email changed";
	if(sq.select("Users", "where id = '" + nameNvalue_["id"] + "'")) return "id already exist";

	if(nameNvalue_["remember"] == "on") id_ = nameNvalue_["id"];//signup pass
	SHA2 sha;
	auto a = sha.hash(nameNvalue_["psw"].cbegin(), nameNvalue_["psw"].cend());
	string enc = base64_encode({a.begin(), a.end()});
	sq.insert(nameNvalue_["id"], email_, enc, "", "", 0, 1, 0, 0, 0, "", 0, 0, 0, 0);

	if(sq.select("Users", "where id = '" + nameNvalue_["recommender"] + "'")) {//recommend bonus
		sq.query("update Users set click_induce = click_induce + 20 where id = '" +
				nameNvalue_["recommender"] + "'");
		sq.query("update Users set click_induce = click_induce + 30 where id = '" +
				nameNvalue_["id"] + "'");
	}
	return "you are registered";
}

void Adnet::index()
{
	if(string s = nameNvalue_["id"]; s != "") {//login
		SHA2 sha;
		string pass = nameNvalue_["pwd"];
		auto a = sha.hash(pass.cbegin(), pass.cend());
		pass = base64_encode({a.begin(), a.end()});

		if(sq.select("Users", "where id = '" + s + "' and password = '" + pass + "'"))
			id_ = sq[0]["id"].asString();
	} else if(s = nameNvalue_["opt"]; s != "") id_ = "";//logout

	swap("@ID", id_);
	swap("@LOGGED", id_ == "" ? "false" : "true");
	append("<body>", req_header_["IP-Addr"]);
}

void Adnet::id_hit()
{
	string id = requested_document_;
	content_ = fileNhtml_["index.html"];
	index();
	sq.select("Users", "limit 1");//need this to prevent error
	sq.query("update Users set click_induce = click_induce + 1 where id = '" + id + "'");
}

static string mailx(string src, string dst, string title, string content)
{
	string cmd = "mailx " + dst + " -r " + src + " -s '" + title + "' <<HERE_CONTENT\n"
		+ content + "\nHERE_CONTENT";
	system(cmd.data());
	return "mail sent";
}

string Adnet::forgot()
{
	if(string s = nameNvalue_["id"]; s != "") {
		if(sq.select("Users", "where id = '" + s + "'")) return sq[0]["email"].asString();
	} else if(s = nameNvalue_["email"]; s != "") {
		if(sq.select("Users", "where email = '" + s + "'")) return sq[0]["id"].asString();
	} else if(s = nameNvalue_["pwd"]; s != "") {
		uniform_int_distribution<> di{10000, 99999};
		random_device rd;
		key_ = di(rd);
		pwd_ = s;
		change_id_ = nameNvalue_["id_change"];
		return mailx("adnet@zeta2374.com", nameNvalue_["dest"], "change password from AdNET",
				"type next 5 digit number to activate the new password\n" + to_string(key_));
	} else if(s = nameNvalue_["num"]; s != "" && key_ > 9999 && key_ == stoi(s)) {
		SHA2 sha;
		auto a = sha.hash(pwd_.begin(), pwd_.end());
		sq.query("update Users set password = '" + base64_encode({a.begin(), a.end()})
				+ "' where id = '" + change_id_ + "'");
	}
	return "";
}

string Adnet::email_check()
{
	uniform_int_distribution<> di{10000, 99999};
	random_device rd;
	verify_code_ = di(rd);
	email_ = nameNvalue_["email"];
	return mailx("adnet@zeta2374.com", email_, "email verification", 
			"type next 5 digits to verify email\n" + to_string(verify_code_));
}

string Adnet::recommend()
{
	if(sq.select("Users", "where id = '" + nameNvalue_["id"] + "'")) return "id exist";
	else return "id does not exist";
}

static const string category[] = {"computer", "programming", "game", "gadgets",
	"education", "shopping", "travel", "food", "entertainment", "society",
	"fashion", "health", "finance", "other", "distance", "country"};

void Adnet::pref()
{//php
	sq.query("delete from Pref where id = '" + id_ + "'");
	string command = "insert into Pref values ('" + id_ + "'";
	for(const string s : category) command += nameNvalue_[s] == "" ? ",0" : ",1";
	sq.query(command + ')');
	sq.query("update Users set lat = " + nameNvalue_["lat"] + ", lng = "
			+ nameNvalue_["lng"] + ", nation = '" + nameNvalue_["nation"] + "', km = "
			+ nameNvalue_["km"] + " where id = '" + id_ + "'");
	content_ = "<meta http-equiv=refresh content='0; url = https://adnet.zeta2374.com' />";
}

void Adnet::preference()
{
	if(sq.select("Pref", "where id = '" + id_ + "'"))
		for(string s : category) if(sq[0][s].asBool()) append("name=" + s, " checked");
	if(sq.select("Users", "where id = '" + id_ + "'")) {
		if(sq[0]["lat"].asFloat()) append("name=lat", " value=" + sq[0]["lat"].asString());
		if(sq[0]["lng"].asFloat()) append("name=lng", " value=" + sq[0]["lng"].asString());
		if(string s = sq[0]["nation"].asString(); s != "") append("value=" + s, " checked");
	}
}
