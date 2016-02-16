#pragma once
#include <config.h>
#include <Data.h>
#include <string>
#include <memory>

using namespace std;
class CStdData:public CData
{
public:
	CStdData(const std::shared_ptr<CBufferObj>& buffer);
	CStdData(const string & strData);
	~CStdData(void);
public:
	virtual void Parse(const std::shared_ptr<CBufferObj>& buffer);
	//virtual std::shared_ptr<CBufferObj> GetFormateData();
	virtual string ToString();
};

