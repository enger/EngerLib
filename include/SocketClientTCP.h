#ifndef SOKETCLIENT_HPP
#define SOKETCLIENT_HPP
#include "config.h"
#include <vector>
#include <string>
#include "BufferObj.h"
#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>
#include <DataRouter.h>
#include <Socket.h>
#include <winsock2.h>

using namespace std;

#define BUFFER_SIZE_RECIVE 102400
class ENGERLIB_EXP CSocketClientTCP:public CSocket
{
private:
	sockaddr_in m_ServerAddr;
	//string		m_strServerAddr;
	//u_short		m_usServerPort;
	bool		m_bStop;
	std::shared_ptr<boost::threadpool::pool> recive_pool_prt;
	std::shared_ptr<boost::threadpool::pool> send_pool_prt;
	string		m_strName;
	bool		m_bValid;
	SOCKET		m_Socket;

public:
	CSocketClientTCP(void);
	~CSocketClientTCP(void);
private:
	int Close();
	int Initiate();
	int ReciveMsg();
public:
	//void SetAddrs(string serverAddr){m_strServerAddr=serverAddr;}
	void SetName(string name){m_strName=name;}
	void SetValid(bool valid){m_bValid=valid;}
	//void SetPorts(u_short serverport){m_usServerPort=serverport;}
	//启动方式1=接收，2=发送，3=接收和发送
	virtual int Stop();
	int ConnectToServer();
	void SendData();
public:
	virtual int Start(SocketMode mode);
};
#endif