#include "Statistic.h"
#include <strstream>

CStatistic::CStatistic(void)
	:m_strTimeID("")
	,m_iAvgDelay(0)
	,m_iTotalPointCount(0)
	,m_iTotalIDCount(0)
	,m_iLessThan30sCount(0)
	,m_iLessThan60sCount(0)
	,m_iLessThan120sCount(0)
	,m_iLessThan180sCount(0)
	,m_iLessThan240sCount(0)
	,m_iLessThan300sCount(0)
	,m_iMoreThan300sCount(0)
	,m_iInvalidGPSCount(0)
	,sqldriver(nullptr)
{
	cout<<"CStatistic::CStatistic(void)"<<endl;
	sqldriver = shared_ptr<CMySqlDriver>(new CMySqlDriver());
	sqldriver->server_adds="115.29.53.99:3721";
	sqldriver->usrname="gpsreader";
	sqldriver->password="gahxoqve51enger5233";
	sqldriver->database="gpsmonitor";
}


CStatistic::~CStatistic(void)
{
	//cout<<"CStatistic::~CStatistic(void)"<<endl;
	//m_mapGPSDatas.clear();
}
void CStatistic::Reset()
{
	m_strTimeID="";
	m_iAvgDelay=0;
	m_iTotalPointCount=0;
	m_iTotalIDCount=0;
	m_iLessThan30sCount=0;
	m_iLessThan60sCount=0;
	m_iLessThan120sCount=0;
	m_iLessThan180sCount=0;
	m_iLessThan240sCount=0;
	m_iLessThan300sCount=0;
	m_iMoreThan300sCount=0;
	m_iInvalidGPSCount=0;
}
int	CStatistic::WriteToDataBase()
{
	if (m_strTimeID.length()<=0)
	{
		return 0;
	}
	int ret=0;
	StatisticsData();
	stringstream strss;
	strss<<"insert into gpsstatus_new \
					(modelkey,time,totalpointcount,totalidcount,avgdelay,\
					lessthan30scount,lessthan60scount,lessthan120scount,lessthan180scount,lessthan240scount,\
					lessthan300scount,morethan300scount,invalidgpscount) \
					values (";
	strss<<"'"<<m_strModelKey<<"',"<<m_strTimeID<<","<<m_iTotalPointCount<<","<<m_iTotalIDCount<<","<<m_iAvgDelay<<","
		<<m_iLessThan30sCount<<","<<m_iLessThan60sCount<<","<<m_iLessThan120sCount<<","<<m_iLessThan180sCount<<","<<m_iLessThan240sCount<<","
		<<m_iLessThan300sCount<<","<<m_iMoreThan300sCount<<","<<m_iInvalidGPSCount
		<<")";
	sqldriver->WriteIntoDatabase(strss.str());
	return ret;
}
void CStatistic::StatisticsData()
{
	boost::mutex::scoped_lock lock(mutexData);
	std::map<string,vector<std::shared_ptr<CData>>>::iterator iteid=m_mapGPSDatas.begin();
	m_iTotalIDCount=m_mapGPSDatas.size();
	m_iAvgDelay=0;
	m_iTotalPointCount=0;
	m_iLessThan30sCount=0;
	m_iLessThan60sCount=0;
	m_iLessThan120sCount=0;
	m_iLessThan180sCount=0;
	m_iLessThan240sCount=0;
	m_iLessThan300sCount=0;
	m_iMoreThan300sCount=0;
	m_iInvalidGPSCount=0;
	long long totaldelay=0;
	for (;iteid!=m_mapGPSDatas.end();iteid++)
	{
		const vector<std::shared_ptr<CData>>& vecGpsdatas=iteid->second;
		m_iTotalPointCount+=vecGpsdatas.size();

		vector<std::shared_ptr<CData>>::const_iterator itedata=vecGpsdatas.begin();
		for (;itedata!=vecGpsdatas.end();itedata++)
		{
			if ((*itedata)->IsRealValid())
			{
				long delaysecs=(*itedata)->GetDelay();
				totaldelay+=delaysecs;
				if (delaysecs<30)
				{
					m_iLessThan30sCount++;
				}
				else if (delaysecs>=30 && delaysecs<60)
				{
					m_iLessThan60sCount++;
				}
				else if (delaysecs>=60 && delaysecs<120)
				{
					m_iLessThan120sCount++;
				}
				else if (delaysecs>=120 && delaysecs<180)
				{
					m_iLessThan180sCount++;
				}
				else if (delaysecs>=180 && delaysecs<240)
				{
					m_iLessThan240sCount++;
				}
				else if (delaysecs>=240 && delaysecs<300)
				{
					m_iLessThan300sCount++;
				}
				else //>300
				{
					m_iMoreThan300sCount++;
				}
			}
			else
			{
				m_iInvalidGPSCount++;
			}
		}
		if (m_iTotalPointCount-m_iInvalidGPSCount>0)
		{
			m_iAvgDelay=(int)(totaldelay/(m_iTotalPointCount-m_iInvalidGPSCount));
		}
		iteid->second.clear();
	}
	m_mapGPSDatas.clear();
}
void CStatistic::AddData(const shared_ptr<CData>& ptr_data)
{
	string strCurrentTimeID = ptr_data->GetTimeID();
	if (strCurrentTimeID!=m_strTimeID)
	{
		WriteToDataBase();
		m_strTimeID=strCurrentTimeID;
		cout<<"µ±Ç°TIMEID="<<m_strTimeID<<endl;
	}
	boost::mutex::scoped_lock lock(mutexData);
	std::map<string,vector<std::shared_ptr<CData>>>::iterator ite=m_mapGPSDatas.find(ptr_data->m_strGPSID);
	if (ite!=m_mapGPSDatas.end())
	{
		ite->second.push_back(ptr_data);
	}
	else
	{
		vector<std::shared_ptr<CData>> vecgpsdats;
		vecgpsdats.push_back(ptr_data);
		m_mapGPSDatas.insert(make_pair(ptr_data->m_strGPSID ,vecgpsdats));
	}
}
   