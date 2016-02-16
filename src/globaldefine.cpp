//#include "stdafx.h"
#include <globaldeclear.h>
#include <boost/thread/mutex.hpp>
#include <string>
#include <strstream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>  
 //#include <afxwin.h>
#include <boost/asio.hpp>
#include <Windows.h>
using namespace std;
using namespace boost;

#define _WIN32_WINNT 0x0501
string g_strModelKey="";
void SetGlobaleModelKey(string const& modelKey)
{
	g_strModelKey=modelKey;
}
string GetGlobaleModelKey()
{
	return g_strModelKey;
}
/******************************************************************************
功能：string转换为wstring
参数：1.str 要转换的string对象；
返回值：转换后的wstring对象
时间：2011.10.8
作者：EngerWu
********************************************************************************/
wstring MultCharToWideChar(const string& str)
{
	//获取缓冲区的大小，并申请空间，缓冲区大小是按字符计算的
	int len=MultiByteToWideChar(CP_ACP,0,str.c_str(),(int)str.size(),NULL,0);
	TCHAR *buffer=new TCHAR[len+1];
	//多字节编码转换成宽字节编码
	MultiByteToWideChar(CP_ACP,0,str.c_str(),(int)str.size(),buffer,len);
	buffer[len]='\0';//添加字符串结尾
	//删除缓冲区并返回值
	wstring return_value;
	return_value.append(buffer);
	delete []buffer;
	buffer=0;
	return return_value;
}

/******************************************************************************
功能：wstring转换为string
参数：1.str 要转换的wstring对象；
返回值：转换后的string对象
时间：2011.10.8
作者：EngerWu
********************************************************************************/
string WideCharToMultiChar(const wstring& str)
{
	string return_value;
	//获取缓冲区的大小，并申请空间，缓冲区大小是按字节计算的
	int len=WideCharToMultiByte(CP_ACP,0,str.c_str(),(int)str.size(),NULL,0,NULL,NULL);
	char *buffer=new char[len+1];
	WideCharToMultiByte(CP_ACP,0,str.c_str(),(int)str.size(),buffer,len,NULL,NULL);
	buffer[len]='\0';
	//删除缓冲区并返回值
	return_value.append(buffer);
	delete []buffer;
	buffer=0;
	return return_value;
}

bool IsDirectoryExists( const string & path)
{
	return filesystem::exists(filesystem::path(path));
	//fs::path full_path= fs::system_complete( fs::path(path, fs::native ) );
	//return fs::exists(full_path);
}
bool CreatePath(const string& path)
{
	namespace fs = boost::filesystem;
	fs::path full_path( fs::initial_path() );
	full_path = fs::system_complete( fs::path(path, fs::native ) );
	//判断各级子目录是否存在，不存在则需要创建
	if ( !fs::exists( full_path ) )
	{
		// 创建多层子目录
		bool bRet = fs::create_directories(full_path);
		if (false == bRet)
		{
			return false;
		}

	}
	return true;
}
boost::mutex _mutex_log;
void WriteLog(const string & strlog)
{
	boost::mutex::scoped_lock lock(_mutex_log);

	string strpath=GetModelBasePath()+"\\log";
	try
	{
		if (!IsDirectoryExists(strpath))
		{
			CreatePath(strpath);
		}
		string filename;
		if (g_strModelKey.length()==0)
		{
			filename=GetModelName();
		}
		else
		{
			filename=g_strModelKey;
		}
		std::string strTime = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());  
		strpath+="\\"+filename+"_"+strTime.substr(0,8)+".log";
		ofstream ofs(strpath,ios::app);
		ofs<<strTime<<"	";
		ofs<<strlog<<"\n";
		ofs.close();

	}
	catch (std::exception& ex)
	{
	}
}
string GetFormateDataSize(unsigned long long ullsize)
 {
	 stringstream strstrm;
	 if (ullsize<KB)
	 {
		 strstrm<<ullsize<<" B";
	 }
	 else if (ullsize<MB)
	 {
		 strstrm<<(double)ullsize/KB<<" KB";
	 }
	 else if (ullsize<GB)
	 {
		 strstrm<<(double)ullsize/MB<<" MB";
	 }
	 else if (ullsize<TB)
	 {
		  strstrm<<(double)ullsize/GB<<" GB";
	 }
	 else
	 {
		 strstrm<<(double)ullsize/TB<<" TB";
	 }
	 return strstrm.str();
 }
string GetModelBasePath()
{
	char fullname[MAX_PATH]={0};
	GetModuleFileNameA(NULL,fullname,sizeof(fullname));
	for (int i=MAX_PATH-1;i>0;i--)
	{
		if (fullname[i]==L'\\')
		{
			fullname[i]=L'\0';
			break;
		}
	}
	return string(fullname);
}
string GetModelName()
{
	CHAR fullname[MAX_PATH]={0};
	GetModuleFileNameA(NULL,fullname,sizeof(fullname));
	string sztemp(fullname);
	int index1 = sztemp.find_last_of(L'\\');
	int index2 = sztemp.find_last_of(L'.');
	return sztemp.substr(index1+1,index2-index1-1);
}
string GetErrorDetails(DWORD dwErrorCode)
{
	// Get the error code
	string szErrorDetails;
	DWORD dwError = dwErrorCode;//GetDlgItemInt(hwnd, IDC_ERRORCODE, NULL, FALSE);

	HLOCAL hlocal = NULL;   // Buffer that gets the error message string

	// Use the default system locale since we look for Windows messages.
	// Note: this MAKELANGID combination has 0 as value
	DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

	//// Get the error code's textual description
	BOOL fOk = FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_ALLOCATE_BUFFER, 
		NULL, dwError, systemLocale, 
		(PSTR) &hlocal, 0, NULL);

	if (!fOk)
	{
		// Is it a network-related error?
		HMODULE hDll = LoadLibraryExA(("netmsg.dll"), NULL, 
			DONT_RESOLVE_DLL_REFERENCES);

		if (hDll != NULL)
		{
			fOk = FormatMessageA(
				FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS |
				FORMAT_MESSAGE_ALLOCATE_BUFFER,
				hDll, dwError, systemLocale,
				(PSTR) &hlocal, 0, NULL);
			FreeLibrary(hDll);
		}
	}

	if (fOk && (hlocal != NULL)) 
	{
		szErrorDetails = ((PCSTR) LocalLock(hlocal));
		LocalFree(hlocal);
	} 
	else
	{
		szErrorDetails=("没有找到错误码信息");
	}
	return szErrorDetails;
}
bool GetDirFiles(const std::string& sDirName,std::vector<std::string> &vFiles, string strend,bool bSecursive)
{
	using namespace boost::filesystem;
	typedef recursive_directory_iterator  rd_iterator;

	filesystem::path fullpath(sDirName);
	if(!filesystem::exists(fullpath))
	{
		return false;
	}
#if bSecursive
	filesystem::recursive_directory_iterator end_iter;
	for(filesystem::recursive_directory_iterator iter(fullpath);iter!=end_iter;iter++)
#else
	filesystem::directory_iterator end_iter;
	for(filesystem::directory_iterator iter(fullpath);iter!=end_iter;iter++)
#endif
	{
		try
		{
			if (filesystem::is_regular_file(iter->status()))
			{
				std::string sFileName = iter->path().filename().string();
				if (strend.length()>0)
				{
					if (boost::iends_with(sFileName,strend))
					{
						vFiles.push_back(sFileName);
					}					
				}
				else
				{
					vFiles.push_back(sFileName);
				}
			}
		}
		catch ( const std::exception & ex )
		{
			std::stringstream strss;
			strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
				<<" on line "<<__LINE__<<"\n";
			strss<<"# ERR:"<<ex.what();
			WriteLog(strss.str());
			continue;
		}
	}
	return true;
}

int PostHttpData(const string& host, const string& port, const string& page, const char* data,int data_len, string& reponse_data)
{
	using boost::asio::ip::tcp;

  try
  {
    boost::asio::io_service io_service;
    //如果io_service存在复用的情况
    if(io_service.stopped())
      io_service.reset();

    // 从dns取得域名下的所有ip
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host, port);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    
    // 尝试连接到其中的某个ip直到成功 
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator); 

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "POST " << page << " HTTP/1.0\r\n";
    request_stream << "Host: " << host << ":" << port << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Length: " << data_len << "\r\n";
    request_stream << "Content-Type: application/x-www-form-urlencoded\r\n";
    request_stream << "Connection: close\r\n\r\n";
    request_stream.write(data,data_len);

    // Send the request.
    boost::asio::write(socket, request);
    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

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
      return -2;
    }
    // 如果服务器返回非200都认为有错,不支持301/302等跳转
    if (status_code != 200)
    {
      reponse_data = "Response returned with status code != 200 " ;
      return status_code;
    }

    // 传说中的包头可以读下来了
    std::string header;
    std::vector<string> headers;        
    while (std::getline(response_stream, header) && header != "\r")
      headers.push_back(header);

    // 读取所有剩下的数据作为包体
    boost::system::error_code error;
    while (boost::asio::read(socket, response,
        boost::asio::transfer_at_least(1), error))
    {
	}

    //响应有数据
    if (response.size())
    {
      std::istream response_stream(&response);
      std::istreambuf_iterator<char> eos;
      reponse_data = string(std::istreambuf_iterator<char>(response_stream), eos);                        
    }

    if (error != boost::asio::error::eof)
    {
      reponse_data = error.message();
      return -3;
    }
  }
  catch(std::exception& e)
  {
    reponse_data = e.what();
      return -4;  
  }
  return 0;
}
