#include<cassert>
#include<fstream>
#include<regex>
#include<cassert>
#include"dndd.h"
using namespace std;

vector<string> DnDD::tables()
{//return data tables
	vector<string> v;
	auto tb = sq.show_tables();
	for(auto s : tb) if(s != "Users" && s != "Vote" && s != "Follow") v.push_back(s);
	return v;
}

array<int, 5> DnDD::allowlevel(string table, string book)
{//read write comment vote
	array<int, 5> ar;
	sq.select(table, "where num=" + book + " and page=0 and title <> \'코멘트임.\' order by edit desc limit 1");

	string s = sq[0]["contents"].asString();
	for(int i=0; i<4; i++) ar[i] = s[i] - '0';
	ar[4] = s[5] - '0';
	for(int i=0; i<5; i++) cout << ar[i];
	return ar;
}

int DnDD::maxpage(string table, string book)
{
	assert(sq.select(table, "where num=" + book + " order by page desc limit 1") > 0);
	return sq[0]["page"].asInt();
}

string DnDD::field(string s)
{//return table contents as bootstrap panel string
	vector<string> v;
	string t;
	sq.select(s, "where title <> \'코멘트임.\' order by num desc, page, date, edit desc");
	sq.group_by({"email", "date"});
	for(int i=0; i<sq.size(); i++) {
		string n = sq[i]["num"].asString();
		string tt = sq[i]["title"].asString();
		string p = sq[i]["page"].asString();
		if(sq[i]["page"] == 0) {//if book
			t += "<div class=\"panel-heading\"><a href=\"." + n;
			t += "\" data-toggle=\"collapse\">" + n + ". " + tt + "</div>\n";
		} 
		t += "<div class=\"panel-body collapse " + n;
		t += "\">&nbsp;&nbsp;<a href=\"page.html?table=" + s + "&book=" + n;
		t += "&page=" + p + "\">" + p + ". " + tt + "</a></div>\n";
	} 
	return t;
}

