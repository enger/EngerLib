#include "ShenYangData.h"
#include <interface.h>
//std
#include <vector>
#include <strstream>

//local
#include <SocketServerUDP.h>
#include <SocketClientTCP.h>
#include <SocketClientTCP.h>
#include <MySqlDriver.h>

#pragma region 接收连接
/*
// 接收连接，CSocketServerUDP方式，指定本地端口
*/
std::shared_ptr<CSocketServerUDP> g_sktRecive;
u_short port=40013;
#pragma endregion 接收连接

#pragma region 转发连接
/*
// 转发连接，CSocketClientTCP方式，从数据库中读取
*/
vector<std::shared_ptr<CSocketClientTCP>> g_vecClients;

#pragma endregion 转发连接

std::shared_ptr<boost::threadpool::pool> process_pool_prt;

bool g_bStop=false;
//string strServerAdd="127.0.0.1";
//u_short port=40013;

//惟一序列号，用于区分是哪个dll
string SerialNumber="test";


boost::mutex mutexCleints;
void ProcessThread();

//更新客户端
void UpdateClientsToSend()
{
	CMySqlDriver m_mysqlDriver;
	m_mysqlDriver.server_adds="115.29.53.99:3721";
	m_mysqlDriver.usrname="gpsreader";
	m_mysqlDriver.password="gahxoqve51enger5233";
	m_mysqlDriver.database="gpsmonitor";
	m_mysqlDriver.Connect();
	sql::ResultSet* res=m_mysqlDriver.GetQueryResult("select * from cities_send order by name asc");
	boost::mutex::scoped_lock lock(mutexCleints);
	g_vecClients.clear();
	if (res!=NULL)
	{
		res->beforeFirst();
		while (res->next())
		{
			string name=res->getString("name").c_str();
			string ip=res->getString("ip").c_str();
			int p=res->getInt("port");
			shared_ptr<CSocketClientTCP> cstcp = shared_ptr<CSocketClientTCP>(new CSocketClientTCP());
			cstcp->SetName(name);
			cstcp->SetAddrs(ip);
			cstcp->SetPorts(p);
			cstcp->Start(CSocketClientTCP::SendOnly);
			g_vecClients.push_back(cstcp);
			cout<<name<<","<<ip<<","<<p<<endl;
		}
		delete res;
		res=0;
	}

}
void Start()
{
	process_pool_prt=std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
	process_pool_prt->schedule(boost::bind(&ProcessThread));
}
void Stop()
{
	g_sktRecive->Stop();
	g_vecClients.clear();
	g_bStop=true;
}
void GetServer(string & strRet)
{
	stringstream ss;
	//ss <<SerialNumber<<":"<<strServerAdd<<":"<<port;
	ss >> strRet;
}
void ProcessThread()
{
	try
	{
		g_sktRecive=std::shared_ptr<CSocketServerUDP>(new CSocketServerUDP());
		g_sktRecive->SetPort(port);
		g_bStop=false;
		g_sktRecive->Start();
		UpdateClientsToSend();
		while(!g_bStop)
		{
			std::vector<std::shared_ptr<CBufferObj>> vecBuffer;
			g_sktRecive->GetDataBuffer(vecBuffer);
			if (vecBuffer.size()==0)
			{
				Sleep(10);
				continue;
			}
			if (!g_bStop)
			{
				boost::mutex::scoped_lock lock(mutexCleints);
				vector<std::shared_ptr<CSocketClientTCP>>::iterator ite= g_vecClients.begin();			
				string strData;
				for (size_t i=0;i<vecBuffer.size();i++)
				{
					strData.append(&*(vecBuffer[i]->ptr_buffer),vecBuffer[i]->buff_length());
				}
				stringstream ss_stream(strData);
				string stemp;
				string strResult;
				while (getline(ss_stream,stemp))
				{
					CShenYangData sydata(stemp);
					strResult.append(sydata.ToString());
				}
				std::shared_ptr<CBufferObj> retBuffer = std::shared_ptr<CBufferObj>(new CBufferObj(strResult.c_str(),strResult.length()));
				for (;ite!=g_vecClients.end();ite++)
				{
					(*ite)->PushSendData(retBuffer);
				}
			}
		}
	}
	catch (std::exception* e)
	{
		cout<<e->what();
	}

}