#pragma once
#include <Data.h>
#include <string>
#include <memory>

using namespace std;

class  CShenYangData:public CData
{
public:
	CShenYangData(const std::shared_ptr<CBufferObj>& buffer);
	CShenYangData(const std::string & strData);
	~CShenYangData(void);
private:
	static const int			oriFieldsCount=11;
	static const unsigned int	ValidMask=0x01;
	static const unsigned int	LoadMask=0x040000;
private:
	string		m_strASYN;
	string		m_strType;
	double		m_dMileage;
	unsigned int		m_uiAlarmWord;
	unsigned int		m_uiStateWord;
	string		m_strMSG;
public:
	virtual void Parse(const std::shared_ptr<CBufferObj>& buffer);
	//virtual std::shared_ptr<CBufferObj> GetFormateData();
	virtual string ToString();

};

