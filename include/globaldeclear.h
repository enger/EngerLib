#ifndef GLOBALCLEAR_HPP
#define GLOBALCLEAR_HPP

#include <string>
#include <config.h>
#include <vector>
using namespace std;

typedef unsigned long DWORD;
const unsigned long long KB=1024;
const unsigned long long  MB=((KB)*(KB));
const unsigned long long  GB=((KB)*(MB));
const unsigned long long  TB =((KB)*(GB));
ENGERLIB_EXP void SetGlobaleModelKey(string const& modelKey);
ENGERLIB_EXP string GetGlobaleModelKey();
ENGERLIB_EXP bool IsDirectoryExists(string const& path);
ENGERLIB_EXP string GetModelBasePath();
ENGERLIB_EXP string GetModelName();
ENGERLIB_EXP void WriteLog(const string & strlog);
ENGERLIB_EXP string GetFormateDataSize(unsigned long long ullsize);
ENGERLIB_EXP wstring MultCharToWideChar(const string& str);
ENGERLIB_EXP string WideCharToMultiChar(const wstring& str);
ENGERLIB_EXP bool CreatePath(const string& path);
ENGERLIB_EXP string GetErrorDetails(DWORD dwErrorCode);
ENGERLIB_EXP bool GetDirFiles(const std::string& sDirName,std::vector<std::string> &vFiles, string strend="",bool bSecursive=false);
ENGERLIB_EXP int PostHttpData(const string& host, const string& port, const string& page, const char* data,int data_len, string& reponse_data);

#endif