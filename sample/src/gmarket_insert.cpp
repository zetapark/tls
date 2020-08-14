#include<cstdio>
#include<string>
#include<fstream>
#include<database/mysqldata.h>
#include<iostream>
using namespace std;

SqlQuery sq; 

void insert_db(string *sa) {
	if(sq.select("email", "where email = '" + sa[5] + "'")) 
		sq.query("insert into product values (" + sq[0]["id"].asString() + ", '" + sa[0] + "')");
	else {
		sq.query("insert into email (email, company, name, tel, addr, country, source)"
				" values ('" + sa[5] + "', '" + sa[1] + "', '" + sa[2] + "', '" + sa[3] + "', '"
				+ sa[9] + "', 'KR', 'G Market')");
		sq.select("email", "where email = '" + sa[5] + "'");
		sq.query("insert into product values (" + sq[0]["id"].asString() + ", '" + sa[0] + "')");
	}
}

istream &operator>>(istream &is, char)
{//제품, 회사, 대표, 전화, *, 이메일, *, *, *, 주소
	string garbage;
	string sa[30], s;
	int i = 0;
	do {
		getline(is, s);
		sa[i++] = s;
	} while(s != "");
	if(i == 13) {
		for(int j=1; j<10; j++) sa[j] = sa[j+1];
		sa[0] = sa[0].substr(0, sa[0].find("<dl class"));
		for(int j=1; j<10; j++) {
			string t = sa[j];
			int start = t.find("<dd>") + 4;
			int end = t.find("</dd>");
			sa[j] = t.substr(start, end - start);
		}
		insert_db(sa);
	}

	return is;
}

int main() {
	sq.connect("localhost", "zeta", "cockcodk0", "email");
	ifstream f{"crawl.txt"};
	char c;
	while(f >> c);
}

