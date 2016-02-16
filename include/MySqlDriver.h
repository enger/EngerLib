#pragma once
#include <config.h>
#include <string>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <boost/thread/mutex.hpp>
using namespace std;
class ENGERLIB_EXP CMySqlDriver
{
public:
	CMySqlDriver(void);
	~CMySqlDriver(void);
public:
	string server_adds;
	string	usrname;
	string password;
	string database;
private:
	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	boost::mutex mutexdatabase;
	//sql::ResultSet* res;
	//sql::PreparedStatement* pstmt;
public:
	bool	Connect();
	sql::ResultSet* GetQueryResult(string sql);
	int	WriteIntoDatabase(string sql);
};

