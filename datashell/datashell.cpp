// datashell.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <MyProInfo.h>
#include <memory>
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	string dllpath(argv[1]);
	CProInfo prt_pro(dllpath);
	prt_pro.Start();
	const unsigned int* uicmd = (const unsigned int*) prt_pro.GetShareMemory();

	while(*uicmd==1)
	{
		Sleep(1000);
	}
	prt_pro.Stop();
	return 0;
}