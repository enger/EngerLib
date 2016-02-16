#include "ShenYangData.h"
#include <globaldeclear.h>
#include <interface.h>
#include <State.h>
#include <DataRouter.h>
//std
#include <vector>
#include <sstream>
#include <iostream>
//local
#include <SocketServerUDP.h>
#include <SocketClientTCP.h>
#include <SocketClientTCP.h>
#include <MySqlDriver.h>
#include <datarancer.h>
#include <Statistic.h>
#include <NetDataMaker.h>
using namespace engerlib;

string strModelKey="anshan_210300_001";
class CDataRouterSY2Std:public CDataRouter
{
public:
	CDataRouterSY2Std(void){};
	~CDataRouterSY2Std(void){};
protected:
	CStatistic m_statistic;
public:
	void SetModelKey(string strmodelkey){m_statistic.m_strModelKey=strmodelkey;}
protected:
	virtual void Apply();
};
void CDataRouterSY2Std::Apply()
{
	shared_ptr<CBufferObj> prt_current_pbuffer=nullptr;
	CNetDataMaker net_data_maker;

	size_t headidx=0;
	while(!bStop)
	{
		shared_ptr<CBufferObj> prt_inbuff=NextInBuffer();
		if (nullptr==prt_inbuff)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(20));
			continue;
		}
		if (prt_inbuff->buff_length()==0)
		{
			continue;
		}
		if (nullptr==prt_current_pbuffer)
		{
			prt_current_pbuffer=prt_inbuff;
		}
		else
		{
			//拷贝
			size_t newlen=prt_current_pbuffer->buff_length()+prt_inbuff->buff_length();
			shared_ptr<CBufferObj> prt_new_buff = shared_ptr<CBufferObj>(new CBufferObj(0,newlen));
			memcpy_s(&*(prt_new_buff->ptr_buffer),newlen,&*(prt_current_pbuffer->ptr_buffer),prt_current_pbuffer->buff_length());
			memcpy_s(&*(prt_new_buff->ptr_buffer)+prt_current_pbuffer->buff_length(),
				newlen-prt_current_pbuffer->buff_length(),
				&*(prt_inbuff->ptr_buffer),prt_inbuff->buff_length());
			prt_current_pbuffer=prt_new_buff;
		}
		string strData;
		//找最后一个换行符
		int idx=prt_current_pbuffer->buff_length()-1;
		char* pbuf = &*prt_current_pbuffer->ptr_buffer;
		while(idx>0)
		{
			if (pbuf[idx]=='\r')
			{
				break;
			}
			idx--;
		}
		//找到换行符
		if (idx>0)
		{
			strData.append(pbuf,idx);
			if (idx==prt_current_pbuffer->buff_length()-1)
			{
				prt_current_pbuffer=nullptr;
			}
			else
			{
				shared_ptr<CBufferObj> prt_backup_pbuffer=prt_current_pbuffer;
				prt_current_pbuffer = shared_ptr<CBufferObj> \
					(new CBufferObj(&*(prt_backup_pbuffer->ptr_buffer)+idx+1,prt_backup_pbuffer->buff_length()-idx-1));
			}
		}
		else//未找到换行符
		{
			continue;
		}
		stringstream ss_stream(strData);
		string stemp;
		//string strResult;
		vector<shared_ptr<CData>> vecdata;
		while (getline(ss_stream,stemp))
		{
			//CShenYangData sydata(stemp);
			//strResult.append(sydata.ToString());
			shared_ptr<CShenYangData> sydata = shared_ptr<CShenYangData>(new CShenYangData(stemp));
			if (sydata->m_strGPSID.length()>0)
			{
				//添加统计数据
				m_statistic.AddData(sydata);
				vecdata.push_back(sydata);
				//strResult.append(sydata->ToString());
			}
		}
		shared_ptr<CBufferObj> buffer = net_data_maker.get_data(vecdata);
		if (buffer!=nullptr && buffer->buff_length()>0)
		{
			PushBackOutBuffer(buffer);
		}
		//if (strResult.length()>0)
		//{
		//	std::shared_ptr<CBufferObj> retBuffer = std::shared_ptr<CBufferObj>(new CBufferObj(strResult.c_str(),strResult.length()));
		//	PushBackOutBuffer(retBuffer);
		//}
	}
}

void InitiateModel(shared_ptr<CDataProcesser> & dataprocesser)
{
	SetGlobaleModelKey(strModelKey);
	shared_ptr<CDataTrancer> datatrancer =  shared_ptr<CDataTrancer>(new CDataTrancer());
	shared_ptr<CDataRouterSY2Std> datarouter =shared_ptr<CDataRouterSY2Std>(new CDataRouterSY2Std());
	datarouter->SetModelKey(strModelKey);
	datatrancer->m_rcvDataRouter=datarouter;
	datatrancer->m_strModelKey=strModelKey;
	dataprocesser=datatrancer;
}
void GetModelKey(string & modelkey)
{
	SetGlobaleModelKey(strModelKey);
	modelkey=GetGlobaleModelKey();
}