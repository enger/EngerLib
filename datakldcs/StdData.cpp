#include "StdData.h"
//#include <datacontainer.h>
#include <strstream>
#include <vector>
#include <SocketClientTCP.h>
#include <MySqlDriver.h>
#include <boost/thread/mutex.hpp>
#include <boost/threadpool.hpp>
#include <globaldeclear.h>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

typedef boost::date_time::local_adjustor< boost::posix_time::ptime, +8, boost::posix_time::no_dst> sct_shz;// 上海 东八区
boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));

CStdData::CStdData(const std::shared_ptr<CBufferObj>& buffer)
{

}

CStdData::CStdData(const std::string & strData)
{
	try
	{
		vector<string> fields;
		boost::split(fields,strData,boost::is_any_of(",;"));
		if (fields.size()>=12 && fields[0].length()==8)
		{
			string strgpstime=fields[0]+"T"+fields[1];
			boost::posix_time::ptime ptgpstime = boost::posix_time::from_iso_string(strgpstime);
			ptgpstime = sct_shz::local_to_utc(ptgpstime);
			m_ulGPSTime = (ptgpstime - epoch).total_seconds();
			m_strCityFlag=fields[2];
			m_strGPSID=fields[3];
			m_dLongitude=atof(fields[4].c_str());
			m_dLatitude=atof(fields[5].c_str());
			m_dSpeed=atof(fields[6].c_str());
			m_dDir=atof(fields[7].c_str());
			m_iType=atoi(fields[8].c_str());
			m_iLoadState=atoi(fields[9].c_str());
			m_iValid=atoi(fields[10].c_str());
			string strrcvtime=fields[11];
			boost::posix_time::ptime ptrcvtime = boost::posix_time::time_from_string(strrcvtime);
			ptrcvtime = sct_shz::local_to_utc(ptrcvtime);
			m_ulRCVTime = (ptrcvtime - epoch).total_seconds();
		}
	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());
	}

}

CStdData::~CStdData(void)
{
}
void CStdData::Parse(const std::shared_ptr<CBufferObj>& buffer)
{

}
string CStdData::ToString()
{
	using namespace boost::posix_time;
	using namespace std;
	using namespace boost::gregorian;

	if (m_strGPSID.length()==0)
	{
		return "";
	}
	else
	{
		ptime ptGPS(boost::date_time::c_local_adjustor<ptime>::utc_to_local(from_time_t(m_ulGPSTime)));
		ptime ptRCV(boost::date_time::c_local_adjustor<ptime>::utc_to_local(from_time_t(m_ulRCVTime)));
		//日期部分
		string strgpstime = to_iso_string(ptGPS);
		std::replace( strgpstime.begin(), strgpstime.end(), 'T', ','); 
		string strrvctime = to_iso_extended_string(ptRCV);
		std::replace( strrvctime.begin(), strrvctime.end(), 'T', ' ');
		date d1 = ptGPS.date();  
		// 获取时间部分  
		time_duration td = ptGPS.time_of_day(); 

		std::stringstream ss;
		ss<<strgpstime<<",";
		ss<<m_strCityFlag<<","
			<<m_strGPSID<<",";
		int prec=numeric_limits<double>::digits10;
		ss.precision(prec);//覆盖默认精度
		ss<<m_dLongitude<<","
			<<m_dLatitude<<",";

		ss<<m_dSpeed<<","
			<<m_dDir<<","
			<<m_iType<<","
			<<m_iLoadState<<","
			<<m_iValid<<",";
		ss<<strrvctime<<";";
		string strRet=ss.str();
		ss.clear();
		strRet.append("\r\n");
		return strRet;
	}

}
