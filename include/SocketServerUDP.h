#pragma once
#include <config.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>

#include <BufferObj.h>
#include <datarouter.h>
#include <Socket.h>
#include <winsock2.h>

using namespace std;
using namespace engerlib;
class ENGERLIB_EXP CSocketServerUDP:public CSocket
{
private:
	std::shared_ptr<boost::threadpool::pool> recive_pool_prt;
	SOCKET		m_Socket;
	bool		m_bStop;
public:
	CSocketServerUDP(void);
	~CSocketServerUDP(void);
private:
	int ReciveMsg();
	int Close();
public:
	//void SetPort(u_short serverport){m_usServerPort=serverport;}
	virtual int Start(SocketMode mode);
	virtual int Stop(void);
	int Initiate();
	ULONGLONG GetReciveSize(){ return m_ullRecivedSize; }
};
