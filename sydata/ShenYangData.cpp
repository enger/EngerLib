#include "ShenYangData.h"
#include <boost/threadpool.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <algorithm>
using namespace boost::posix_time;
using namespace std;
using namespace boost::gregorian;
typedef boost::date_time::local_adjustor< boost::posix_time::ptime, +8, boost::posix_time::no_dst> sct_shz;// �Ϻ� ������

ptime epoch(date(1970,1,1));

CShenYangData::CShenYangData(const std::shared_ptr<CBufferObj>& buffer)
	:m_strASYN("ASYN")
	,m_strType("")
	,m_dMileage(0)
	,m_uiAlarmWord(0)
	,m_uiStateWord(0)
	,m_strMSG("")
{
	m_strCityFlag="SY";
	shared_ptr<char> strbuffer = shared_ptr<char>(new char[buffer->buff_length()+1]);
	memset(&*strbuffer,0,buffer->buff_length()+1);
	memcpy_s(&*strbuffer,buffer->buff_length()+1,&*(buffer->ptr_buffer),buffer->buff_length());
	
	string strData(&*strbuffer);
	vector<string> fields;
	//strData="ASYN GPS MMC8000GPSANDASYN051113-802015-00000000|1451354423|0|123485619|41811939|7.104514|0.0|00000000|00040000|";
	boost::split(fields,strData,boost::is_any_of("| "));
	if (
		(//��Ϣ�ֶ�Ϊ��
		(fields.size()==oriFieldsCount && fields[oriFieldsCount-1].length()==8)
		||//��Ϣ�ֶβ�Ϊ��
		(fields.size()==(oriFieldsCount+1) && fields[oriFieldsCount].length()>0)
		)//��ʼ��־
		&& fields[0]=="ASYN" )
	{
		m_strASYN=fields[0];
		m_strType=fields[1];
		m_strGPSID=fields[2];
		m_ulGPSTime=strtoul(fields[3].c_str(),0,0);
		m_dMileage=strtod(fields[4].c_str(),0);
		m_dLongitude=strtod(fields[5].c_str(),0)/1e6;
		m_dLatitude=strtod(fields[6].c_str(),0)/1e6;
		m_dSpeed=strtod(fields[7].c_str(),0);
		m_dDir=strtod(fields[8].c_str(),0);
		m_uiAlarmWord=strtoul(fields[9].c_str(),0,0);
		m_uiStateWord=strtoul(fields[10].c_str(),0,0);

		m_iLoadState=LoadMask&m_uiStateWord;
		m_iValid=ValidMask&m_uiStateWord;
		if (fields.size()==(oriFieldsCount+1))
		{
			m_strMSG=fields[11];
		}
		m_ulRCVTime=(microsec_clock::universal_time() - epoch).total_seconds();
	}

}
CShenYangData::CShenYangData(const std::string & strData)
	:m_strASYN("ASYN")
	,m_strType("")
	,m_dMileage(0)
	,m_uiAlarmWord(0)
	,m_uiStateWord(0)
	,m_strMSG("")

{
	m_strCityFlag="SY";
	vector<string> fields;
	//string strData="ASYN GPS MMC8000GPSANDASYN051113-802015-00000000|1451354423|0|123485619|41811939|7.104514|0.0|00000000|00040000|";
	boost::split(fields,strData,boost::is_any_of("| "));
	if (
		(//��Ϣ�ֶ�Ϊ��
		(fields.size()==oriFieldsCount && fields[oriFieldsCount-1].length()==8)
		||//��Ϣ�ֶβ�Ϊ��
		(fields.size()==(oriFieldsCount+1))
		)//��ʼ��־
		&& fields[0]=="ASYN" )
	{
		m_strASYN=fields[0];
		m_strType=fields[1];
		m_strGPSID=fields[2];
		m_ulGPSTime=strtoul(fields[3].c_str(),0,0);
		m_dMileage=strtod(fields[4].c_str(),0);
		m_dLongitude=strtod(fields[5].c_str(),0)/1e6;
		m_dLatitude=strtod(fields[6].c_str(),0)/1e6;
		m_dSpeed=strtod(fields[7].c_str(),0);
		m_dDir=strtod(fields[8].c_str(),0);
		m_uiAlarmWord=strtoul(fields[9].c_str(),0,0);
		m_uiStateWord=strtoul(fields[10].c_str(),0,0);

		m_iLoadState=LoadMask&m_uiStateWord;
		m_iValid=ValidMask&m_uiStateWord;

		if (fields.size()==(oriFieldsCount+1))
		{
			m_strMSG=fields[11];
		}
		m_ulRCVTime=(microsec_clock::universal_time() - epoch).total_seconds();
	}

}

CShenYangData::~CShenYangData(void)
{
}
void CShenYangData::Parse(const std::shared_ptr<CBufferObj>& buffer)
{
	m_strGPSID="";
	shared_ptr<char> strbuffer = shared_ptr<char>(new char[buffer->buff_length()+1]);
	memset(&*strbuffer,0,buffer->buff_length()+1);
	memcpy_s(&*strbuffer,buffer->buff_length()+1,&*(buffer->ptr_buffer),buffer->buff_length());

	string strData(&*strbuffer);
	vector<string> fields;
	//strData="ASYN GPS MMC8000GPSANDASYN051113-802015-00000000|1451354423|0|123485619|41811939|7.104514|0.0|00000000|00040000|";
	boost::split(fields,strData,boost::is_any_of("| "));
	if (
		(//��Ϣ�ֶ�Ϊ��
		(fields.size()==oriFieldsCount && fields[oriFieldsCount-1].length()==8)
		||//��Ϣ�ֶβ�Ϊ��
		(fields.size()==(oriFieldsCount+1) && fields[oriFieldsCount].length()>0)
		)//��ʼ��־
		&& fields[0]=="ASYN" )
	{
		m_strASYN=fields[0];
		m_strType=fields[1];
		m_strGPSID=fields[2];
		m_ulGPSTime=strtoul(fields[3].c_str(),0,0);
		m_dMileage=strtod(fields[4].c_str(),0);
		m_dLongitude=strtod(fields[5].c_str(),0);
		m_dLatitude=strtod(fields[6].c_str(),0);
		m_dSpeed=strtod(fields[7].c_str(),0);
		m_dDir=strtod(fields[8].c_str(),0);
		m_uiAlarmWord=strtoul(fields[9].c_str(),0,0);
		m_uiStateWord=strtoul(fields[10].c_str(),0,0);

		m_iLoadState=LoadMask&m_uiStateWord;
		m_iValid=ValidMask&m_uiStateWord;

		if (fields.size()==(oriFieldsCount+1))
		{
			m_strMSG=fields[11];
		}
		m_ulRCVTime=(microsec_clock::universal_time() - epoch).total_seconds();

	}
}
//std::shared_ptr<CBufferObj> CShenYangData::GetFormateData()
//{
//	using namespace boost::posix_time;
//	using namespace std;
//	using namespace boost::gregorian;
//
//	if (m_strGPSID.length()==0)
//	{
//		return std::shared_ptr<CBufferObj>();
//	}
//	else
//	{
//		ptime ptGPS(boost::date_time::c_local_adjustor<ptime>::utc_to_local(from_time_t(m_ulGPSTime)));
//		//���ڲ���
//		date d1 = ptGPS.date();  
//		// ��ȡʱ�䲿��  
//		time_duration td = ptGPS.time_of_day(); 
//		
//		std::stringstream ss;
//		ss.fill('0');
//		ss.width(4);
//		ss <<d1.year();
//
//		ss.width(2);
//		ss<<d1.month().as_number();
//		ss.width(2);
//		ss<<d1.day().as_number()<<",";
//		ss.width(2);
//		ss<<td.hours();
//		ss.width(2);
//		ss<<td.minutes();
//		ss.width(2);
//		ss<<td.seconds()<<",";
//
//		ss<<m_strCityFlag<<","
//			<<m_strGPSID<<",";
//		int prec=numeric_limits<double>::digits10;
//		ss.precision(prec);//����Ĭ�Ͼ���
//		ss<<m_dLongitude<<","
//			<<m_dLatitude<<",";
//
//		ss<<m_dSpeed<<","
//			<<m_dDir<<","
//			<<m_iType<<","
//			<<m_iLoadState<<","
//			<<m_iValid;
//		string strRet;
//		ss>>strRet;
//		strRet.append("\r\n");
//		ss.clear();
//		std::shared_ptr<CBufferObj> buffer = std::shared_ptr<CBufferObj>(new CBufferObj(strRet.c_str(),strRet.length()));
//		return buffer;
//		//ss >> strRet;
//	}
//	return std::shared_ptr<CBufferObj>();
//
//}
string CShenYangData::ToString()
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
		//���ڲ���
		string strgpstime = to_iso_string(ptGPS);
		std::replace( strgpstime.begin(), strgpstime.end(), 'T', ','); 
		string strrvctime = to_iso_extended_string(ptRCV);
		std::replace( strrvctime.begin(), strrvctime.end(), 'T', ' ');
		date d1 = ptGPS.date();  
		// ��ȡʱ�䲿��  
		time_duration td = ptGPS.time_of_day(); 

		std::stringstream ss;
		ss<<strgpstime<<",";
		ss<<m_strCityFlag<<","
			<<m_strGPSID<<",";
		int prec=numeric_limits<double>::digits10;
		ss.precision(prec);//����Ĭ�Ͼ���
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
