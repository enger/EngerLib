//#ifndef SOKETSERVER_HPP
//#define SOKETSERVER_HPP
#pragma once
#include "config.h"
#include <iostream>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>
#include <DataRouter.h>
#include <Socket.h>
#include <winsock2.h>

using namespace std;

class  CSoketAndSend
{
public:
	CSoketAndSend()
	{
		socket_send_pool_prt = std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
		IsValid=true;
	}
public:
	bool IsValid;
	SOCKET sockt;
	sockaddr_in addr;
	std::shared_ptr<boost::threadpool::pool> socket_send_pool_prt;
public:
	void SendMSG(shared_ptr<CBufferObj> buff)
	{
		if (buff==nullptr)
		{
			return;
		}
		socket_send_pool_prt->schedule(boost::bind(&CSoketAndSend::SendMSGThread,this,buff));
	}
	void SendMSGThread(shared_ptr<CBufferObj> buff)
	{
		int ret=send(sockt,&*(buff->ptr_buffer),buff->buff_length(),0);
		if (ret<=0)
		{
			IsValid=false;
			closesocket(sockt);
		}
	}
};


class ENGERLIB_EXP CSocketServerTCP:public CSocket
{
	typedef map<SOCKET,CSoketAndSend> CLIENTSOCKETS;
	typedef CLIENTSOCKETS::iterator CLIENTSOCKETS_ITE;
private:
	SOCKET	m_Socket;
	//u_short	m_Port;
	std::shared_ptr<boost::threadpool::pool> socket_send_pool_prt;
	std::shared_ptr<boost::threadpool::pool> socket_recive_pool_prt;
	std::shared_ptr<boost::threadpool::pool> socket_listen_pool_prt;
	boost::mutex mutexCleints;
	CLIENTSOCKETS clientSockets;
	unsigned	maxConnet;
	bool m_bStop;
	bool	m_bTrans;
	SocketMode m_mode;
public:
	CSocketServerTCP(void);
	~CSocketServerTCP(void);
private:
	int	Initiate();
	int	ServerAccept();
	int	Close();
	int ConnectSocketError(const SOCKET& cleintSocket);
	void Recive(const SOCKET& cleintSocket);
	void Send();
public:
	//void SetServerPort(u_short p){m_Port=p;}
	virtual int Start(SocketMode mode);
	virtual int Stop();
	void GetClients(vector<string>& vecclients);
};
