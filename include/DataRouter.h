#pragma once
#include <config.h>
#include <BufferObj.h>
#include <memory>
#include <vector>
#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/threadpool.hpp>
namespace engerlib
{
	using namespace std;
	class ENGERLIB_EXP CDataRouter
	{
	public:
		CDataRouter(void);
		~CDataRouter(void);
	private:
		boost::mutex _mutex_in_bufer;
		boost::mutex _mutex_out_bufer;
		unsigned long long _in_size;
		unsigned long long _out_size;
		queue<shared_ptr<CBufferObj>> _q_in_buffer;
		queue<shared_ptr<CBufferObj>> _q_out_buffer;
		std::shared_ptr<boost::threadpool::pool> process_pool_prt;
	protected:
		bool	bStop;
	public:
		unsigned long long _max_in_size;
		unsigned long long _max_out_size;
	public:
		shared_ptr<CBufferObj> NextInBuffer();
		shared_ptr<CBufferObj> NextOutBuffer();
		unsigned long long GetInBuferSize(){return _in_size;}
		unsigned long long GetOutBuferSize(){return _out_size;}
	protected:
		void PushBackOutBuffer(shared_ptr<CBufferObj>& prtbuffer);
		void PushBackOutBuffer(vector<shared_ptr<CBufferObj>>& vecbuffer);
	public:
		void PushBackInBuffer(shared_ptr<CBufferObj>& prtbuffer);
		void PushBackInBuffer(vector<shared_ptr<CBufferObj>>& vecbuffer);
		void ClearInBuffer();
		void ClearOutBuffer();
		void StartProcess();
		void StopProcess();
	protected:
		virtual void Apply()=0;
	};

	class ENGERLIB_EXP CDataRouterTransOnly:public CDataRouter
	{
	public:
		CDataRouterTransOnly(void){};
		~CDataRouterTransOnly(void){};
	protected:
		virtual void Apply();
	};


}

