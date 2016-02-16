#include <DataRouter.h>
#include <globaldeclear.h>
using namespace engerlib;
CDataRouter::CDataRouter(void)
	:bStop(false)
	,_in_size(0)
	,_out_size(0)
	,_max_in_size(10*MB)
	,_max_out_size(10*MB)
{
	process_pool_prt=shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
	StartProcess();
}


CDataRouter::~CDataRouter(void)
{
	StopProcess();
}

shared_ptr<CBufferObj> CDataRouter::NextInBuffer()
{
	boost::mutex::scoped_lock lock(_mutex_in_bufer);
	shared_ptr<CBufferObj> buff = nullptr;
	if (_q_in_buffer.size()>0)
	{
		buff = _q_in_buffer.front();
		_in_size-=buff->buff_length();
		_q_in_buffer.pop();
	}
	return buff;
}
shared_ptr<CBufferObj> CDataRouter::NextOutBuffer()
{
	boost::mutex::scoped_lock lock(_mutex_out_bufer);
	shared_ptr<CBufferObj> buff = nullptr;
	if (_q_out_buffer.size()>0)
	{
		buff = _q_out_buffer.front();
		_out_size-=buff->buff_length();
		_q_out_buffer.pop();
	}
	return buff;
}
void CDataRouter::PushBackInBuffer(shared_ptr<CBufferObj>& prtbuffer)
{
	boost::mutex::scoped_lock lock(_mutex_in_bufer);
	_q_in_buffer.push(prtbuffer);
	_in_size+=prtbuffer->buff_length();
	while(_in_size>_max_in_size && !_q_in_buffer.empty())
	{
		_in_size-=_q_in_buffer.front()->buff_length();
		_q_in_buffer.pop();
	}
}
void CDataRouter::PushBackInBuffer(vector<shared_ptr<CBufferObj>>& vecbuffer)
{
	boost::mutex::scoped_lock lock(_mutex_in_bufer);
	vector<shared_ptr<CBufferObj>>::iterator ite = vecbuffer.begin();
	for (;ite!=vecbuffer.end();ite++)
	{
		_q_in_buffer.push(*ite);
		_in_size+=(*ite)->buff_length();
	}
	while(_in_size>_max_in_size && !_q_in_buffer.empty())
	{
		_in_size-=_q_in_buffer.front()->buff_length();
		_q_in_buffer.pop();
	}
}
void CDataRouter::ClearInBuffer()
{
	boost::mutex::scoped_lock lock(_mutex_in_bufer);
	while(!_q_in_buffer.empty())
	{
		_q_in_buffer.pop();
	}
	_in_size=0;
	WriteLog("ClearInBuffer");
}

void CDataRouter::PushBackOutBuffer(shared_ptr<CBufferObj>& prtbuffer)
{
	boost::mutex::scoped_lock lock(_mutex_out_bufer);
	_q_out_buffer.push(prtbuffer);
	_out_size+=prtbuffer->buff_length();
	if (_out_size>_max_out_size)
	{
		unsigned long long oldsize=_out_size;
		while(_out_size>_max_out_size && !_q_out_buffer.empty())
		{
			_out_size-=_q_out_buffer.front()->buff_length();
			_q_out_buffer.pop();
		}
		unsigned long long cleansize=oldsize-_out_size;
		stringstream strss;
		strss<<"数据量超出最大限度："<<GetFormateDataSize(oldsize)
			<<",清理掉："<<GetFormateDataSize(cleansize);
		WriteLog(strss.str());
	}
}
void CDataRouter::PushBackOutBuffer(vector<shared_ptr<CBufferObj>>& vecbuffer)
{
	boost::mutex::scoped_lock lock(_mutex_out_bufer);
	vector<shared_ptr<CBufferObj>>::iterator ite = vecbuffer.begin();
	for (;ite!=vecbuffer.end();ite++)
	{
		_q_out_buffer.push(*ite);
		_out_size+=(*ite)->buff_length();
	}
	if (_out_size>_max_out_size)
	{
		unsigned long long oldsize=_out_size;
		while(_out_size>_max_out_size && !_q_out_buffer.empty())
		{
			_out_size-=_q_out_buffer.front()->buff_length();
			_q_out_buffer.pop();
		}
		unsigned long long cleansize=oldsize-_out_size;
		stringstream strss;
		strss<<"数据量超出最大限度："<<GetFormateDataSize(oldsize)
			<<",清理掉："<<GetFormateDataSize(cleansize);
		WriteLog(strss.str());
	}

}
void CDataRouter::ClearOutBuffer()
{
	boost::mutex::scoped_lock lock(_mutex_out_bufer);
	while(!_q_out_buffer.empty())
	{
		_q_out_buffer.pop();
	}
	_out_size=0;
	WriteLog("ClearOutBuffer");
}

void CDataRouter::StartProcess()
{
	bStop=false;
	process_pool_prt->schedule(boost::bind(&CDataRouter::Apply,this));
}
void CDataRouter::StopProcess()
{
	bStop=true;
	process_pool_prt->clear();
	process_pool_prt->wait();
}

void CDataRouterTransOnly::Apply()
{
	while(!bStop)
	{
		shared_ptr<CBufferObj> prt_inbuff=NextInBuffer();
		if (nullptr==prt_inbuff)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
			continue;
		}
		if (prt_inbuff->buff_length()==0)
		{
			continue;
		}
		PushBackOutBuffer(prt_inbuff);
	}
}