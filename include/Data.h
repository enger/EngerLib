#pragma once
#include <config.h>
#include <memory>
#include <BufferObj.h>
#include <string>

using namespace std;

class ENGERLIB_EXP CData
{
public:
	CData(void);
	virtual ~CData(void);
public:
	unsigned long	m_ulGPSTime;
	string		m_strCityFlag;
	string		m_strGPSID;
	double		m_dLongitude;
	double		m_dLatitude;
	double		m_dSpeed;
	double		m_dDir;
	int			m_iType;
	int			m_iLoadState;
	int			m_iValid;
	unsigned long	m_ulRCVTime;
private:
	size_t get_buffer_size();
public:
	virtual void Parse(const std::shared_ptr<CBufferObj>& buffer)=0;
	virtual string ToString()=0;
	std::shared_ptr<CBufferObj> GetFormateData();
	long GetDelay(){return m_ulRCVTime-m_ulGPSTime;}
	bool IsRealValid(){ return (abs(GetDelay())<24*60*60);}
	string GetTimeID();
};

