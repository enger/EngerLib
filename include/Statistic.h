#pragma once
#include <config.h>
#include <Data.h>
#include <string>
#include <MySqlDriver.h>
#include <vector>
#include <map>
using namespace std;
class ENGERLIB_EXP CStatistic
{
public:
	CStatistic(void);
	~CStatistic(void);
private:
	string	m_strTimeID;
	int		m_iAvgDelay;
	int		m_iTotalPointCount;
	int		m_iTotalIDCount;
	int		m_iLessThan30sCount;
	int		m_iLessThan60sCount;
	int		m_iLessThan120sCount;
	int		m_iLessThan180sCount;
	int		m_iLessThan240sCount;
	int		m_iLessThan300sCount;
	int		m_iMoreThan300sCount;
	int		m_iInvalidGPSCount;

	boost::mutex mutexData;
	std::map<string,vector<std::shared_ptr<CData>>>	m_mapGPSDatas;
	shared_ptr<CMySqlDriver> sqldriver;

private:
	void Reset();
	int	WriteToDataBase();
	void StatisticsData();
public:
	string m_strModelKey;
public:
	void AddData(const shared_ptr<CData>& ptr_data);
};