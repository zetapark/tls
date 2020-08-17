#include<ctime>
#include<random>
#include<fstream>
#include<sha256.h>
#include<gmpxx.h>
#include<rsa.h>
#include<util/log.h>
#include<mpz.h>
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
	else if(requested_document_ == "search.php") search();
	else if(requested_document_ == "design.php") design();
	else if(requested_document_ == "board.php") board();
//	else if(requested_document_.find('.') == string::npos) id_hit();//adnet.zeta2374.com/techlead
}

void Adnet::board()
{//generate a link to login remotely somewhat like oauth service -> goto board 
	if(id_ == "") return;
	mpz_class K = 0xcfef40c72702c7f8f28bff1cad418831a874dd79b084c6efea4eb4c3e3b767ee373ef784e32aa3be50c064100ab2da02aca17cc6af4a8beab9cc47c54e353bf314ce44fbbd73b5e91ab2573fbfa7edb12f8c52120f0f6ddcb70205288302ac0813bfe66460880d9480e7112aed5f27abb93ca4bb311484d5569e23d4b39440ce53445d70bb0367f913a8a730d60192e4dbbb0e0cc2930b930e92223646be7f0e99e7ff2cfb7c16fbea074ef095f558c2bf2ddd5e1d749a1dffff3eff8b46d9b77ef2f61d8eae2b878ba665935c68c95562df09bedafccb0b9c44d9bab15b16b74e715a7c89a806b1463e7015b11115c155ffa4d3c6d27fc2f16946902b037ccb_mpz;
	mpz_class e = 0x10001;

	RSA rsa{e, 1, K};
	sq.select("Users", "where id = '" + id_ + "'");
	string m = struct2str(time(nullptr)) + sq[0]["password"].asString();
	stringstream ss;
	ss << hex << rsa.encode(bnd2mpz(m.begin(), m.end()));
	content_ = "<meta http-equiv=refresh content='0; url = https://dndd.zeta2374.com/main.html?db=zetapark&field=Adnet&id=" + id_ + "&sign=" + ss.str() + "' />";
}

void Adnet::search()
{
	if(string s = nameNvalue_["search"]; s != ""){
		sq.select("Users", "where description like '%" + s + "%' or id like '%" + s + "%'");
		for(int i=0; i<sq.size(); i++)
			content_ += "<a href='" + sq[i]["link"].asString() + "' target=_blank>" +
				sq[i]["id"].asString() + " : " + sq[i]["description"].asString() + "</a><br>";
	}
}

void Adnet::db_ip(string ip)
{
	db_ip_ = ip;
}

static string mailx(string src, string dst, string title, string content)
{
	string cmd = "mailx " + dst + " -r " + src + " -s '" + title + "' <<HERE_CONTENT\n"
		+ content + "\nHERE_CONTENT";
	system(cmd.data());
	return "mail sent";
}

void Adnet::design()
{
	if(string s = nameNvalue_["backgroundfilename"]; s != "") {
		ofstream f{"design/" + id_ + '-' + s};
		f << nameNvalue_["background"];
	}
	if(string s = nameNvalue_["logofilename"]; s != "") {
		ofstream f{"design/" + id_ + '-' + s};
		f << nameNvalue_["logo"];
	}
 	mailx("ask@zeta2374.com", "adnet@zeta2374.com", "디자인 요청", id_ + "님이\n" +
			nameNvalue_["desc"] + "\n위의 내용과 아래의 기타 요청사항\n" + nameNvalue_["other"]);
	content_ = "<meta http-equiv=refresh content='0; url = https://adnet.zeta2374.com' />";
}

void Adnet::banner()
{
	if(id_ != "" && sq.select("Users", "where id = '" + id_ + "'")) {
		int point = sq[0]["point"].asInt(), click_induce = sq[0]["click_induce"].asInt();
		if(nameNvalue_["submit"] == "crypto money") {
			if(int amount = stoi(nameNvalue_["point"]); 100 < amount) 
				mailx(sq[0]["email"].asString(), "adnet@zeta2374.com", "암호 화폐 전환 요청",
						id_ + "님이 " + to_string(amount) + "의 포인트를 암호화폐로 전환 요청하였습니다.");
		} else if(nameNvalue_["submit"] == "click induce") {
			if(int amount = stoi(nameNvalue_["point"]); amount <= point) {
				point -= amount; click_induce += amount;
				sq.query("update Users set point = " + to_string(point) + ", click_induce = " 
						+ to_string(click_induce) + " where id = '" + id_ + "'");
			}
		}
		prepend("<ul>", "<h4>" + sq[0]["description"].asString()) + "</h4>";
		swap(":</li>", ": " + to_string(click_induce) + "</li>");
		swap(":</li>", ": " + sq[0]["show_induce"].asString() + "</li>");
		swap(":</li>", ": " + sq[0]["my_banner_show"].asString() + "</li>");
		swap(":</li>", ": " + sq[0]["my_banner_click"].asString() + "</li>");
		swap(":</li>", ": " + sq[0]["link"].asString() + "</li>");
		swap(":</li>", ": " + to_string(point) + "</li>");
		append("min=0 max=", to_string(point));
		swap("max=>", "max=" + to_string(point) + '>');
	}

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
}

void Adnet::id_hit()
{
	string id = requested_document_;
	content_ = fileNhtml_["index.html"];
	index();
	sq.select("Users", "limit 1");//need this to prevent error
	sq.query("update Users set click_induce = click_induce + 1 where id = '" + id + "'");
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
	"fashion", "health", "finance", "others", "distance", "country"};

void Adnet::pref()
{//php
	string command;
	if(sq.select("Pref", "where id = '" + id_ + "'")) {
		command = "update Pref set ";
		for(const string &s : category) command += s + " = " + (nameNvalue_[s]=="" ? "0," : "1,");
		command.back() = ' ';
		command += " where id = '" + id_ + "'";
	} else {
		command = "insert into Pref values ('" + id_ + "', ";
		for(const string &s : category) command += nameNvalue_[s] == "" ? ",0" : ",1";
		command += ')';
	}
	sq.query(command);

	if(nameNvalue_["lat"] == "") nameNvalue_["lat"] = "0";
	if(nameNvalue_["lng"] == "") nameNvalue_["lng"] = "0";
	if(nameNvalue_["km"] == "") nameNvalue_["km"] = "0";
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
		append("name=lat", " value=" + sq[0]["lat"].asString());
		append("name=lng", " value=" + sq[0]["lng"].asString());
		if(string s = sq[0]["nation"].asString(); s != "") append("value=" + s, " checked");
		append("name=km", " value=" + sq[0]["km"].asString());
	}
}
