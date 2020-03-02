#include<cassert>
#include<fstream>
#include<regex>
#include"dndd.h"
using namespace std;

static string level2txt(array<int, 5> allow)
{
	string s[6] = {"anonymous", "registered", "regular", "representative", "senior", "root"};
	string r = "<html><body><h3>Read level &#x2267 " + s[allow[0]];
	r += "<br>Write level &#x2267 " + s[allow[1]];
	r += "<br>Comment level &#x2267 " + s[allow[2]];
	r += "<br>Vote level &#x2267 " + s[allow[3]];
	r += "<br>Voting options = " + to_string(allow[4]) + "</h3></body></html>";
	return r;
}

string base64_encode(vector<uint8_t> v);
vector<uint8_t> base64_decode(string s);
void DnDD::pg()
{
	if(nameNvalue_["add"] != "") {//from add
		if(stoi(level) >= allow[1]) {//from page.html, check write level
			sq.select(table, "where num = " + book + " order by page desc limit 1");
			page = to_string(maxpage(table, book) + 1);
			sq.insert({book, page, id, "Edit this page", "", sq.now(), "null"});
		} else content_ = "<script>alert('your level does not qualify.')</script>";
	} else if(nameNvalue_["read"] != "") {//from new 
		sq.select(table, "order by num desc limit 1");
		book = to_string(sq[0]["num"].asInt() + 1);
		sq.insert({book, "0", id, nameNvalue_["title"], 
				nameNvalue_["read"] + nameNvalue_["write"] + nameNvalue_["comment"] 
				+ nameNvalue_["vote"] + '0' + nameNvalue_["option"] + '0', 
				sq.now(), "null"});
		page = "0";
	} else if(nameNvalue_["title"] != "") {//from edit
		sq.select(table, "limit 1");//if from page no date
		sq.insert({book, page, id, nameNvalue_["title"], nameNvalue_["content"],
				tmp.empty() ? sq.now() : tmp["date"].asString(), "null"});
	} else if(nameNvalue_["comment"] != "") {//if from comment
		sq.select(table, "limit 1");
		sq.insert({book, page, id, "코멘트임.", nameNvalue_["comment"], sq.now(), "null"});
	} else {//if get method
		table = nameNvalue_["table"];
		book = nameNvalue_["book"];
		page = nameNvalue_["page"]; 
	}
	int max_page = maxpage(table, book);
	int ipage = stoi(page);
	allow = allowlevel(table, book);

	if(stoi(level) < allow[0]) {//check read level
		content_ = "<script>alert(\"not enough level to read this article\")</script>";
		return;
	}

	//set buttons
	swap("FIRST", table + "&book=" + book + "&page=0");
	swap("PREV", table + "&book=" + book + "&page=" + to_string(ipage ? ipage-1 : 0));
	swap("NEXT", table + "&book=" + book + "&page=" + to_string(ipage == max_page ? max_page : ipage + 1));
	swap("LAST", table + "&book=" + book + "&page=" + to_string(max_page));
	swap("RESULT", table + "&book=" + book + "&option=" + to_string(allow[4]) + "&db=" + db);
	string r, s = "<label class=\"radio-inline\"><input type=\"radio\" name=\"option\" value=\"";
	for(int i=1; i<=allow[4]; i++) 
		r += s + to_string(i) + "\">" + to_string(i) + "</label>\n";
	swap("OPTIONS", r);

	//main frame
	sq.select(table, "where num=" + book + " and page=" + page + " and title <> '코멘트임.' order by edit desc limit 1");
	swap("FOLLOW", sq[0]["email"].asString());
	swap("TITLE", "<small>" + table + '.' + book + '.' + page + ".</small>" + sq[0]["title"].asString());

	if(page == "0") {
		iframe_content_ = level2txt(allow);
		sq.select("(select tt.num, tt.page, email, title, contents, date, edit from "
				+ table + " tt inner join "
				+ " (select num, page, max(edit) as maxedit from " + table
				+ " where title <> '코멘트임.' and num = " + book 
				+ " group by num, page) tmp on tt.num = tmp.num and "
				+ " tt.page = tmp.page and edit = tmp.maxedit) tmp2"
				, "where title <> '코멘트임.' order by page");
		string s = "<br>";
		for(int i=1; i<sq.size(); i++)  
			s += "<a target=blank href='page.html?table=" + table + "&book="
				+ book + "&page=" + to_string(i) + "'>" + to_string(i) + ". " 
				+ sq[i]["title"].asString() + "</a><br>";
		iframe_content_.insert(iframe_content_.find("</body>"), s);
	} else {
		auto v = base64_decode(sq[0]["contents"].asString());
		iframe_content_ = string{v.begin(), v.end()};
	}

	tmp = sq[0];//5 date
	string date = sq[0]["date"].asString();
	sq.select("Users", "where email='" + sq[0]["email"].asString() + "' order by date desc limit 1");
	swap("WHO", "by " + sq[0]["name"].asString() + " on " + date);
	//swap("WHO", "by " + sq[0]["email"].asString() + " on " + sq[0]["date"].asString());

	//attachment덧글
	sq.select(table, "where num=" + book + " and page=" + page + " and title = '코멘트임.' order by date desc, email, edit desc");
	sq.group_by({"date", "email", "edit"});
	string t;
	for(int i=0; i<sq.size(); i++) {
		t += "<div class=\"panel-heading\">written by ";
		t += sq[i]["email"].asString() + " on " + sq[i]["date"].asString();
		t += "</div>\n<div class=\"panel-body\">";
		t += sq[i]["contents"].asString() + "</div>\n";
	}
	swap("ATTACHMENT", t);
}

