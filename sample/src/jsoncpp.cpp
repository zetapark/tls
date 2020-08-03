#include<iostream>
#include<fstream>
#include<util/option.h>
#include<database/mysqldata.h>
using namespace std;

string base64_encode(vector<unsigned char> v);
vector<unsigned char> base64_decode(string s);

string mailx(string src, string dst, string title, string filename, string option)
{
	string cmd = "mailx " + dst + " -r " + src + " -a '" + option + "' -s '" + title
		+ "' < " + filename;
	system(cmd.data());
	return "mail sent";
}

int main(int ac, char **av)
{
	CMDoption co{
		{"file", "mail file to send", "mail.html"},
		{"title", "mail title", "AdNET"},
		{"option", "mail send option", "Content-Type: text/html"},
		{"language", "mail content language", "KR"},
		{"description", "mail content description", ""},
		{"pure_text", "if mail is html or pure text", false},
		
		{"send", "send mail?", false},
		{"receiver", "receiver email address", 15},//zeta
		{"sender", "sender email address", 14},//adnet
		{"msg", "message id", 2},//gmail event

		{"view", "view message content", false}
	};
	if(!co.args(ac, av)) return 0;

	SqlQuery sq;
	sq.connect("localhost", "zeta", "cockcodk0", "email");
//	sq.connect("localhost", "adnet", "adnetadnet", "adnet");
//	sq.query("select * from Pref right join "
//			"(select id, link, nation, lat, lng, etc from Users where click_induce <> 0 "
//			"order by (my_banner_show / click_induce) limit 100) as t1 "
//			"on t1.id = Pref.id");
//	sq.fetch(-1);
//	cout << sq << endl;
//	sq.removeIndex(0, &sq[0]);
//	cout << sq << endl;
//
	if(co.get<bool>("send")) {
		string receiver, sender, title, filename, option;
		int rec = co.get<int>("receiver");
		int msg = co.get<int>("msg");
		if(sq.select("email", "where id = " + to_string(rec)))
			receiver = sq[0]["email"].asString();
		if(sq.select("email", "where id = " + to_string(co.get<int>("sender"))))
			sender = sq[0]["email"].asString();
		if(sq.select("message", "where mid = " + to_string(msg))) {
			filename = "~/mail/" + sq[0]["filename"].asString();
			option = sq[0]["html"].asBool() ? "Content-Type: text/html" : "";
			title = sq[0]["title"].asString();
		}
		mailx(sender, receiver, title, filename, option);
		sq.select("send", "limit 1");
		sq.insert(rec, msg, sq.now(), option);
	} else if(co.get<bool>("view")) {
		if(sq.select("message", "where mid = " + to_string(co.get<int>("msg")))) {
			auto v = base64_decode(sq[0]["content"].asString());
			string msg = string{v.begin(), v.end()};
			cout << msg << endl;
		}
	} else {//insert a mail file into database
		ifstream f{co.get<const char*>("file")};
		vector<unsigned char> v;
		for(unsigned char c; f >> noskipws >> c;) v.push_back(c);
		sq.query("insert into message (title, content, language, html, description) values ('" 
				+ string{co.get<const char*>("title")} + "', '" + base64_encode(v) + "', '"
				+ co.get<const char*>("language") + "', " + (co.get<bool>("pure_text") ? '0' : '1')
				+ ", '" + co.get<const char*>("description") + "')");
	}
}

