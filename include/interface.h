#pragma once
#include <config.h>
#include <string>
#include <DataProcesser.h>
#include <memory>
extern "C" ENGERLIB_EXP void InitiateModel(shared_ptr<CDataProcesser> & dataprocesser);
extern "C" ENGERLIB_EXP void GetModelKey(string & modelkey);
//extern "C" ENGERLIB_EXP void Start();
//extern "C" ENGERLIB_EXP void Stop();
//extern "C" ENGERLIB_EXP void Exit();
//extern "C" ENGERLIB_EXP void ReadParams();
//extern "C" ENGERLIB_EXP void GetDataSource(std::string& strRet);
//extern "C" ENGERLIB_EXP void GetDataOut(std::string& strRet);
//extern "C" ENGERLIB_EXP unsigned long long GetRCVDataSize();
//extern "C" ENGERLIB_EXP unsigned long long GetSendDataSize();