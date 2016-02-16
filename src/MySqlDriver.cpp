#include <MySqlDriver.h>
#include <globaldeclear.h>
#include <strstream>
CMySqlDriver::CMySqlDriver(void)
	:driver(0)
	,con(0)
	,stmt(0)
{
	try
	{
		driver = get_driver_instance();
	}
	catch (sql::SQLException &e)
	{
		std::stringstream strss;
		strss<<"# ERR: SQLException in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what()<<" (MySQL error code:"<<e.getErrorCode()
			<<", SQLState:"<<e.getSQLState()<<")";
		WriteLog(strss.str());
	}
}


CMySqlDriver::~CMySqlDriver(void)
{
	if (con!=0)
	{
		con->close();
		delete con;
		con=0;
	}
	if (stmt!=0)
	{
		delete stmt;
		stmt=0;
	}
}
bool	CMySqlDriver::Connect()
{
	bool ret=false;
	try
	{
		if (con!=0)
		{
			con->close();
			delete con;
			con=0;
		}
		if (stmt!=0)
		{
			delete stmt;
			stmt=0;
		}
		string url="tcp://"+server_adds;
		con = (driver->connect(url.c_str(), usrname.c_str(), password.c_str()));
		/* Connect to the MySQL test database */
		con->setSchema(database.c_str());
		stmt = (con->createStatement());
		stmt->execute("set names gb2312");
		stmt->execute("set character set gb2312");
		ret=true;
	}
	catch (sql::SQLException &e)
	{
		std::stringstream strss;
		strss<<"# ERR: SQLException in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what()<<" (MySQL error code:"<<e.getErrorCode()
			<<", SQLState:"<<e.getSQLState()<<")";
		WriteLog(strss.str());
	}
	return ret;
}
sql::ResultSet* CMySqlDriver::GetQueryResult(string sql)
{
	boost::mutex::scoped_lock lock(mutexdatabase);
	sql::ResultSet* ret=0;
	try
	{
		bool bconneted = false;
		if (con==NULL || !con->isValid() || con->isClosed())
		{
			bconneted = Connect();
		}
		else
		{
			bconneted=true;
		}
		if (bconneted)
		{
			sql::PreparedStatement* pstmt = (con->prepareStatement(sql.c_str()));
			ret=(pstmt->executeQuery());
			delete pstmt;
			pstmt=0;
		}
	}
	catch (sql::SQLException &e)
	{
		std::stringstream strss;
		strss<<"# ERR: SQLException in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what()<<" (MySQL error code:"<<e.getErrorCode()
			<<", SQLState:"<<e.getSQLState()<<")";
		WriteLog(strss.str());
	}
	return ret;
}
int	CMySqlDriver::WriteIntoDatabase(string sql)
{
	int ret=-1;
	boost::mutex::scoped_lock lock(mutexdatabase);
	try
	{
		bool bconneted = false;
		if (con==NULL || !con->isValid() || con->isClosed())
		{
			bconneted = Connect();
		}
		else
		{
			bconneted=true;
		}
		if (bconneted)
		{
			sql::PreparedStatement* pstmt = (con->prepareStatement(sql.c_str()));
			pstmt->execute();
			delete pstmt;
			pstmt=0;
		}
	}
	catch (sql::SQLException &e)
	{
		std::stringstream strss;
		strss<<"# ERR: SQLException in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what()<<" (MySQL error code:"<<e.getErrorCode()
			<<", SQLState:"<<e.getSQLState()<<")";
		WriteLog(strss.str());
	}
	return ret;
}