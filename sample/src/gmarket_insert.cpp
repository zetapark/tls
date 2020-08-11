#include<cstdio>
#include<string>
#include<fstream>
#include<database/mysqldata.h>
#include<iostream>
using namespace std;

SqlQuery sq;

void insert_db(string *sa) {
	if(sq.select("email", "where email = '" + sa[7] + "'")) 
		sq.query("insert into product values (" + sq[0]["id"].asString() + ", '" + sa[0] + "')");
	else {
		sq.query("insert into email (email, name, company, tel, addr, country, source)"
				" values ('" + sa[7] + "', '" + sa[1] + "', '" + sa[2] + "', '" + sa[4] + "', '"
				+ sa[8] + "', 'KR', 'G Market')");
		sq.select("email", "where email = '" + sa[7] + "'");
		sq.query("insert into product values (" + sq[0]["id"].asString() + ", '" + sa[0] + "')");
	}
}

istream &operator>>(istream &is, char)
{//제품, 이름, 대표, *, 전화, 등록번호, *, 이메일, 주소, *, *
	string garbage;
	string sa[30], s;
	int i = 0;
	do {
		getline(is, s);
		sa[i++] = s;
	} while(s != "");
	if(i == 12) insert_db(sa);

	return is;
}

int main() {
	sq.connect("localhost", "zeta", "cockcodk0", "email");
	ifstream f{"/home/zeta/crawl/11st.txt"};
	char c;
	while(f >> c);
}

