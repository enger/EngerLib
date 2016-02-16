//local
#include <interface.h>
#include <globaldeclear.h>
#include <State.h>
#include <NetDataMaker.h>
#include <DataRouter.h>
#include <datarancer.h>
#include <SocketServerUDP.h>
#include <SocketClientTCP.h>
#include <SocketClientTCP.h>
#include <MySqlDriver.h>
#include <Statistic.h>
#include <DataRouterZip.h>
//std
#include <vector>
#include <strstream>
#include <iostream>


using namespace engerlib;

string strModelKey="data_unzip_001";

void InitiateModel(shared_ptr<CDataProcesser> & dataprocesser)
{
	SetGlobaleModelKey(strModelKey);
	shared_ptr<CDataTrancer> datatrancer =  shared_ptr<CDataTrancer>(new CDataTrancer());
	shared_ptr<CDataRouterZip> datarouter=shared_ptr<CDataRouterZip>(new CDataRouterZip());
	datatrancer->m_rcvDataRouter=datarouter;
	datatrancer->m_strModelKey=strModelKey;
	dataprocesser=datatrancer;
}
void GetModelKey(string & modelkey)
{
	SetGlobaleModelKey(strModelKey);
	modelkey=GetGlobaleModelKey();//"stddata_biaozhun_001";
}