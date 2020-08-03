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
		{"receiver", "receiver email address", 15},//zeta
		{"sender", "sender email address", 14},//adnet
		{"msg", "message id", 2}//gmail event
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
}

