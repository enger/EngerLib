#include "Socket.h"
#include <globaldeclear.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

CSocket::CSocket(void)
	:m_param(nullptr)
	,m_prtDataRouter(nullptr)
	,m_ullSentSize(0)
	,m_ullRecivedSize(0)
{
}


CSocket::~CSocket(void)
{
}
int CSocket::Startup()
{
	int ret=0;
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 0 );	
	ret = WSAStartup( wVersionRequested, &wsaData );
	if ( ret != 0 ) 
	{
		stringstream strss;
		strss<<"Socket2.0初始化失败，退出!错误信息："<<GetErrorDetails(WSAGetLastError());
		WriteLog(strss.str());	
		return ret;
	}	
	if ( LOBYTE( wsaData.wVersion ) != 2 ||	HIBYTE( wsaData.wVersion ) != 0 )
	{
		stringstream strss;
		strss<<"Socket2.0初始化失败，退出!错误信息："<<GetErrorDetails(WSAGetLastError());
		WriteLog(strss.str());		return -1; 
	}
	return ret;
}
int CSocket::Cleanup()
{
	return WSACleanup();
}
