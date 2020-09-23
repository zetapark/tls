#include<ctime>
#include<random>
#include<fstream>
#include<tls/sha256.h>
#include<gmpxx.h>
#include<tls/rsa.h>
#include<util/log.h>
#include<tls/mpz.h>
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
	if(id_ == "") {
		content_ = "<meta http-equiv=refresh content='0; url = https://dndd.zeta2374.com/main.html?db=zetapark&field=AdNET' />";
		return;
	}
	mpz_class K = 0xd728fbd450aa96d5e9f7e1e3c95cffeb789b6b14e543f96035df49d453d58b2c489df5792ec2f2957368eac50a7cba5b8fc909b94ae8e91e0f5004bd8b14c3d93c2f5e0f722dca610daba2f8bb672233126117b5e98b4e15835ccfe56a8624c8380a9ff2c73d7cd0e5641cfca954478b35f269d2838ed36d4258b2f1579135b7debb2450cd92280e0eaabe22cd84b4f9be5604e7f64bf7e4e181ede901ff5df06dd24e9456e54ae06338ae516d9ab462bb20bbfda89629026e95bdc1585ddf31ec2c8b61de9a40dbf8f5645bea05222e9e405288851e7d906117d2bd3c110a53ba20d9d00f27898e2ff910280345952ced9f94a85c170c4dc25f8cc57207c8eb_mpz;
	mpz_class e = 0x10001;

	RSA rsa{e, 1, K};
	sq.select("Users", "where id = '" + id_ + "'");
	string m = struct2str(time(nullptr)) + sq[0]["password"].asString();
	stringstream ss;
	ss << hex << rsa.encode(bnd2mpz(m.begin(), m.end()));
	content_ = "<meta http-equiv=refresh content='0; url = https://dndd.zeta2374.com/main.html?db=zetapark&field=AdNET&id=" + id_ + "&sign=" + ss.str() + "' />";
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

static string mailx(string src, string dst, string title, string content, bool html = false)
{
	string cmd = "mailx " + dst + (html ? " -a 'Content-Type: text/html'" : "") +
		" -r " + src + " -s '" + title + "' <<HERE_CONTENT\n" + content + "\nHERE_CONTENT";
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
		prepend("<ul>", "<h4>" + sq[0]["description"].asString() + "</h4>");
		swap(":</li>", ": " + to_string(click_induce) + "</li>");
		swap(":</li>", ": " + sq[0]["show_induce"].asString() + "</li>");
		swap(":</li>", ": " + sq[0]["my_banner_show"].asString() + "</li>");
		swap(":</li>", ": " + sq[0]["my_banner_click"].asString() + "</li>");
		swap(":</li>", ": " + sq[0]["link"].asString() + "</li>");
		swap(":</li>", ": " + to_string(point) + "</li>");
		append("min=0 max=", to_string(point));
		swap("max=>", "max=" + to_string(point) + '>');
	}

	for(int i=0; i<8; i++) swap("@ID", id_);
	if(nameNvalue_["leaderfilename"] != "") {
		ofstream f{"stage/" + id_ + "-leader.jpg"};
		f << nameNvalue_["leader"];
	}
	if(nameNvalue_["mobilefilename"] != "") {
		ofstream f{"stage/" + id_ + "-mobile.jpg"};
		f << nameNvalue_["mobile"];
	}
	if(nameNvalue_["squarefilename"] != "") {
		ofstream f{"stage/" + id_ + "-square.jpg"};
		f << nameNvalue_["square"];
	}
	if(nameNvalue_["skyfilename"] != "") {
		ofstream f{"stage/" + id_ + "-sky.jpg"};
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

	//for board use
	SqlQuery sq2;
	sq2.connect(db_ip_, "dndd", "dndddndd", "zetapark");
	sq2.select("Users", "limit 1");
	sq2.insert(nameNvalue_["id"], enc, 2, email_, "", sq2.now());

	if(sq.select("Users", "where id = '" + nameNvalue_["recommender"] + "'")) {//recommend bonus
		sq.query("update Users set click_induce = click_induce + 20 where id = '" +
				nameNvalue_["recommender"] + "'");
		sq.query("update Users set click_induce = click_induce + 30 where id = '" +
				nameNvalue_["id"] + "'");
	}
	mailx("adnet@zeta2374.com", nameNvalue_["email"], "AdNET 가입을 환영합니다.", R"(<html>
	<head>
		<meta charset="utf-8" />
		<meta content="width=device-width, initial-scale=1, shrink-to-fit=no" name="viewport" />
	</head>
	<body>
		<div style="text-align: center; padding-top: 200px; padding-bottom: 200; background: url('https://tomcat.zeta2374.com/image/back.jpg') repeat">
			<a href=https://adnet.zeta2374.com>
				<img src=https://tomcat.zeta2374.com/image/adnet.png width=150>
			</a>
			<p>
			AdNET 가입을 환영합니다.
			</p>

			<p>
			AdNET의 제일 목표는 회원 여러분의 상품, 웹사이트 혹은 컨텐츠가 좀 더 널리 알려지고 
			활성화되는 것입니다.
			AdNET은 일반적인 광고회사가 아니며, AdNET이 하는 역할은 크지 않습니다.
			AdNET은 회원 여러분이 서로 도울 수 있는 근거지를 만들어주고자 합니다.
			AdNET은 플랫폼을 통해 회원 여러분이 서로 도울 수 있는 방법을 제공할 뿐입니다.
			</p>

			<p>
			인터넷은 연결망입니다.
			한 회원의 웹사이트 혹은 컨텐츠가 다른 웹사이트를 광고해 주고, 
			그 다른 웹사이트 혹은 컨텐츠는 나의 것을 광고해 준다면, 
			내가 나의 컨텐츠만을 광고하는 고립된 사이트보다 인터넷 망의 효과는 배가됩니다.
			</p>

			<p>
			거기에 더하여 광고를 통해 얻은 수익을 조금이라도 배분하고자 합니다.
			</p>

			<p>
			AdNET을 활용하기 위해 해야 할 첫번째 단계는 
			회원님의 웹페이지에 AdNET <a href=https://adnet.zeta2374.com/insert.html>배너를 삽입</a>하는 것입니다.

			그 다음은 여러분의 상품을 광고할 배너를 만들거나,
			<a href=https://adnet.zeta2374.com#design-request>배너 제작을 의뢰</a>하는 것입니다.
			이벤트 기간 중에는 배너 제작을 무료로 해드리고 있습니다.
			</p>
			<p style=margin-top:100>
			스팸 메일로 분류되었을 경우, adnet@zeta2374.com을 주소록에 추가해 주세요.
			</p>
		</div>
	</body>
</html>)", true);
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

string code_email(int code) {
	return R"(
<html>
	<head>
		<meta charset="utf-8" />
		<meta content="width=device-width, initial-scale=1, shrink-to-fit=no" name="viewport" />
	</head>
	<body>
		<div style="text-align: center; padding-top: 200px; padding-bottom: 200; background: url('https://tomcat.zeta2374.com/image/back.jpg') repeat">
			<a href=https://adnet.zeta2374.com>
				<img src=https://tomcat.zeta2374.com/image/adnet.png width=150>
			</a>
			<p>
			Thanks for using AdNET.
			</p>
			<h3>Please Enter this verification Code</h3>
			<h4>)" + to_string(code) + R"(</h4>
			<p style=margin-top:100>
			스팸 메일로 분류되었을 경우, adnet@zeta2374.com을 주소록에 추가해 주세요.
			</p>
		</div>
	</body>
</html>)";
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
				code_email(key_), true);
	} else if(s = nameNvalue_["num"]; s != "" && key_ > 9999 && key_ == stoi(s)) {
		SHA2 sha;
		auto a = sha.hash(pwd_.begin(), pwd_.end());
		auto enc = base64_encode({a.begin(), a.end()});
		sq.query("update Users set password = '" + enc + "' where id = '" + change_id_ + "'");

		//for board use
		SqlQuery sq2;
		sq2.connect(db_ip_, "dndd", "dndddndd", "zetapark");
		sq2.select("Users", "limit 1");
		sq2.insert(change_id_, enc, 2, email_, "", sq2.now());
		
		return "password changed";
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
			code_email(verify_code_), true);
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
		command = "insert into Pref values ('" + id_ + "' ";
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
		if(string s = sq[0]["nation"].asString(); s != "") append("value=" + s, " selected checked");
		append("name=km", " value=" + sq[0]["km"].asString());
	}
}
