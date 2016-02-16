#ifndef BUFFEROBJ_HPP
#define BUFFEROBJ_HPP
#include <config.h>
#include <memory>

class  ENGERLIB_EXP CBufferObj
{
public:
	CBufferObj(const char* buffer,int len);
	~CBufferObj(void);
private:
	size_t		len_buffer;
public:
	std::shared_ptr<char>	ptr_buffer;
	size_t buff_length(){ return len_buffer;}
	std::shared_ptr<char> get_buffer(){ return ptr_buffer; }
};

#endif