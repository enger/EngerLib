#include <BufferObj.h>
#include <globaldeclear.h>
#include <sstream>
#include <iostream>
using namespace std;
CBufferObj::CBufferObj(const char* buffer,int len)
{
	if (len<=0)
	{
		//std::stringstream strss;
		//strss<<"Wrong length of buffer "<<len;
		//WriteLog(strss.str());
		return;
	}
	ptr_buffer = std::shared_ptr<char>(new char[len]);
	memset(&(*ptr_buffer),0,len);
	if (buffer!=0)
	{
		memcpy_s(&(*ptr_buffer),len,buffer,len);
	}
	len_buffer=len;
}


CBufferObj::~CBufferObj(void)
{
}
