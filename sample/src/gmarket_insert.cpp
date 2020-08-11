#include<cstdio>
#include<string>
#include<fstream>
#include<database/mysqldata.h>
#include<iostream>
using namespace std;

SqlQuery sq;

struct Seller {
	string email, name, company, tel, address, homepage, country, source, product;
	void insert_db() {
		int i = 0;
		if(sq.select("email", "where email = '" + email + "'")) 
			sq.query("insert into product values (" + sq[0]["id"].asString() + ", '" + product + "')");
		else {
			sq.query("insert into email (email, name, company, tel, addr, homepage, " 
				"country, source) values ('" + email + "', '" + name + "', '" + company + 
				"', '" + tel + "', '" + address + "', '" + homepage + "', '" + country + 
				"', '" + source + "')");
			sq.select("email", "where email = '" + email + "'");
			sq.query("insert into product values (" + sq[0]["id"].asString() + ", '" + product + "')");
		}
		cout << i++ << ' ';
	}
} seller;

bool ok()
{
	puts(("product : " + seller.product).data());
	puts(("email : " + seller.email).data());
	puts(("name : " + seller.name).data());
	puts(("company : " + seller.company).data());
	puts(("tel : " + seller.tel).data());
	puts(("addr : " + seller.address).data());
	puts(("homepage : " + seller.homepage).data());
	puts(("country : " + seller.country).data());
	puts(("source : " + seller.source).data());
	puts("is this ok?");
	char c;
	cin >> c;
	return c == 'y';
}

istream &operator>>(istream &is, Seller &slr)
{
	string garbage;
	for(string s; s != ""; getline(is, s));
	getline(is, slr.product);
	getline(is, slr.name);
	getline(is, slr.company);
	getline(is, garbage);
	getline(is, slr.tel);
	getline(is, garbage);
	getline(is, garbage);
	getline(is, slr.email);
	getline(is, slr.address);
	for(int i=0; i<3; i++) getline(is, garbage);
	return is;
}

int main() {
	seller.source = "G market";
	seller.country = "KR";
	sq.connect("localhost", "zeta", "cockcodk0", "email");
	ifstream f{"/home/zeta/crawl/11st.txt"};
	while(f >> seller) if(ok()) seller.insert_db();
}

