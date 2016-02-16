//�ϱ���λ�õ�ṹ
#include "pos_fcgi.h"
#include <boost/crc.hpp>
#include "zconf.h"
#include "zlib.h"
#include "globaldeclear.h"
#pragma comment(lib,"zlib.lib")
/*========================
�������ܣ���λ������,��FCGI�ӿ�Э����
������
	[in]posData		λ������
	[in]posNum		λ�����ݸ���
========================*/
int post_demo(CNV_POS_POSITION *posData,int posNum,unsigned char isZip,string datakey,int src,char** out_buf,int* out_len)
{
	int  i=0,result=0,offset=0;
	uLongf	 postDataLen=0, packPostDataLen=0;							//postDataLen:ѹ��ǰ���ݳ���,packPostDataLen:ѹ�������ݳ���
	char *positionData=NULL, *packpositionData=NULL;				//positionData:ѹ��ǰ����,packpositionData:ѹ��������
	unsigned int checkSum=0,realPosNumLics=0,realPosNumNolics=0;	//checkSum:У����,realPosNumLics:�г���λ�ø���,realPosNumNolics:�޳���λ�ø���
	CNV_REQUEST_HEAD_FCGI *pFcgiReqHead=NULL;			//�ӿڹ���ͷ
	CNV_DEV_HEAD_LICS	  *pDevHeadLics=NULL;			//���ڸ�����Ϣʱ�豸�ṹ
	CNV_DEV_HEAD_FCGI_NOLICS *pDevHeadNolics=NULL;		//�����ڸ�����Ϣʱ�豸�ṹ
	CNV_POS_FCGI		  *pFcgiPos	   =NULL;			//λ�����ݽṹ
	//��֯����
	postDataLen=posNum*(sizeof(CNV_DEV_HEAD_LICS)+sizeof(CNV_POS_FCGI));	//ѹ��ǰ����,��ȫ�����ڸ�����Ϣ���ڴ�,ʵʱ�޸����ݳ���
	packPostDataLen = sizeof(CNV_REQUEST_HEAD_FCGI)+postDataLen+datakey.length()+sizeof(checkSum);	//ѹ����:����ͷ+����+��Կ+У����
	positionData=new char[postDataLen];///*(K_CHAR *)*/(char*)malloc(postDataLen);				//ѹ��ǰ����,Ԥ������>=��Ҫ����
	packpositionData=new char[packPostDataLen];///*(K_CHAR *)*/(char*)malloc(packPostDataLen);		//ѹ��������,Ԥ������>=��Ҫ����
	memset(positionData,0,postDataLen);
	memset(packpositionData,0,packPostDataLen);
	//��������ͷ
	pFcgiReqHead = (CNV_REQUEST_HEAD_FCGI *)packpositionData;
	pFcgiReqHead->Src 	  = src;
	pFcgiReqHead->CSystem = 1;
	pFcgiReqHead->Protocol= 1;
	pFcgiReqHead->Num 	  = posNum;
	pFcgiReqHead->IsZip   = isZip;
	//�������ݣ������豸ͷ+λ�����ݡ����δ洢����,ͳһѹ��
	offset = 0;
	for(i=0;i<posNum;i++)
	{
		if(posData[i].DUID<=0)		//�豸ID������,���˵���λ������,�����ϴ�
		{
			//LOG_SYS_DEBUG("PosData Duid Error:%d",posData[i].DUID);
			continue;
		}
		if(strlen(posData[i].LicenceNum)<=0)
		{	//�����ڳ��ƺ�ʱ���豸ͷ
			pDevHeadNolics = (CNV_DEV_HEAD_FCGI_NOLICS *)(positionData+offset);
			pDevHeadNolics->CareType = 3;
			pDevHeadNolics->PosNum   = 1;
			pDevHeadNolics->FCuid[15]= 0;	//���Ʊ�־λ
			sprintf(pDevHeadNolics->FCuid,"%d",posData[i].DUID);
			realPosNumNolics ++;
			offset += sizeof(CNV_DEV_HEAD_FCGI_NOLICS);
		}
		else
		{	//���ڳ��ƺ�ʱ���豸ͷ
			pDevHeadLics = (CNV_DEV_HEAD_LICS *)(positionData+offset);
			pDevHeadLics->CareType = 3;
			pDevHeadLics->PosNum   = 1;
			pDevHeadLics->FCuid[15]= 1;	//���Ʊ�־λ
			sprintf(pDevHeadLics->FCuid,"%d",posData[i].DUID);
			memcpy(pDevHeadLics->License,posData[i].LicenceNum,11);	//�ӿ�Э�飺��ิ��11���ַ�
			realPosNumLics ++;
			offset += sizeof(CNV_DEV_HEAD_LICS);
		}
		//λ�õ�
		pFcgiPos = (CNV_POS_FCGI *)(positionData+offset);
		pFcgiPos->X			= posData[i].lX;
		pFcgiPos->Y			= posData[i].lY;
		pFcgiPos->Speed		= posData[i].lSpeed;
		pFcgiPos->High		= posData[i].lZ;
		pFcgiPos->Direction = posData[i].lDirection;
		pFcgiPos->Time		= posData[i].lTime;
		pFcgiPos->Remark	= posData[i].Remark;
		offset += sizeof(CNV_POS_FCGI);
	}
	//�����޸ģ�ʵ�ʴ������,ʵ��λ�����ݳ���
	pFcgiReqHead->Num = realPosNumLics + realPosNumNolics;
	postDataLen = realPosNumLics*(sizeof(CNV_DEV_HEAD_LICS)+sizeof(CNV_POS_FCGI)) + realPosNumNolics*(sizeof(CNV_DEV_HEAD_FCGI_NOLICS)+sizeof(CNV_POS_FCGI));
	//ѹ����λ������
	if (pFcgiReqHead->IsZip)
	{
		result = compress((Bytef *)(packpositionData+sizeof(CNV_REQUEST_HEAD_FCGI)), 
			(uLongf *)&(packPostDataLen), 
			(Bytef *)positionData, 
			postDataLen);
	//������Կ
		memcpy(packpositionData+sizeof(CNV_REQUEST_HEAD_FCGI)+packPostDataLen,datakey.c_str(),datakey.length());
		packPostDataLen += sizeof(CNV_REQUEST_HEAD_FCGI)+datakey.length();//strlen(gHttpRequestPara.Key);

	}
	else
	{
	//����λ������
		memcpy(packpositionData+sizeof(CNV_REQUEST_HEAD_FCGI),positionData,postDataLen);
	//������Կ
		memcpy(packpositionData+sizeof(CNV_REQUEST_HEAD_FCGI)+postDataLen,datakey.c_str(),datakey.length());
		packPostDataLen = sizeof(CNV_REQUEST_HEAD_FCGI)+postDataLen+datakey.length();//strlen(gHttpRequestPara.Key);

	}
	//memcpy(packpositionData+sizeof(CNV_REQUEST_HEAD_FCGI)+packPostDataLen,datakey.c_str(),datakey.length());
	//����crc32�����ݳ��ȣ�ѹ�������ݳ���+����ͷ+��Կ
	//packPostDataLen += sizeof(CNV_REQUEST_HEAD_FCGI)+datakey.length();//strlen(gHttpRequestPara.Key);
	boost::crc_32_type crc32;
	crc32.process_bytes(packpositionData, packPostDataLen);    
	//unsigned int icrc32my=GetCrc32(packpositionData, packPostDataLen);
	unsigned int boostcrc32 = crc32();//crc32.checksum();
	//if (icrc32my!=boostcrc32)
	//{
	//	char newbuffer[2048];
	//	memset(newbuffer,0,2048);
	//	memcpy(newbuffer,packpositionData,packPostDataLen);
	//	int rightsum=0;
	//}
	//icrc32my=icrc32my;
	checkSum = boostcrc32;////crc32(0, packpositionData, packPostDataLen);
	packPostDataLen -= datakey.length();//strlen(gHttpRequestPara.Key);
	//����У����
	memcpy(packpositionData+packPostDataLen,&checkSum,sizeof(checkSum));
	packPostDataLen += sizeof(checkSum);
	//�ͷ�ѹ��ǰ�ڴ�
	if (NULL!=positionData)	
	{
		delete []positionData;
		//free(positionData);
		positionData=NULL;
	}
	
	//ѹ��������,ʹ������ͷ�......
	*out_buf=packpositionData;
	*out_len=packPostDataLen;
	return 0;
}