#pragma once
#include<vector>
#include<sstream>
#include<json/json.h>
#include<cstring>
#include<type_traits>
#include"mysqlquery.h"

class SqlQuery : public Mysqlquery, public Json::Value
{
public:
	SqlQuery() = default;
	SqlQuery(const SqlQuery& r);
	bool query(std::string q);
	int fetch(int n = -1);
	int select(std::string table, std::string where = "");
	bool insert_nth_row(int n);
	bool insert(std::vector<std::string> v);
	template<class T1, class... T2> bool insert(T1 a, T2... b)
	{
		values_ << a << ",";
		return insert(b...);
	}
	template<class... T> bool insert(std::string s, T... b)
	{
		if(s == "NULL") values_ << s << ',';
		else values_ << "'" << s << "',";
		return insert(b...);
	}
	template<class... T> bool insert(const char* p, T... b)
	{
		if(strcmp(p, "NULL")) values_ << "'" << p << "',";
		else values_ << p << ',';
		return insert(b...);
	}
	bool insert();
	
	void group_by(std::vector<std::string> v);
	
	std::string encrypt(std::string pass);
	std::vector<std::string> show_tables();
	std::string now();//system clock->mysql datetime string

protected:
	struct Column {
		std::string name;
		unsigned size;
		std::string type;
	};
	std::string table_name;
	std::vector<Column> columns;
	int column_count_;

private:
	bool is_int(int nth_column);
	bool is_real(int nth_column);
	std::stringstream values_;//this is not copy constructable.copy constructor needed
};

