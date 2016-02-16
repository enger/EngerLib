#pragma once
#include "config.h"
#include <vector>
#include <string>
//#include "BufferObj.h"
#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>
//#include <DataRouter.h>
#include <Socket.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class ENGERLIB_EXP CHttpPost:public CSocket
{
public:
	CHttpPost(void);
	~CHttpPost(void);
private:
	std::shared_ptr<boost::threadpool::pool> send_pool_prt;
	int			m_iSendThreadCount;
	string		m_strName;
	bool		m_bValid;
	bool		m_bStop;
	std::shared_ptr<tcp::socket> m_tcpSocket;
	std::shared_ptr<boost::asio::io_service> m_io_service;
	string		m_strHost;
	string		m_strPort;
	string		m_strPage;

	boost::mutex mutexsend;
	//boost::asio::io_service m_io_service;
private:
	int		Initiate();
	int		ConnectHttp();
	void SendData();
	int PostData(shared_ptr<CBufferObj>& bufobj);
public:
	void SetName(string name){m_strName=name;}
	void SetValid(bool valid){m_bValid=valid;}
public:
	virtual int Start(SocketMode mode);
	virtual int Stop();

};

