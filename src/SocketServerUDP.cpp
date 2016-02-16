#include "SocketServerUDP.h"
#include "globaldeclear.h"
#include <strstream>
#define BUFFER_SIZE_RECIVE 10240
#define MAXBUFFER_SIZE 500

CSocketServerUDP::CSocketServerUDP(void)
{
	m_bStop=false;
	m_ullRecivedSize=0;
	m_prtDataRouter=nullptr;
	recive_pool_prt = std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
}

CSocketServerUDP::~CSocketServerUDP(void)
{
	Stop();
	Cleanup();
}
int CSocketServerUDP::Initiate()
{
	//2.�����׽���
	int ret=Startup();
	//UDP
	m_Socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);//SOCK_STREAM
	if (m_Socket == INVALID_SOCKET ) 
	{
		stringstream strss;
		strss<<"Socket����ʧ��,������Ϣ!������Ϣ��"<<GetErrorDetails(WSAGetLastError());
		WriteLog(strss.str());
		return -1;
	}

	//3.bind�׽��֣��൱�ڸ������׽��ָ�ֵ
	sockaddr_in myaddr; 
	memset(&myaddr,0,sizeof(myaddr));
	myaddr.sin_family=AF_INET;

	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("127.0.0.1");
	myaddr.sin_port=htons(m_param->serverport);//m_usServerPort
	ret= ::bind(m_Socket,(sockaddr*)&myaddr,sizeof(myaddr));

	if (ret!=0)
	{
		stringstream strss;
		strss<<"�󶨶˿�ʧ��!������Ϣ��"<<GetErrorDetails(WSAGetLastError());
		WriteLog(strss.str());
		return ret;
	}

	int nNetTimeout=1000*60;//1�� 
	//����ʱ��
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(nNetTimeout));
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&nNetTimeout, sizeof(nNetTimeout));
	return ret;
}
int CSocketServerUDP::ReciveMsg()
{
	int ret=0;
	int iResult = 0;
	WriteLog("��ʼ��������");
	char buffer[BUFFER_SIZE_RECIVE];
	memset(buffer,0,sizeof(buffer));
	while(true)
	{
		if (m_bStop)
		{
			break;
		}
		iResult = recv(m_Socket,buffer,sizeof(buffer),0);
		if (iResult <= 0)
		{
			if (!m_bStop)
			{
				stringstream strss;
				strss<<"��������ʧ��!������Ϣ��"<<GetErrorDetails(WSAGetLastError());
				WriteLog(strss.str());
			}
			else
			{
				WriteLog("�ֶ�ֹͣ����");
			}
		}
		else
		{
			m_ullRecivedSize+=iResult;
			if (m_prtDataRouter!=nullptr)
			{
				m_prtDataRouter->PushBackInBuffer(
					shared_ptr<CBufferObj>(new CBufferObj(buffer,iResult)));
			}
		}
	}
	if (iResult<=0)
	{
		ret=-1;
	}
	WriteLog("���շ���!");
	return ret;
}
int CSocketServerUDP::Close()
{
	int ret=closesocket(m_Socket);
	return ret;
}
int CSocketServerUDP::Start(SocketMode mode)
{
	int ret=0;
	if (m_param->serverport<1 || m_param->serverport>=65535)
	{
		WriteLog("����д��ȷ�Ľ��շ������˿ں�");
		return -1;
	}
	if ((ret = Initiate())!=0)
	{
		return ret;
	}
	//��ʼ��������
	m_bStop=false;
	recive_pool_prt->schedule(boost::bind(&CSocketServerUDP::ReciveMsg,this));
	return ret;
}
int CSocketServerUDP::Stop(void)
{
	m_bStop=true;
	Close();
	recive_pool_prt->clear();
	recive_pool_prt->wait();

	return 0;
}
