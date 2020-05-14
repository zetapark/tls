#pragma once
#include <string>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>

class Mysqlquery 
{
private:
    sql::Driver *driver = nullptr;
    sql::Connection *con = nullptr;
    sql::Statement *stmt = nullptr;

protected:
	sql::ResultSet *res = nullptr;
    bool myQuery(std::string str);

public:
    bool connect(std::string host, std::string user, std::string pass, std::string db);
    void connect(Mysqlquery& copy);
	virtual ~Mysqlquery();
};

