#include "datarancer.h"
#include "globaldeclear.h"
#include "BufferObj.h"
#include <SocketClientTCP.h>
#include <SocketServerTCP.h>
#include <SocketServerUDP.h>
#include <HttpPost.h>
#include <MySqlDriver.h>

#include <boost/progress.hpp>  
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp>  
#include <boost/property_tree/xml_parser.hpp>
//#include <boost/property_tree/xml_parser.hpp>  
#include <boost/typeof/typeof.hpp>  
#include <boost/foreach.hpp> 
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>
using boost::optional;

CDataTrancer::CDataTrancer(void)
	:m_sktRcv(nullptr)
{
	sqldriver = shared_ptr<CMySqlDriver>(new CMySqlDriver());
	sqldriver->server_adds="115.29.53.99:3721";
	sqldriver->usrname="gpsreader";
	sqldriver->password="gahxoqve51enger5233";
	sqldriver->database="gpsmonitor";
	send_pool_prt=std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
}

shared_ptr<CSocket> MakeSocket(string protocol)
{
	shared_ptr<CSocket> ret=nullptr;
	if (protocol=="ServerUDP")
	{
		ret=shared_ptr<CSocketServerUDP>(new CSocketServerUDP());
	}
	else if (protocol=="ServerTCP")
	{
		ret=shared_ptr<CSocketServerTCP>(new CSocketServerTCP());
	}
	else if (protocol=="ClientTCP")
	{
		ret=shared_ptr<CSocketClientTCP>(new CSocketClientTCP());
	}
	else if (protocol=="ClientUDP")
	{
	}
	else if (protocol=="ClientHTTP")
	{
		ret=shared_ptr<CHttpPost>(new CHttpPost());
	}
	return ret;
}
CDataTrancer::~CDataTrancer(void)
{
}
void CDataTrancer::Start()
{
	ReadParams();
	m_bStop=false;
	if (m_sktRcv->m_param->valid)
	{
		m_sktRcv->Start(CSocket::ReciveOnly);
		for (int i=0;i<m_vecSktSend.size();i++)
		{
			if (m_vecSktSend.at(i)->m_param->valid)
			{
				cout<<m_vecSktSend.at(i)->m_param->modelkey<<endl
					<<m_vecSktSend.at(i)->m_param->protocol<<endl
					<<m_vecSktSend.at(i)->m_param->serverport<<endl
					<<i<<endl;
				m_vecSktSend.at(i)->Start(CSocket::SendOnly);
			}
		}
		send_pool_prt->schedule(boost::bind(&CDataTrancer::SendDataThread,this));
	}

	//if (!m_sktRcv)
	//{
	//	g_sktRecive=std::shared_ptr<CSocketServerUDP>(new CSocketServerUDP());
	//	g_sktRecive->m_ptrDataRouter=shared_ptr<CDataRouterSY2Std>(new CDataRouterSY2Std());
	//}
	//g_sktRecive->SetPort(serverport);
	//g_bStop=false;
	//g_sktRecive->Start();

	//send_pool_prt=std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
	//send_pool_prt->schedule(boost::bind(&SendDataThread));
}
void CDataTrancer::Stop()
{
	m_bStop=true;
	if (m_sktRcv)
	{
		m_sktRcv->Stop();
	}
	{
		boost::mutex::scoped_lock lock(mutexCleints);
		vector<std::shared_ptr<CSocket>>::iterator ite= m_vecSktSend.begin();			
		for (;ite!=m_vecSktSend.end();ite++)
		{
			(*ite)->Stop();
			(*ite)=nullptr;
		}
		m_vecSktSend.clear();
	}
	if (send_pool_prt)
	{
		send_pool_prt->clear();
		send_pool_prt->wait();
	}
}
void CDataTrancer::Exit()
{
	Stop();
	//g_sktRecive=0;
	//send_pool_prt=0;
}
//读取配置文件
void CDataTrancer::ReadDataSource()
{
	try
	{
		string strSql="select * from datasource where modelkey='"+m_strModelKey+"'";
		sql::ResultSet* res=sqldriver->GetQueryResult(strSql);
		if (res!=NULL)
		{
			res->beforeFirst();
			while (res->next())
			{
				shared_ptr<CSocketParam> sktparam = shared_ptr<CSocketParam>(new CSocketParam());
				sktparam->modename=res->getString("modelname").c_str();
				sktparam->name=res->getString("datasource").c_str();
				sktparam->protocol=res->getString("protocol").c_str();
				sktparam->serveradd=res->getString("serveradd").c_str();
				sktparam->serverport=res->getInt("serverport");
				sktparam->valid=(bool)res->getInt("valide");
				m_sktRcv=MakeSocket(sktparam->protocol);
				m_sktRcv->m_prtDataRouter=m_rcvDataRouter;
				m_sktRcv->m_param=sktparam;
			}
			delete res;
			res=0;
		}
	}
	catch(sql::SQLException &e)
	{
		std::stringstream strss;
		strss<<"# ERR: SQLException in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what()<<" (MySQL error code:"<<e.getErrorCode()
			<<", SQLState:"<<e.getSQLState()<<")";
		WriteLog(strss.str());
	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());
	}

}
//更新客户端
void CDataTrancer::ReadDataOut()
{
	try
	{
		string strSql="select * from dataout where modelkey='"+m_strModelKey+"'" + " and valid=1"
			+" order by name asc";
		sql::ResultSet* res=sqldriver->GetQueryResult(strSql);
		boost::mutex::scoped_lock lock(mutexCleints);
		m_vecSktSend.clear();
		if (res!=NULL)
		{
			res->beforeFirst();
			while (res->next())
			{
				shared_ptr<CSocketParam> sktparam = shared_ptr<CSocketParam>(new CSocketParam());
				sktparam->modelkey=m_strModelKey;
				sktparam->protocol=res->getString("protocol").c_str();
				sktparam->serveradd=res->getString("address").c_str();
				sktparam->serverport=res->getInt("port");
				sktparam->name=res->getString("name").c_str();
				sktparam->valid = (bool)res->getInt("valid");
				shared_ptr<CSocket> cstcp = MakeSocket(sktparam->protocol);//shared_ptr<CSocketClientTCP>(new CSocketClientTCP());
				cstcp->m_prtDataRouter = shared_ptr<CDataRouterTransOnly>(new CDataRouterTransOnly());
				cstcp->m_param=sktparam;
				m_vecSktSend.push_back(cstcp);
			}
			delete res;
			res=0;
		}
	}
	catch(sql::SQLException &e)
	{
		std::stringstream strss;
		strss<<"# ERR: SQLException in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what()<<" (MySQL error code:"<<e.getErrorCode()
			<<", SQLState:"<<e.getSQLState()<<")";
		WriteLog(strss.str());
	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());
	}
}

void CDataTrancer::ReadParams()
{
	ReadDataSource();
	ReadDataOut();
}
void CDataTrancer::SendDataThread()
{
	while(!m_bStop)
	{
		std::shared_ptr<CBufferObj> retBuffer = m_sktRcv->m_prtDataRouter->NextOutBuffer();
		if (retBuffer==nullptr)
		{
			Sleep(100);
			continue;
		}
		if (!m_bStop)
		{
			boost::mutex::scoped_lock lock(mutexCleints);
			vector<std::shared_ptr<CSocket>>::iterator ite= m_vecSktSend.begin();			
			for (;ite!=m_vecSktSend.end();ite++)
			{
				(*ite)->m_prtDataRouter->PushBackInBuffer(retBuffer);
			}	
		}
	}
}

string CDataTrancer::GetState()
{
	using boost::property_tree::ptree;

	boost::property_tree::ptree pt;
	boost::property_tree::ptree& pt_root=pt.push_back(ptree::value_type("Model", ptree() ))->second;
	boost::property_tree::ptree& pt_rcv=pt_root.push_back(ptree::value_type("Recive", ptree() ))->second;
	boost::property_tree::ptree& pt_send=pt_root.push_back(ptree::value_type("Send", ptree() ))->second;
	pt_root.put("<xmlattr>.ModelKey",m_strModelKey);
	//recive
	if (true)
	{
		const shared_ptr<CSocketParam>& rcvsktparam= m_sktRcv->m_param;
		stringstream strsstemp;
		strsstemp<<rcvsktparam->serveradd<<":"<<rcvsktparam->serverport;
		pt_rcv.put("<xmlattr>.Address",strsstemp.str());
		pt_rcv.put("<xmlattr>.Name",rcvsktparam->name);
		pt_rcv.add("Protocol",rcvsktparam->protocol);
		pt_rcv.add("RcvSize",m_sktRcv->m_ullRecivedSize);
		if (rcvsktparam->protocol=="ServerTCP")
		{
			boost::property_tree::ptree& pt_rcv_cleint=pt_rcv.push_back(ptree::value_type("Clients", ptree()))->second; 
			vector<string> vecclients;
			shared_ptr<CSocketServerTCP> skServerTCP = dynamic_pointer_cast<CSocketServerTCP>(m_sktRcv);
			skServerTCP->GetClients(vecclients);
			for (size_t i=0;i<vecclients.size();i++)
			{
				pt_rcv_cleint.add("ReciveClient",vecclients[i]);//;
			}
		}
	}
	//send
	for (size_t i=0;i<m_vecSktSend.size();i++)
	{
		boost::property_tree::ptree& pt_send_child=pt_send.push_back(ptree::value_type("SendSockets", ptree() ))->second;
		const shared_ptr<CSocketParam>& sendsktparam= m_vecSktSend[i]->m_param;
		stringstream strsstemp;
		strsstemp<<sendsktparam->serveradd<<":"<<sendsktparam->serverport;
		pt_send_child.put("<xmlattr>.Address",strsstemp.str());
		pt_send_child.put("<xmlattr>.Name",sendsktparam->name);
		pt_send_child.add("Protocol",sendsktparam->protocol);
		pt_send_child.add("SendSize",m_vecSktSend[i]->m_ullSentSize);
		if (sendsktparam->protocol=="ServerTCP")
		{
			boost::property_tree::ptree& pt_send_cleint=pt_send_child.push_back(ptree::value_type("Clients", ptree()))->second; 
			vector<string> vecclients;
			shared_ptr<CSocketServerTCP> skServerTCP = dynamic_pointer_cast<CSocketServerTCP>(m_vecSktSend[i]);
			skServerTCP->GetClients(vecclients);
			for (size_t i=0;i<vecclients.size();i++)
			{
				pt_send_cleint.add("SendClient",vecclients[i]);
			}
		}

	}
	std::stringstream ss;  
	boost::property_tree::write_xml(ss, pt);  
	std::string s = ss.str();  
	return s;
	 //boost::property_tree::ptree children;  
	 //boost::property_tree::ptree child;  
	   
	//for(size_t i = 0; i < smss.size(); i++)  
	//{  
	//	sms_info sms = smss.at(i); 
	//	child.put(sms_info::INDEX, sms.index());
	//	child.put(sms_info::TO, sms.to()); 
	//	child.put(sms_info::FROM, sms.from()); 
	//	child.put(sms_info::MSG, sms.msg());  
	//	children.push_back(std::make_pair("", child));  
	//}  
	//pt_root.add_child(ROOTNAME, children);  
	//std::stringstream ss;  
	//boost::property_tree::write_json(ss, pt_root);  
	//std::string s = ss.str();  

	//stringstream strstm;
	//strstm<<m_sktRcv->m_ullRecivedSize<<"\r\n";
	//for (size_t i=0;i<m_vecSktSend.size();i++)
	//{
	//	strstm<<m_vecSktSend[i]->m_param->ToStringLong()<<"	"
	//		<<m_vecSktSend[i]->m_ullSentSize<<"\r\n";
	//}
	//return strstm.str();
}
