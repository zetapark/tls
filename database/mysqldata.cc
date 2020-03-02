#include<chrono>
#include<initializer_list>
#include<unordered_map>
#include<ctime>
#include<sstream>
#include"mysqldata.h"
#include"options/log.h"
using namespace std;

SqlQuery::SqlQuery(const SqlQuery& r) : Mysqlquery{r}
{ }

bool SqlQuery::insert()
{
	std::string q = "insert into " + table_name + " values (" + values_.str();
	q.back() = ')';
	q += ";";
	values_.str("");
	LOGD << q << endl;
	return myQuery(q);
}

bool SqlQuery::is_int(int n)
{
	return columns[n].type.find("INT") != string::npos;
}

bool SqlQuery::is_real(int n)
{
	return columns[n].type.find("FLOAT") != string::npos || 
		columns[n].type.find("DOUBLE") != string::npos;
}

bool SqlQuery::insert(vector<string> v) 
{//d should be 1 record
	string q = "insert into " + table_name + " values (";
	for(int i=0; i<columns.size(); i++) {
		string s = v[i];
		if(columns[i].type == "INT" || columns[i].type == "FLOAT" || s == "null" || columns[i].type == "DOUBLE") q += s + ",";
		else q += "'" + s + "',";
	}
	q.back() = ')';
	q += ";";
	return myQuery(q);
}

bool SqlQuery::insert_nth_row(int n)
{//seekp rewind, insert the data of sq[n]
	stringstream ss; ss << "insert into " << table_name << " (";
	for(int i=0; i<columns.size(); i++) ss << columns[i].name << ',';
	ss.seekp(-1, ss.cur); ss << ") values (";
	for(int i=0; i<columns.size(); i++) ss << (*this)[n][columns[i].name] << ',';
	ss.seekp(-1, ss.cur); ss << ");";
	return myQuery(ss.str());
}

string SqlQuery::now()
{
	myQuery("select now();");
	res->next();
	return res->getString(1);
}

string SqlQuery::encrypt(string s)
{
	myQuery("select password('" + s + "');");
	res->next();
	return res->getString(1);
}

int SqlQuery::select(string table, string where)
{
	string q = "select * from " + table + ' ' + where + ';';
	myQuery(q);

	table_name = table;
	sql::ResultSetMetaData* mt = res->getMetaData();
	columns.clear(); clear();
	int c = mt->getColumnCount();
	for(int i = 0; i < c; i++) //populate columns
		columns.push_back({mt->getColumnName(i+1), mt->getColumnDisplaySize(i+1), mt->getColumnTypeName(i+1)});
	for(int j=0; res->next(); j++) for(int i = 0; i < c; i++) { //populate contents
		if(is_int(i)) (*this)[j][columns[i].name] = res->getInt(i+1);
		else if(is_real(i)) 
			(*this)[j][columns[i].name] = static_cast<double>(res->getDouble(i+1));
		else (*this)[j][columns[i].name] = static_cast<string>(res->getString(i+1));
	}
	return size();
}

vector<string> SqlQuery::show_tables()
{
	vector<string> record;
	myQuery("show tables;");
	while(res->next()) record.push_back(res->getString(1));
	return record;
}

void SqlQuery::group_by(vector<string> v)
{//v:group by column name, will return like old mysql group by
	if(!size()) return;
	Json::Value r;
	r[0] = (*this)[0];
	for(int i=1, k=1; i<size(); i++) for(int j=0; j<v.size(); j++) 
		if((*this)[i][v[j]] != (*this)[i-1][v[j]]) {//if diffrent insert
			r[k++] = (*this)[i];
			break;
		}
	this->Json::Value::operator=(r);
}
