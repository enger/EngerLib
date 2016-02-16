#include <SocketServerTCP.h>
#include <globaldeclear.h>
#include <strstream>
boost::mutex mutexprint_;
CSocketServerTCP::CSocketServerTCP(void)
{
	maxConnet=50;
	m_ullSentSize=0;
	m_ullRecivedSize=0;
	socket_listen_pool_prt = std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
	socket_send_pool_prt = std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
	socket_recive_pool_prt = std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
}
CSocketServerTCP::~CSocketServerTCP(void)
{
	Stop();
	Cleanup();
}
int CSocketServerTCP::Initiate()
{
	Close();
	int ret=Startup();
	if (ret!=0)
	{
		stringstream strss;
		strss<<"返回值：ret="<<ret<<"error="<<GetErrorDetails(WSAGetLastError());

		WriteLog(strss.str());
		return ret;
	}
	//2.创建socket，
	m_Socket = socket(
		AF_INET,           //internetwork: UDP, TCP, etc
		SOCK_STREAM,        //SOCK_STREAM说明是TCP,SOCK_DGRAM是UDP
		IPPROTO_TCP                  //protocol,0
		);

	if (m_Socket == INVALID_SOCKET ) 
	{
		stringstream strss;
		strss<<"Socket创建失败,错误信息!错误信息："<<GetErrorDetails(WSAGetLastError());
		WriteLog(strss.str());
		return -1;
	}

	//3.bind套接字，相当于给本地套接字赋值
	sockaddr_in myaddr; 
	memset(&myaddr,0,sizeof(myaddr));
	myaddr.sin_family=AF_INET;

	myaddr.sin_addr.s_addr =htonl(INADDR_ANY);//inet_addr("192.168.1.198"); ;// //inet_addr("127.0.0.1");
	myaddr.sin_port=htons(m_param->serverport);//m_Port
	ret= ::bind(m_Socket,(sockaddr*)&myaddr,sizeof(myaddr));

	if (ret!=0)
	{
		stringstream strss;
		strss<<"绑定端口失败!错误信息："<<GetErrorDetails(WSAGetLastError());
		WriteLog(strss.str());
		return ret;
	}
	//5.listen 另一端的socket
	if ((ret=listen(m_Socket,5)) == SOCKET_ERROR )
	{
		stringstream strss;
		strss<<"启动监听失败!错误信息："<<GetErrorDetails(WSAGetLastError());
		WriteLog(strss.str());
		closesocket(m_Socket);
		WSACleanup();
	}
	int iTimeOut=60*1000;
	m_bStop=false;
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&iTimeOut, sizeof(iTimeOut));
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&iTimeOut, sizeof(iTimeOut));
	socket_listen_pool_prt->schedule(boost::bind(&CSocketServerTCP::ServerAccept,this));
	return ret;
}
int	CSocketServerTCP::ServerAccept()
{
	int ret=0;
	while (!m_bStop)
	{
		sockaddr_in addrto;            //发往的地址	
		memset(&addrto,0,sizeof(addrto));
		addrto.sin_family=AF_INET;
		addrto.sin_addr.s_addr=inet_addr("127.0.0.1");
		//端口号必须和服务器绑定的端口号一致
		addrto.sin_port=htons(7861);

		int nlen = sizeof(addrto);
		SOCKET clientSocket = accept(m_Socket,(sockaddr*)&addrto,&nlen);
		if (clientSocket == INVALID_SOCKET)
		{
			stringstream strss;
			strss<<"接受客户端无效!错误信息："<<GetErrorDetails(WSAGetLastError());
			WriteLog(strss.str());
			break;
		}
		else
		{
			boost::mutex::scoped_lock lock(mutexCleints);
			unsigned currentConnNumTrans=clientSockets.size();
			if (currentConnNumTrans<maxConnet)
			{
				CSoketAndSend ss;
				ss.addr=addrto;
				ss.sockt=clientSocket;
				clientSockets.insert(make_pair(clientSocket,ss));
				//socket_recive_pool_prt->schedule(boost::bind(&CSocketServerTCP::Recive,this,clientSocket));
			}
			else
			{
				closesocket(clientSocket);
				for (CLIENTSOCKETS_ITE ite=clientSockets.begin();ite!=clientSockets.end();ite++)
				{
					SOCKET clientSocket=ite->first;
					closesocket(clientSocket);
				}
				clientSockets.clear();
			}
		}
	}
	return ret;
}
int CSocketServerTCP::ConnectSocketError(const SOCKET& cleintSocket)
{
	if (clientSockets.size()>0)
	{
		CLIENTSOCKETS_ITE ite = clientSockets.find(cleintSocket);
		if (ite!=clientSockets.end())
		{
			closesocket(cleintSocket);
			clientSockets.erase(ite);
		}
	}
	return 0;
}
void CSocketServerTCP::Recive(const SOCKET& cleintSocket)
{
	size_t busize=10*KB;
	shared_ptr<char> buffer=shared_ptr<char>(new char[busize]);
	int rcvsize=0;

	while(!m_bStop)
	{
		rcvsize = recv(cleintSocket,&*buffer,busize,0);
		if (rcvsize<=0)
		{
			ConnectSocketError(cleintSocket);
			break;
		}
		else
		{
			m_ullRecivedSize+=rcvsize;
			if (m_prtDataRouter!=nullptr)
			{
				m_prtDataRouter->PushBackInBuffer(shared_ptr<CBufferObj>(new CBufferObj(&*buffer,rcvsize)));
			}
		}
	}
}
void CSocketServerTCP::Send()
{
	while(!m_bStop)
	{
		if (m_prtDataRouter==nullptr)
		{
			break;
		}
		shared_ptr<CBufferObj> buff = m_prtDataRouter->NextOutBuffer();
		if (buff==nullptr)
		{
			Sleep(100);
		}
		else
		{
			boost::mutex::scoped_lock lock(mutexCleints);
			CLIENTSOCKETS_ITE ite=clientSockets.begin();
			bool bSent=false;
			while(ite!=clientSockets.end())
			{
				if (ite->second.IsValid)
				{
					ite->second.SendMSG(buff);
					ite++;
					bSent=true;
				}
				else
				{
					ite=clientSockets.erase(ite);
				}
			}
			if (bSent)
			{
				m_ullSentSize+=buff->buff_length();
			}
		}
	}
}
int CSocketServerTCP::Close()
{
	int ret=0;
		boost::mutex::scoped_lock lock(mutexCleints);
		for (CLIENTSOCKETS_ITE ite=clientSockets.begin();ite!=clientSockets.end();ite++)
		{
			SOCKET clientSocket=ite->first;
			closesocket(clientSocket);
			
		}
		clientSockets.clear();
	closesocket(m_Socket);
	return ret;
}
int CSocketServerTCP::Start(SocketMode mode)
{
	int ret=0;
	m_mode=mode;
	if (m_mode==ReciveOnly)
	{
		maxConnet=1;
	}
	if (m_param->serverport<1 || m_param->serverport>=65535)
	{
		return -1;
	}
	m_bStop=false;
	if ((ret=Initiate())!=0)
	{
		return ret;
	}
	if (m_mode==SendOnly)
	{
		socket_send_pool_prt->schedule(boost::bind(&CSocketServerTCP::Send,this));
	}
	return ret;
}
int CSocketServerTCP::Stop()
{
	int ret=0;
	m_bStop=true;
	Close();

	if (socket_listen_pool_prt!=0)
	{
		socket_listen_pool_prt->clear();
	}
	if (socket_send_pool_prt!=0)
	{
		socket_send_pool_prt->clear();
	}
	socket_send_pool_prt->wait();
	socket_listen_pool_prt->wait();
	return ret;
}
void CSocketServerTCP::GetClients(vector<string>& vecclients)
{
	boost::mutex::scoped_lock lock(mutexCleints);
	for (CLIENTSOCKETS_ITE ite=clientSockets.begin();ite!=clientSockets.end();ite++)
	{
		stringstream strss;
		strss.clear();
		strss<<inet_ntoa(ite->second.addr.sin_addr)<<":"<<ntohs(ite->second.addr.sin_port);
		vecclients.push_back(strss.str());
	}
}