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

#include "pos_fcgi.h"
//std
#include <vector>
#include <strstream>
#include <iostream>


using namespace engerlib;

string strModelKey="data_kld_changsha_001";

class CDataRouterStd2KLD:public CDataRouter
{
public:
	CDataRouterStd2KLD(void)
	{
		//if (add_kld_pool_prt!=nullptr)
		//{
		//	add_kld_pool_prt =shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
		//	m_strKey="52DD38DBE8CB23ACEA0AE1EF080B6E16";
		//	m_iSrc=211;
		//}
	};
	~CDataRouterStd2KLD(void)
	{
		cout<<"~CDataRouterStd2KLD(void)"<<endl;
	};
private:
	boost::mutex mutexKLD;
	vector<shared_ptr<CNV_POS_POSITION>> m_vecKdlData;
	shared_ptr<boost::threadpool::pool> add_kld_pool_prt;
	string m_strKey;
	int m_iSrc;
private:
	void StdData2KLD();
	void AddKLDData(shared_ptr<CNV_POS_POSITION> klddata);
	void GetKLDData(vector<shared_ptr<CNV_POS_POSITION>>& vecOut,size_t num);
protected:
	virtual void Apply();
};
void CDataRouterStd2KLD::AddKLDData(shared_ptr<CNV_POS_POSITION> klddata)
{
	boost::mutex::scoped_lock lock(mutexKLD);
	m_vecKdlData.push_back(klddata);
}
void CDataRouterStd2KLD::GetKLDData(vector<shared_ptr<CNV_POS_POSITION>>& vecOut,size_t iCount)
{
	boost::mutex::scoped_lock lock(mutexKLD);
	if (m_vecKdlData.size()<=0)
	{
		return;
	}
	if (iCount<=0 || m_vecKdlData.size()<=iCount)
	{
		vecOut.insert(vecOut.end(),m_vecKdlData.begin(),m_vecKdlData.end());
		m_vecKdlData.clear();
	}
	else
	{
		vecOut.insert(vecOut.end(),m_vecKdlData.begin(),m_vecKdlData.begin()+iCount);
		m_vecKdlData.erase(m_vecKdlData.begin(),m_vecKdlData.begin()+iCount);
	}
}

void CDataRouterStd2KLD::StdData2KLD()
{
	shared_ptr<CBufferObj> prt_current_pbuffer=nullptr;
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
		string strResult;
		while (getline(ss_stream,stemp))
		{
			shared_ptr<CData> stddata = shared_ptr<CStdData>(new CStdData(stemp));
			if (stddata->m_strGPSID.length()>0)
			{
				shared_ptr<CNV_POS_POSITION> clddata=shared_ptr<CNV_POS_POSITION>(new CNV_POS_POSITION());
				clddata->CareType=stddata->m_iType;
				clddata->DUID=atoi(stddata->m_strGPSID.c_str());
				clddata->KUID=0;
				clddata->lDirection=stddata->m_dDir;
				memset(clddata->LicenceNum,0,sizeof(clddata->LicenceNum));
				clddata->lSpeed=(int)(stddata->m_dSpeed*1000);
				clddata->lTime= stddata->m_ulGPSTime;
				clddata->lX=(int)(stddata->m_dLongitude*3600000);
				clddata->lY=(int)(stddata->m_dLatitude*3600000);
				clddata->lZ=0;
				clddata->Remark=0x05;
				if (stddata->m_iValid==1)
				{
					clddata->Remark|=0x01;
				}

				if (stddata->m_iLoadState==1)
				{
					clddata->Remark|=0x02;
				}
				AddKLDData(clddata);
			}
		}
	}

}
void CDataRouterStd2KLD::Apply()
{
	//if (add_kld_pool_prt!=nullptr)
	{
		add_kld_pool_prt =shared_ptr<boost::threadpool::pool>(new boost::threadpool::pool(1));
		m_strKey="52DD38DBE8CB23ACEA0AE1EF080B6E16";
		m_iSrc=211;
	}

 	add_kld_pool_prt->schedule(boost::bind(&CDataRouterStd2KLD::StdData2KLD,this));
	while(!bStop)
	{
		size_t iOneTimeCount=127;
		vector<shared_ptr<CNV_POS_POSITION>> vecKLDData;
		GetKLDData(vecKLDData,iOneTimeCount);
		int num = vecKLDData.size();
		if (num==0)
		{
			Sleep(2000);
			continue;
		}
		CNV_POS_POSITION* pPosData = new CNV_POS_POSITION[num];
		for (int i=0;i<num;i++)
		{
			memcpy_s(pPosData+i,sizeof(CNV_POS_POSITION),vecKLDData.at(i).get(),sizeof(CNV_POS_POSITION));
		}
		char* packpositionData=NULL;
		int packPostDataLen=0;

		post_demo(pPosData,num,1,m_strKey,m_iSrc,&packpositionData,&packPostDataLen);

		if (packPostDataLen>0)
		{
			shared_ptr<CBufferObj> buffer = shared_ptr<CBufferObj>(new CBufferObj(packpositionData,packPostDataLen));
			PushBackOutBuffer(buffer);
		}
		vecKLDData.clear();
		if (NULL!=pPosData)
		{
			delete[] pPosData;
			pPosData=NULL;
		}
		if (NULL!=packpositionData)	
		{
			delete []packpositionData;
			packpositionData=NULL;
		}
	}
	add_kld_pool_prt->clear();
	add_kld_pool_prt->wait();
}
void InitiateModel(shared_ptr<CDataProcesser> & dataprocesser)
{
	SetGlobaleModelKey(strModelKey);
	shared_ptr<CDataTrancer> datatrancer =  shared_ptr<CDataTrancer>(new CDataTrancer());
	shared_ptr<CDataRouterStd2KLD> datarouter=shared_ptr<CDataRouterStd2KLD>(new CDataRouterStd2KLD());
// 	datarouter->SetModelKey(strModelKey);
	datatrancer->m_rcvDataRouter=datarouter;
	datatrancer->m_strModelKey=strModelKey;
	dataprocesser=datatrancer;
}
void GetModelKey(string & modelkey)
{
	SetGlobaleModelKey(strModelKey);
	modelkey=GetGlobaleModelKey();//"stddata_biaozhun_001";
}