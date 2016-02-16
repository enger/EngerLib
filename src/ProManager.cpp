#include "ProManager.h"
//#include <Windows.h>
#include <globaldeclear.h>
#include <strstream>
#include <sstream>
#include <boost/filesystem.hpp>
string GetModelKey(string dllpath)
{
	string modelkey="";
	try
	{
		typedef void (__cdecl * GetModelKeyFunc)(string & modelkey);//定义函数指针类型

		HMODULE hInst=LoadLibraryA(dllpath.c_str());//动态加载Dll
		if (hInst==0)
		{
			std::stringstream strss;
			strss<<"LoadLibrary("<<dllpath<<") in "<<__FILE__<<"("<<__FUNCTION__<<")"
				<<" on line "<<__LINE__<<"\n";
			strss<<"# ERR:"<<GetErrorDetails(GetLastError());
			WriteLog(strss.str());
		}
		else
		{
			GetModelKeyFunc _get_model_key=(GetModelKeyFunc)::GetProcAddress(hInst,"GetModelKey");
			if(_get_model_key)
			{
				_get_model_key(modelkey);
			}
			if (hInst)
			{
				FreeLibrary(hInst);
			}
			//CloseHandle(hInst);
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
	return modelkey;
}
CProManager::CProManager(void)
{
}
CProManager::~CProManager(void)
{
}
bool CProManager::CreateShellProcess( const string& dllpath)
{
	//一些必备参数设置
	shared_ptr<CProcessProInfo> ppinfo = shared_ptr<CProcessProInfo>(new CProcessProInfo());
	string strModelKey = GetModelKey(dllpath);
	if (strModelKey.length()==0)
	{
		return false;
	}
	ppinfo->modelKey=strModelKey;
	ppinfo->modelPath=dllpath;
	STARTUPINFOA si;
	memset(&si,0,sizeof(STARTUPINFO));//初始化si在内存块中的值（详见memset函数）
	si.cb=sizeof(STARTUPINFO);
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=SW_HIDE;
	string exepath=GetModelBasePath()+("\\datashell.exe");
	string strcmd=exepath + " " + dllpath;
	//shared_ptr<PROCESS_INFORMATION> pi = shared_ptr<PROCESS_INFORMATION>(new PROCESS_INFORMATION());//必备参数设置结束
	if(!CreateProcessA(exepath.c_str(),(char*)strcmd.c_str(),NULL,NULL,FALSE,0,NULL,NULL,&si,&ppinfo->pi))
	{
		std::stringstream strss;
		strss<<"# CreateProcessA Fail,"<<GetErrorDetails(GetLastError());
		WriteLog(strss.str());
	}
	else
	{
		//string strMapName("ShareMemory");                // 内存映射对象名称
		//string strComData("This is common data!");        // 共享内存中的数据
		//LPVOID pBuffer;                                    // 共享内存指针
		//Sleep(1000);
		// 首先试图打开一个命名的内存映射文件对象  
		//HANDLE hMap = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, 0, ppinfo->modelKey.c_str());
		//if (NULL == hMap)
		//{   
		//	std::stringstream strss;
		//	strss<<"# ERR: 打开共享内存失败 ‘"<<ppinfo->modelKey<<"’\n"<<GetErrorDetails(GetLastError());
		//	WriteLog(strss.str());
		//}
		//else
		//{    // 打开成功，映射对象的一个视图，得到指向共享内存的指针，显示出里面的数据
		//	ppinfo->share_memory = (char*)::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		int times=0;
		HANDLE hMap = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, 0, ppinfo->modelKey.c_str());
		while(hMap==NULL && times++<1000)
		{
			hMap = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, 0, ppinfo->modelKey.c_str());
			Sleep(100);
		}
		if (hMap!=NULL)
		{
			CloseHandle(hMap);
			map<string,shared_ptr<CProcessProInfo>>::iterator ite = _mapProcess.find(dllpath);
			if (ite==_mapProcess.end())
			{
				_mapProcess.insert(make_pair(dllpath,ppinfo));
			}
			else
			{
				ite->second=ppinfo;
			}
			return true;
		}
	}
	return false;
}
bool CProManager::AddPro(const string& dllpath,bool startNow)
{
	try
	{
		boost::filesystem::path p(dllpath);
		map<string,shared_ptr<CProcessProInfo>>::iterator ite = _mapProcess.find(p.filename().string());
		if (ite==_mapProcess.end() || ite->second==nullptr)
		{
			return CreateShellProcess(p.filename().string());
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
	return false;
}
bool CProManager::RemovePro(const string& sernum)
{
	try
	{
		//map<string,shared_ptr<CProInfo>>::iterator ite = _mapPro.find(sernum);
		//if (ite!=_mapPro.end())
		//{
		//	ite->second->Stop();
		//	ite=_mapPro.erase(ite);
		//}
	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());
	}
	return false;
}
bool CProManager::ClearPro()
{
	try
	{
		//Stop();
		//_mapPro.clear();
	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());
	}
	return false;
}
bool CProManager::Start()
{
	try
	{
	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());
	}
	return false;
}
bool CProManager::Start(const string& dllpath)
{
	try
	{
		boost::filesystem::path p(dllpath);
		map<string,shared_ptr<CProcessProInfo>>::iterator ite = _mapProcess.find(p.filename().string());
		if (ite==_mapProcess.end() || ite->second==nullptr || !ite->second->isRunning)
		{
			return CreateShellProcess(p.filename().string());
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
	return false;
}
bool CProManager::Stop()
{
	try
	{

	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());
	}
	return false;
}
bool CProManager::Stop(const string& dllpath)
{
	try
	{
		boost::filesystem::path p(dllpath);
		map<string,shared_ptr<CProcessProInfo>>::iterator ite = _mapProcess.find(p.filename().string());
		if (ite==_mapProcess.end() || ite->second==nullptr || ite->second->isRunning)
		{
			HANDLE hMap = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, 0, ite->second->modelKey.c_str());

			if (NULL != hMap)
			{   
				//打开成功，映射对象的一个视图，得到指向共享内存的指针，显示出里面的数据
				ite->second->isRunning=true;
				char*  share_memory = (char*)::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				unsigned int * uicmd=(unsigned int *)share_memory;
				*uicmd=0;
				UnmapViewOfFile(share_memory);
				CloseHandle(hMap);
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
	return false;
}
bool CProManager::Update()
{
	try
	{
		//map<string,shared_ptr<CProInfo>>::iterator ite = _mapPro.begin();
		//for (;ite!=_mapPro.end();ite++)
		//{
		//	ite->second->Update();
		//}
	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());
	}
	return false;
}
bool CProManager::Update(const string& dllname)
{
	try
	{
		//map<string,shared_ptr<CProInfo>>::iterator ite = _mapPro.find(dllname);
		//if (ite!=_mapPro.end())
		//{
		//	ite->second->Update();
		//}
	}
	catch (std::exception& e)
	{
		std::stringstream strss;
		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
			<<" on line "<<__LINE__<<"\n";
		strss<<"# ERR:"<<e.what();
		WriteLog(strss.str());
	}
	return false;
}
void CProManager::GetProcess(map<string,shared_ptr<CProcessProInfo>> & mapProc)
{
	map<string,shared_ptr<CProcessProInfo>>::iterator ite = _mapProcess.begin();
	for (;ite!=_mapProcess.end();ite++)
	{
		mapProc.insert(make_pair(ite->first,ite->second));
	}
}
string CProManager::GetRunningState(const string & dllPath)
{
	string strRet="";
	map<string,shared_ptr<CProcessProInfo>>::iterator ite = _mapProcess.find(dllPath);
	if (ite!=_mapProcess.end() && ite->second!=nullptr)
	{
		HANDLE hMap = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, 0, ite->second->modelKey.c_str());
		if (NULL == hMap)
		{
			ite->second->isRunning=false;
			std::stringstream strss;
			strss<<"# ERR: 打开共享内存失败 ‘"<<ite->second->modelKey<<"’\r\n"<<GetErrorDetails(GetLastError());
			WriteLog(strss.str());
		}
		else
		{   
			//打开成功，映射对象的一个视图，得到指向共享内存的指针，显示出里面的数据
			ite->second->isRunning=true;
			char*  share_memory = (char*)::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			std::stringstream strss;
			strss<<share_memory+SM_OFFSET_STATE;
			strRet=strss.str();
			UnmapViewOfFile(share_memory);
			CloseHandle(hMap);
		}
	}
	return strRet;
}
bool CProManager::Initiate()
{
	string strBasePath=GetModelBasePath();
	vector<string> vecmodels;
	GetDirFiles(strBasePath,vecmodels,".dll");
	for (size_t i=0;i<vecmodels.size();i++)
	{
		try
		{
			string modelkey = GetModelKey(vecmodels[i]);
			if (modelkey.length()>0)
			{
				HANDLE hMap = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, 0, modelkey.c_str());
				if (hMap!=NULL)
				{
					char*  share_memory = (char*)::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
					shared_ptr<CProcessProInfo> proinfo = shared_ptr<CProcessProInfo>(new CProcessProInfo());
					proinfo->modelKey=modelkey;
					proinfo->modelPath=vecmodels[i];
					PROCESS_INFORMATION * pPI = (PROCESS_INFORMATION *)(share_memory+SM_OFFSET_PROCESSINFO);
					proinfo->pi.dwProcessId=pPI->dwProcessId;
					proinfo->pi.dwThreadId=pPI->dwThreadId;
					proinfo->pi.hProcess=pPI->hProcess;
					proinfo->pi.hThread=pPI->hThread;
					UnmapViewOfFile(share_memory);
					_mapProcess.insert(make_pair(vecmodels[i],proinfo));
					CloseHandle(hMap);
				}
				else
				{
					_mapProcess.insert(make_pair(vecmodels[i],shared_ptr<CProcessProInfo>()));
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
	}

	return false;
}
