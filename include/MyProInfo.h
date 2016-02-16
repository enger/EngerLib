#pragma once
#include <config.h>
#include <string>
#include <State.h>
#include <datarancer.h>
#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>
#include <DataProcesser.h>
//#include <afxwin.h>
#include <Windows.h>
using namespace std;

#define SHARE_MEMORY_CMD_SIZE 4
#define SHARE_MEMORY_PROCESSINFO_SIZE sizeof(PROCESS_INFORMATION)
#define SHARE_MEMORY_STATE_SIZE KB
#define SHARE_MEMORY_SIZE ((SHARE_MEMORY_CMD_SIZE)+(SHARE_MEMORY_PROCESSINFO_SIZE)+(SHARE_MEMORY_STATE_SIZE))
#define SM_OFFSET_CMD 0
#define SM_OFFSET_PROCESSINFO ((SM_OFFSET_CMD)+(SHARE_MEMORY_CMD_SIZE))
#define SM_OFFSET_STATE ((SM_OFFSET_PROCESSINFO)+(SHARE_MEMORY_PROCESSINFO_SIZE))

class ENGERLIB_EXP CProInfo
{
	typedef void (__cdecl * InitiateModelFunc)(shared_ptr<CDataProcesser> & dataprcesser);//定义函数指针类型
public:
	CProInfo(string dllPath,string modelkey="");
	~CProInfo(void);
private:
	//dll路径
	bool m_bStop;
	string _dllPath;
 	HMODULE hInst;
	InitiateModelFunc _initiate_model;
	HANDLE _lh_share_memory;
	char* _share_memory;
	int				_share_mem_size;
	std::shared_ptr<boost::threadpool::pool> update_pool_prt;
	void UpdateState();
	int CreateShareMemery();
public:
	shared_ptr<CDataProcesser> m_prtDataProcesser;
public:
	void Start();
	void Stop();
	const char* GetShareMemory() const ;
};

