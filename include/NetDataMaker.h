#pragma once
#include <config.h>
#include <Data.h>
#include <BufferObj.h>
#include <memory>
#include <vector>
using namespace std;
	typedef unsigned char byte;
	typedef unsigned int  crc32type;
class ENGERLIB_EXP CNetDataMaker
{
public:
	CNetDataMaker(void);
	~CNetDataMaker(void);
protected:
	size_t	_maxcount;
public:
	 const static byte	_head =(0x23);
	 const static byte	_tail=(0x24);
public:
	std::shared_ptr<CBufferObj> get_data(const vector<shared_ptr<CData>> & vecoridata);
};

