//local
#include <interface.h>
#include <globaldeclear.h>
#include <DataRouter.h>
#include <DataProcesser.h>
#include <DataSaver.h>
//std
#include <vector>
#include <strstream>
#include <iostream>
#include <memory>

using namespace engerlib;
using namespace std;
string strModelKey="stddata_biaozhun_001";

void InitiateModel(shared_ptr<CDataProcesser> & dataprocesser)
{
	SetGlobaleModelKey(strModelKey);
 	shared_ptr<CDataSaver> datasaver = shared_ptr<CDataSaver>(new  CDataSaver());
// 	shared_ptr<CDataRouterTransOnly> datarouter=shared_ptr<CDataRouterTransOnly>(new CDataRouterTransOnly());
// 	datarouter->SetModelKey(strModelKey);
// 	datatrancer->m_rcvDataRouter=datarouter;
// 	datatrancer->m_strModelKey=strModelKey;
	dataprocesser=datasaver;
}
void GetModelKey(string & modelkey)
{
	SetGlobaleModelKey(strModelKey);
	modelkey=GetGlobaleModelKey();//"stddata_biaozhun_001";
}