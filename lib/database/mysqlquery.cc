#include <iostream>
#include <stdexcept>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
//#include<mysql_connection.h>
#include "mysqlquery.h"
using namespace std;

bool Mysqlquery::myQuery(std::string str)
{
	cerr << str << endl;
	try {
        cerr << "Executing query....."; 
//		str = dynamic_cast<sql::mysql::MySQL_Connection*>(con)->escapeString(str);
        res = stmt->executeQuery(str);
		cerr << "Done." << endl;
        return true;
	} catch (sql::SQLException &e) {
		if(e.getErrorCode() != 0) {
			cout << "# ERR: SQLException in " << __FILE__ ;
			cout << "(" << __FUNCTION__<< ") on line " << __LINE__  << endl;
			cout << "# ERR: " << e.what();
			cout << "  (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )"  << endl << endl;
		}
		return false;
	}
}

bool Mysqlquery::connect(string host, string user, string pass, string db)
{
    try {
        //cout << "Connecting database to " << host << endl << endl;
        driver = get_driver_instance();
        con = driver->connect(host, user, pass);
        con->setSchema(db);
        stmt = con->createStatement();
        //std::cout << "done.." << std::endl;
        return true;
    } catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__ ;
		cout << "(" << __FUNCTION__<< ") on line " << __LINE__  << endl;
		cout << "# ERR: " << e.what();
		cout << "  (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )"  << endl << endl;
		return false;
	}
}

void Mysqlquery::connect(Mysqlquery& copy)
{
	driver = copy.driver;
	con = copy.con;
	stmt = copy.stmt;
	res = copy.res;
}

Mysqlquery::~Mysqlquery()
{
	try {
		cerr << "trying to close" << endl;
        con->close();
        if(con) delete con;
		if(res) delete res;
		if(stmt) delete stmt;
        driver->threadEnd();
		cout << "mysql closed" << endl;
    } catch(sql::SQLException &e) {
        cerr << "Failed To Close Connection to DataBase Server" << e.what() << endl;       }
//	cout << "mysql query destroying" << endl;
//    if(con != NULL) {con->close(); delete con;}
//    if(stmt != NULL) delete stmt;
//    if(res != NULL) delete res;
}
