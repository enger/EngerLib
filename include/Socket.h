#pragma once
#include <config.h>
#include <string>
#include <memory>
#include <datarouter.h>
//#include <winsock2.h>

using namespace std;
using namespace engerlib;
class CSocketParam
{
public:
	string modelkey;
	string modename;
	string name;
	string protocol;
	string serveradd;
	unsigned short serverport;
	bool valid;
public:
	string ToStringShort()
	{
		stringstream sst;
		sst<<serveradd<<":"<<serverport;
		return sst.str();
	}
	string ToStringLong()
	{
		stringstream sst;
		sst<<modelkey<<","<<modename<<","<<name<<","
			<<protocol<<","<<serveradd<<":"<<serverport;
		return sst.str();
	}
};
class ENGERLIB_EXP CSocket
{
public:
	enum SocketMode { ReciveOnly,SendOnly,Both};
public:
	CSocket(void);
	virtual ~CSocket(void);
protected:
	int Startup();
	int Cleanup();

public:
	shared_ptr<CSocketParam> m_param;
	unsigned long long m_ullSentSize;
	unsigned long long m_ullRecivedSize;
	shared_ptr<engerlib::CDataRouter> m_prtDataRouter;
	//virtual int Start()=0;
	virtual int Start(SocketMode mode)=0;
	virtual int Stop()=0;
	unsigned long long GetRCVSize(){return m_ullRecivedSize;}
	unsigned long long GetSendSize(){return m_ullSentSize;}

};

