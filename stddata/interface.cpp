//local
#include "StdData.h"
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

//std
#include <vector>
#include <strstream>
#include <iostream>


using namespace engerlib;

string strModelKey="stddata_biaozhun_001";

class CDataRouterStd2NetData:public CDataRouter
{
public:
	CDataRouterStd2NetData(void)
	{
		cout<<"CDataRouterStd2NetData"<<endl;
	};
	~CDataRouterStd2NetData(void)
	{
		cout<<"~CDataRouterStd2NetData"<<endl;
	};
protected:
	CStatistic m_statistic;
public:
	void SetModelKey(string strmodelkey){m_statistic.m_strModelKey=strmodelkey;}
protected:
	virtual void Apply();
};

void CDataRouterStd2NetData::Apply()
{
	shared_ptr<CBufferObj> prt_current_pbuffer=nullptr;
	CNetDataMaker net_data_maker;

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
			shared_ptr<CData> stddata = shared_ptr<CStdData>(new CStdData(stemp));
			if (stddata->m_strGPSID.length()>0)
			{
				//添加统计数据
				m_statistic.AddData(stddata);
				vecdata.push_back(stddata);
			}
			else
			{
				//cout<<stemp<<endl;
			}
		}
		shared_ptr<CBufferObj> buffer = net_data_maker.get_data(vecdata);
		if (buffer!=nullptr && buffer->buff_length()>0)
		{
			PushBackOutBuffer(buffer);
		}
	}
}
void InitiateModel(shared_ptr<CDataProcesser> & dataprocesser)
{
	SetGlobaleModelKey(strModelKey);
	shared_ptr<CDataTrancer> datatrancer =  shared_ptr<CDataTrancer>(new CDataTrancer());
	shared_ptr<CDataRouterStd2NetData> datarouter=shared_ptr<CDataRouterStd2NetData>(new CDataRouterStd2NetData());
	datarouter->SetModelKey(strModelKey);
	datatrancer->m_rcvDataRouter=datarouter;
	datatrancer->m_strModelKey=strModelKey;
	dataprocesser=datatrancer;
	//datatrancer->m_rcvDataRouter=shared_ptr<CDataRouterStd2NetData>(new CDataRouterStd2NetData());
	//datatrancer->m_strModelKey="stddata_biaozhun_001";
}
void GetModelKey(string & modelkey)
{
	SetGlobaleModelKey(strModelKey);
	modelkey=GetGlobaleModelKey();//"stddata_biaozhun_001";
}