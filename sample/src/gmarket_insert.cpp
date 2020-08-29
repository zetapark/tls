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
		sq.query("insert into email (email, company, name, tel, addr, country, source)"
				" values ('" + sa[7] + "', '" + sa[1] + "', '" + sa[2] + "', '" + sa[6] + "', '"
				+ sa[3] + "', 'KR', 'auction')");
		sq.select("email", "where email = '" + sa[7] + "'");
		sq.query("insert into product values (" + sq[0]["id"].asString() + ", '" + sa[0] + "')");
	}
}

istream &operator>>(istream &is, char)
{//제품, 회사, 대표, 주소, *, *, 전화, 이메일
	string garbage;
	string sa[30], s, t;
	int i = 0;
	do {
		getline(is, s);
	} while(s == "");
	t = s;
	getline(is, s);
	if(s != "") sa[0] = s;
	else sa[0] = t;
	do {
		getline(is, s);
	} while(s == "");
	sa[1] = s;
	for(i=2; i<7; i++) {
		getline(is, s);
		sa[i] = s;
	}
	for(int k : {1, 3, 4, 5, 6}) sa[k] = sa[k].substr(sa[k].find(':')+1);
	int idx = sa[6].find('/');
	sa[7] = sa[6].substr(idx + 2);
	sa[6] = sa[6].substr(0, idx-1);

	insert_db(sa);

	return is;
}

int main() {
	sq.connect("localhost", "zeta", "cockcodk0", "email");
	ifstream f{"auction.txt"};
	char c;
	while(f >> c);
}

