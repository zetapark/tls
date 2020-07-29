#include<iostream>
#include<database/mysqldata.h>
using namespace std;

int main()
{
	SqlQuery sq;
	sq.connect("localhost", "adnet", "adnetadnet", "adnet");
	sq.query("select * from Pref right join "
			"(select id, link, nation, lat, lng, etc from Users where click_induce <> 0 "
			"order by (my_banner_show / click_induce) limit 100) as t1 "
			"on t1.id = Pref.id");
	sq.fetch(-1);
	cout << sq << endl;
	sq.removeIndex(0, &sq[0]);
	cout << sq << endl;
}

