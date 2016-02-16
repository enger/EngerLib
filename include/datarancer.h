#pragma once
#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>
#include <config.h>
#include <Socket.h>
#include <BufferObj.h>
#include <memory>
#include <vector>
#include <DataRouter.h>
#include <DataProcesser.h>
class CMySqlDriver;
using namespace std;
using namespace engerlib;
class ENGERLIB_EXP CDataTrancer : public CDataProcesser
{
public:
	CDataTrancer(void);
	~CDataTrancer(void);
public:
	shared_ptr<CSocket> m_sktRcv;
	shared_ptr<CDataRouter> m_rcvDataRouter;
	vector<shared_ptr<CSocket>> m_vecSktSend;
	shared_ptr<CMySqlDriver> sqldriver;
	boost::mutex mutexCleints;
	std::shared_ptr<boost::threadpool::pool> send_pool_prt;
private:
	bool m_bStop;
protected:
	void ReadDataSource();
	void ReadDataOut();
	void SendDataThread();
public:
	virtual void Start();
	virtual void Stop();
	virtual void Exit();
	virtual void ReadParams();
	virtual string GetState();
};

