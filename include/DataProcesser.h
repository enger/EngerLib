#pragma once
#include <config.h>
#include <string>

using namespace std;
class ENGERLIB_EXP CDataProcesser
{
public:
	CDataProcesser(void);
	virtual ~CDataProcesser(void);
public:
	string m_strModelKey;
public:
	virtual void Start()=0;
	virtual void Stop()=0;
	virtual void Exit()=0;
	virtual void ReadParams()=0;
	virtual string GetState()=0;

};

