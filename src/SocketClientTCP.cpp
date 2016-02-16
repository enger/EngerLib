#include <SocketClientTCP.h>
#include <globaldeclear.h>
#include <iostream>

#define MAXBUFFER_SIZE 100
CSocketClientTCP::CSocketClientTCP(void):
m_bStop(false)
{
	m_ullSentSize=0;
	m_ullRecivedSize=0;
	m_prtDataRouter=nullptr;
	recive_pool_prt = std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
	send_pool_prt=std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
}

CSocketClientTCP::~CSocketClientTCP(void)
{
	Stop();
	Cleanup();
}

int CSocketClientTCP::Initiate()
{
	//2.创建套接字
	Close();
	int ret=Startup();
	if (ret==-1)
	{
		return ret;
	}
	m_Socket = socket(AF_INET,SOCK_STREAM,0);
	if (m_Socket == INVALID_SOCKET ) 
	{
		stringstream strss;
		strss<<"Socket创建失败,错误信息!错误信息："<<GetErrorDetails(WSAGetLastError());
		WriteLog(strss.str());
		return -1;
	}

	//3.定义地址
	memset(&m_ServerAddr,0,sizeof(m_ServerAddr));
	m_ServerAddr.sin_family=AF_INET;

	m_ServerAddr.sin_addr.s_addr = inet_addr(m_param->serveradd.c_str()); //m_strServerAddr.c_str() 
	//端口号必须和客户发往的端口号一致
	m_ServerAddr.sin_port=htons(m_param->serverport);//m_usServerPort
	int nNetTimeout=1000*60;//1秒 
	//发送时限
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(nNetTimeout));
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&nNetTimeout, sizeof(nNetTimeout));
	return ret;
}
int CSocketClientTCP::ConnectToServer()
{
	int ret=Initiate();
	if (ret!=0)
	{
		stringstream strss;
		strss<<"Soket初始化失败!错误信息："<<GetErrorDetails(WSAGetLastError());
		WriteLog(strss.str());
		return -1;
	}
	if ((ret = connect(m_Socket,(sockaddr*)&m_ServerAddr,sizeof(m_ServerAddr))) == SOCKET_ERROR)
	{
		stringstream strss;
		strss<<"连接服务器失败!错误信息："<<GetErrorDetails(WSAGetLastError());
		WriteLog(strss.str());
		return ret;
	}
	return ret;
}
int CSocketClientTCP::ReciveMsg()
{	
	char buffer[BUFFER_SIZE_RECIVE];
	memset(buffer,0,sizeof(buffer));
	try
	{
		while(!m_bStop)
		{
			int recvsize = recv(m_Socket,buffer,sizeof(buffer),0);
			if (recvsize <= 0)
			{
				if (m_bStop)
				{
					break;
				}
				stringstream strss;
				strss<<m_strName<<"接收数据失败("
					<<recvsize<<")!错误信息："<<GetErrorDetails(WSAGetLastError());
				WriteLog(strss.str());
				//无数据
				static ULONGLONG ullPreTick=GetTickCount();
				ULONGLONG ullCurrentTick=GetTickCount();
				//10秒后重新发起链接
				while (ullCurrentTick-ullPreTick<1000*10 && !m_bStop)
				{
					if (ullCurrentTick<ullPreTick)
					{
						strss.clear();
						strss<<m_strName<<" 系统时钟溢出，重新生成时钟"
							<<"("<<ullCurrentTick<<"/"<<ullPreTick<<")";
						WriteLog(strss.str());
						break;
					}
					ullCurrentTick=GetTickCount();
					Sleep(100);
				}
				ullPreTick=GetTickCount();
				//连接失败
				if (ConnectToServer()!=0)
				{
					strss.clear();
					strss<<m_strName<<" 重新链接服务器失败:"
						<<GetErrorDetails(GetLastError());
					WriteLog(strss.str());
				}
				else
				{
					strss.clear();
					strss<<m_strName<<" 重新链接服务器成功!开始接收数据";
					WriteLog(strss.str());
				}
			}
			else if (recvsize > 0)
			{
				m_ullRecivedSize+=recvsize;
				if (m_prtDataRouter!=nullptr)
				{
					m_prtDataRouter->PushBackInBuffer(shared_ptr<CBufferObj>(new CBufferObj(buffer,recvsize)));
				}
			}
		}
	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());
	}
	stringstream strss;
	strss<<m_strName<<"socket线程退出！";
	WriteLog(strss.str());
	return 0;
}
int CSocketClientTCP::Close()
{
	int ret=closesocket(m_Socket);
	return ret;
}
int CSocketClientTCP::Start(SocketMode mode)
{
	m_bStop=false;
	ConnectToServer();
	if (mode==ReciveOnly || mode==Both)
	{
		recive_pool_prt->schedule(boost::bind(&CSocketClientTCP::ReciveMsg,this));
	}
	if (mode==SendOnly || mode==Both)
	{
		send_pool_prt->schedule(boost::bind(&CSocketClientTCP::SendData,this));
	}
	return 0;
}
int CSocketClientTCP::Stop()
{
	m_bStop=true;
	Close();	
	m_prtDataRouter->StopProcess();
	recive_pool_prt->clear();
	recive_pool_prt->wait();
	send_pool_prt->clear();
	send_pool_prt->wait();
	return 0;
}
void CSocketClientTCP::SendData()
{
	WriteLog("发送数据启动！");
	while(!m_bStop)
	{
		std::shared_ptr<CBufferObj> buffer =nullptr;
		if (m_prtDataRouter!=nullptr)
		{
			buffer = m_prtDataRouter->NextOutBuffer();
		}
		if (buffer==nullptr)
		{
			Sleep(100);
			continue;
		}
		if (buffer->buff_length()>0)
		{
			int ret = send(m_Socket,&*(buffer->ptr_buffer),buffer->buff_length(),0);
			if (ret<=0)
			{
				WriteLog("发送数据失败！,清理缓冲缓区");
				if (ConnectToServer()!=0)
				{
					m_prtDataRouter->ClearOutBuffer();
					m_prtDataRouter->ClearInBuffer();
				}
			}
			else
			{
				m_ullSentSize+=ret;
			}
		}
	}
	WriteLog("发送数据结束！");

}
