#pragma once
#include <config.h>
#include <map>
#include <memory>
#include <MyProInfo.h>

using namespace std;
class CProcessProInfo
{
public:
	PROCESS_INFORMATION pi;
	string modelKey;
	string modelPath;
	bool isRunning;
	//char* share_memory;
};
class ENGERLIB_EXP CProManager
{
public:
	CProManager(void);
	~CProManager(void);
private:
	//map<string,shared_ptr<CProInfo>> _mapPro;
	map<string,shared_ptr<CProcessProInfo>> _mapProcess;
	bool CreateShellProcess(const string& dllpath);
public:
	bool AddPro(const string& dllpath,bool startNow=true);
	bool RemovePro(const string& name);
	bool ClearPro();
	bool Start();
	bool Start(const string& dllpath);
	bool Stop();
	bool Stop(const string& dllname);
	bool Update();
	bool Update(const string& dllname);
	 void GetProcess(map<string,shared_ptr<CProcessProInfo>> & mapProc);
	 string GetRunningState(const string & dllPath);
	 bool Initiate();
};

