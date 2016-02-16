#pragma once
#include <config.h>
#include <string>
#include <DataProcesser.h>
#include <memory>
#include <DataRouter.h>
#include <Socket.h>
#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>

using namespace std;
using namespace engerlib;
class ENGERLIB_EXP CDataSaver:public CDataProcesser
{
public:
	CDataSaver(void);
	~CDataSaver(void);
public:
	string m_strSavePath;
	shared_ptr<CSocket> m_sktRcv;
	shared_ptr<CDataRouter> m_rcvDataRouter;

	bool m_bStop;
	boost::mutex mutexFile;
	std::shared_ptr<boost::threadpool::pool> wirte_file_pool_prt;

protected:
	void WriteDate();
public:
	virtual void Start();
	virtual void Stop();
	virtual void Exit();
	virtual void ReadParams();
	virtual string GetState();
};

