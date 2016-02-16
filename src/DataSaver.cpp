#include "DataSaver.h"
#include <SocketClientTCP.h>
#include <SocketServerTCP.h>
#include <SocketServerUDP.h>

#include <boost/progress.hpp>  
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp>  
#include <boost/property_tree/xml_parser.hpp>
//#include <boost/property_tree/xml_parser.hpp>  
#include <boost/typeof/typeof.hpp>  
#include <boost/foreach.hpp> 
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

CDataSaver::CDataSaver(void)
	:m_sktRcv(nullptr)
	,m_bStop(false)
{
	wirte_file_pool_prt=std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
}

CDataSaver::~CDataSaver(void)
{
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
	return ret;
}
void CDataSaver::Start()
{
	ReadParams();
	m_bStop=false;
	if (m_sktRcv->m_param->valid)
	{
		m_sktRcv->Start(CSocket::ReciveOnly);
		wirte_file_pool_prt->schedule(boost::bind(&CDataSaver::WriteDate,this));
	}
}
void CDataSaver::Stop()
{
	m_bStop=true;
	if (m_sktRcv)
	{
		m_sktRcv->Stop();
	}
	if (wirte_file_pool_prt)
	{
		wirte_file_pool_prt->clear();
		wirte_file_pool_prt->wait();
	}
}
void CDataSaver::Exit()
{
	Stop();
}
void CDataSaver::ReadParams()
{
// 	ReadDataSource();
// 	ReadDataOut();
}
string CDataSaver::GetState()
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
	std::stringstream ss;  
	boost::property_tree::write_xml(ss, pt);  
	std::string s = ss.str();  
	return s;
}
void CDataSaver::WriteDate()
{

}
