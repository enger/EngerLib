#include "HttpPost.h"
//#include <afxinet.h>
#include <iostream>
#include <Windows.h>
#include <shlwapi.h>
#include <Wininet.h>
#include <globaldeclear.h>

CHttpPost::CHttpPost(void)
{
	m_iSendThreadCount=5;   
	send_pool_prt=shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(m_iSendThreadCount));
	m_io_service=nullptr;
	m_tcpSocket=nullptr;
}


CHttpPost::~CHttpPost(void)
{
}
int CHttpPost::Start(SocketMode mode)
{
	m_bStop=false;
	Initiate();
	ConnectHttp();
	for (int i=0;i<m_iSendThreadCount;i++)
	{
		send_pool_prt->schedule(boost::bind(&CHttpPost::SendData,this));
	}
	return 0;
}
int CHttpPost::Stop()
{
	m_bStop=true;
	send_pool_prt->clear();
	send_pool_prt->wait();
	return 0;
}
int CHttpPost::Initiate()
{
	try
	{
		URL_COMPONENTS uc;//m_param->serveradd.c_str()
		char Scheme[1000];
		char HostName[1000];
		char UserName[1000];
		char Password[1000];
		char UrlPath[1000];
		char ExtraInfo[1000];

		uc.dwStructSize = sizeof(uc);
		uc.lpszScheme = Scheme;
		uc.lpszHostName = HostName;
		uc.lpszUserName = UserName;
		uc.lpszPassword = Password;
		uc.lpszUrlPath = UrlPath;
		uc.lpszExtraInfo = ExtraInfo;

		uc.dwSchemeLength = 1000;
		uc.dwHostNameLength = 1000;
		uc.dwUserNameLength = 1000;
		uc.dwPasswordLength = 1000;
		uc.dwUrlPathLength = 1000;
		uc.dwExtraInfoLength = 1000;

		if (!InternetCrackUrl(m_param->serveradd.c_str(), 0, 0, &uc))
		{
			string logstr=GetErrorDetails(GetLastError());
			WriteLog(logstr);
		}
		m_strHost = uc.lpszHostName;
		m_strPage = uc.lpszUrlPath;
		stringstream strss;
		strss<<uc.nPort;
		m_strPort=strss.str();
		strss.str("");
		return 0;
	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());

	}
	return -1;
}
int CHttpPost::ConnectHttp()
{
	try
	{
		boost::mutex::scoped_lock lock(mutexsend);
		if (m_io_service==nullptr)
		{
			m_io_service=std::shared_ptr<boost::asio::io_service>(new boost::asio::io_service());
		}

		if(m_io_service->stopped())
			m_io_service->reset();

		// 从dns取得域名下的所有ip
		tcp::resolver resolver(*m_io_service.get());
		tcp::resolver::query query(m_strHost, m_strPort);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		if (m_tcpSocket==nullptr)
		{
			m_tcpSocket=std::shared_ptr<tcp::socket>(new tcp::socket(*m_io_service.get()));
		}

		//// 尝试连接到其中的某个ip直到成功 
		boost::asio::connect(*m_tcpSocket.get(), endpoint_iterator); 
		return 0;
	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());

	}
	return -1;


}
int CHttpPost::PostData(shared_ptr<CBufferObj>& bufobj)
{
	boost::mutex::scoped_lock lock(mutexsend);
	int retcode=0;
	try
	{
		boost::asio::streambuf response;
		string reponse_data="";
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "POST " << m_strPage << " HTTP/1.0\r\n";
		request_stream << "Host: " << m_strHost << ":" << m_strPort << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Content-Length: " << bufobj->buff_length() << "\r\n";
		request_stream << "Content-Type: application/x-www-form-urlencoded\r\n";
		request_stream << "Connection: keep-alive\r\n";
		request_stream << "Keep-Alive: timeout=100\r\n\r\n";
		request_stream.write(bufobj->ptr_buffer.get(),bufobj->buff_length());
		boost::asio::write(*m_tcpSocket.get(), request);
// 		size_t revsize = m_tcpSocket->receive(response.prepare(512));
// 		response.commit(revsize);
		boost::asio::read_until(*m_tcpSocket.get(), response,"\r\n\r\n");
		// Check that response is OK.
		std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			reponse_data = "Invalid response";
			retcode= -2;
		}
		else
		{
			// 如果服务器返回非200都认为有错,不支持301/302等跳转
			if (status_code != 200)
			{
				reponse_data = "Response returned with status code != 200 " ;
				retcode= status_code;
			}
			else
			{
				// 传说中的包头可以读下来了
				std::string header;
				std::vector<string> headers;        
				while (std::getline(response_stream, header) && header != "\r")
					headers.push_back(header);
				////响应有数据
				if (response.size())
				{
					//std::istream response_stream(&response);
					std::istreambuf_iterator<char> eos;
					reponse_data = string(std::istreambuf_iterator<char>(response_stream), eos);                        
				}
			}
		}

			if (retcode!=0)
			{
				std::stringstream strss;
				strss<<"发送数据失败！错误="<<reponse_data<<"\r\n"<<"重新连接";
				WriteLog(strss.str());
			}
			else
			{
				unsigned int retValue = *((unsigned int*)reponse_data.data());
				if (retValue!=0)
				{
					string errorstr="";
					 if (retValue==400)
					{
						errorstr="系统错误";
					}
					else if (retValue==401)
					{
						errorstr="参数内容不合法";
					}
					else if (retValue==402)
					{
						errorstr="数据校验错误";
					}
					else if (retValue==403)
					{
						errorstr="超过允许上传的数量";
					}
					else
					{
						std::stringstream strss;
						strss<<"提交失败,未知错误码="<<retValue;
						errorstr=strss.str();						
					}
					WriteLog(errorstr);
				}
				else
				{
					retcode=0;
					m_ullSentSize+=bufobj->buff_length();
				}
			}

	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		//WriteLog(strss.str());
		retcode=-1;
	}
	return retcode;
}

void CHttpPost::SendData()
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
			std::cout<<"无数据"<<endl;
			Sleep(100);
			continue;
		}
		std::cout<<m_prtDataRouter->GetOutBuferSize()<<"/"<<m_prtDataRouter->GetInBuferSize()<<endl;
		if (buffer->buff_length()>0)
		{
#if 0
 			if (PostData(buffer)!=0)
 			{
 				ConnectHttp();
 				std::cout<<"发送失败重新连接"<<endl;
 				PostData(buffer);
 			}
			
#else
			string strRetdata;
			int ret = PostHttpData(m_strHost,m_strPort,m_strPage,buffer->ptr_buffer.get(),buffer->buff_length(),strRetdata);
			if (ret!=0)
			{
				std::stringstream strss;
				strss<<"发送数据失败！错误="<<strRetdata;
				WriteLog(strss.str());
			}
			else
			{
				unsigned int retValue = *((unsigned int*)strRetdata.data());
				//retValue=0;
				if (retValue!=0)
				{
					string errorstr="";
					if (retValue==400)
					{
						errorstr="系统错误";
					}
					else if (retValue==401)
					{
						errorstr="参数内容不合法";
					}
					else if (retValue==402)
					{
						errorstr="数据校验错误";
					}
					else if (retValue==403)
					{
						errorstr="超过允许上传的数量";
					}
					else
					{
						//strss<<"提交失败,未知错误码="<<retValue;
						//errorstr=strss.str();						
					}
					WriteLog(errorstr);
				}
				else
				{
					m_ullSentSize+=buffer->buff_length();
				}
			}

#endif
		}
	}
	WriteLog("发送数据结束！");
}
