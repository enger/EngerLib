#include "MyProInfo.h"
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <globaldeclear.h>
CProInfo::CProInfo(string dllPath,string modelkey)
	:_initiate_model(0)
	,_dllPath(dllPath)
	,_share_memory(NULL)
	,_share_mem_size(KB)
	,_lh_share_memory(NULL)
	,m_bStop(false)
{
	try
	{
		_share_mem_size=SHARE_MEMORY_SIZE;
		hInst=LoadLibraryA(_dllPath.c_str());//¶¯Ì¬¼ÓÔØDll
		if (hInst==0)
		{
			std::stringstream strss;
			strss<<"LoadLibrary("<<dllPath<<") in "<<__FILE__<<"("<<__FUNCTION__<<")"
				<<" on line "<<__LINE__<<"\n";
			strss<<"# ERR:"<<GetLastError();
			WriteLog(strss.str());
			return;
		}
		_initiate_model=(InitiateModelFunc)::GetProcAddress(hInst,"InitiateModel");
		if(_initiate_model)
		{
			//m_prtDataProcesser = std::shared_ptr<CDataTrancer> (new CDataTrancer());
			_initiate_model(m_prtDataProcesser);
			if (modelkey.length()>0)
			{
				m_prtDataProcesser->m_strModelKey=modelkey;
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
CProInfo::~CProInfo(void)
{
	////Exit();
	try
	{
		Stop();

		if (hInst)
		{
			FreeLibrary(hInst);
			//ret=ret;
	// 		CloseHandle(hInst);
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

void CProInfo::Start()
{
	try
	{
		m_bStop=false;
		m_prtDataProcesser->Start();
		if (CreateShareMemery()>0)
		{
			update_pool_prt = std::shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
			update_pool_prt->schedule(boost::bind(&CProInfo::UpdateState,this));
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
void CProInfo::Stop()
{
	try
	{
		if (m_bStop)
		{
			return;
		}
		m_bStop=true;
		unsigned int * uiCMD=(unsigned int*)_share_memory;
		*uiCMD=0;
		m_prtDataProcesser->Stop();
		m_prtDataProcesser=nullptr;
		if (NULL != _share_memory)
		{
			UnmapViewOfFile(_share_memory);  
		}
		if (NULL!=_lh_share_memory)
		{
			CloseHandle(_lh_share_memory);
		}
		update_pool_prt->clear();
		update_pool_prt->wait();
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

int CProInfo::CreateShareMemery()
{  
	_lh_share_memory = CreateFileMappingA(HANDLE(0xFFFFFFFF), NULL, PAGE_READWRITE,  
		0, _share_mem_size, m_prtDataProcesser->m_strModelKey.c_str());  
	if (NULL == _lh_share_memory)  
	{  
		if (ERROR_ALREADY_EXISTS == GetLastError())  
		{
			std::stringstream strss;
			strss<<"share memery block '"<<m_prtDataProcesser->m_strModelKey<<"'Already exists!";
			WriteLog(strss.str());  
		}  
		else  
		{  
			std::stringstream strss;
			strss<<"Create Sheared Memory Failed '"<<m_prtDataProcesser->m_strModelKey<<"',"
				"error="<<GetErrorDetails(GetLastError());
			WriteLog(strss.str());  
		}  
		return -1;
	}  
	_share_memory = (char*)MapViewOfFile(_lh_share_memory, FILE_MAP_WRITE, 0, 0, _share_mem_size);  
	if (NULL == _share_memory)  
	{
		std::stringstream strss;
		strss<<"Map Sheared Memory Failed '"<<m_prtDataProcesser->m_strModelKey<<"',"
			"error="<<GetErrorDetails(GetLastError());
		WriteLog(strss.str());  
		return -1; 
	}
	PROCESS_INFORMATION pi;
	pi.dwProcessId=GetCurrentProcessId();
	pi.dwThreadId=GetCurrentThreadId();
	pi.hProcess=GetCurrentProcess();
	pi.hThread=GetCurrentThread();
	unsigned int * uiCMD=(unsigned int*)_share_memory;
	*uiCMD=1;
	memset(_share_memory+SM_OFFSET_PROCESSINFO,0,SHARE_MEMORY_PROCESSINFO_SIZE);
	memcpy_s(_share_memory+SM_OFFSET_PROCESSINFO,SHARE_MEMORY_PROCESSINFO_SIZE,&pi,sizeof(pi));
	return 1;
}

void CProInfo::UpdateState()
{
	while(!m_bStop)
	{
		string str=m_prtDataProcesser->GetState();
		memset(_share_memory+SM_OFFSET_STATE,0,_share_mem_size-SM_OFFSET_STATE);
		memcpy_s(_share_memory+SM_OFFSET_STATE,_share_mem_size,str.c_str(),min(_share_mem_size-SM_OFFSET_STATE,str.length()));
		Sleep(1000);
	}
}
const char* CProInfo::GetShareMemory() const
{
	return _share_memory;
}

//void CProInfo::Exit()
//{
//	try
//	{
//		_my_exit();
//	}
//	catch (std::exception& e)
//	{
//		std::stringstream strss;
//		strss<<"# ERR: std::exception in "<<__FILE__<<"("<<__FUNCTION__<<")"
//			<<" on line "<<__LINE__<<"\n";
//		strss<<"# ERR:"<<e.what();
//		WriteLog(strss.str());
//	}
//}