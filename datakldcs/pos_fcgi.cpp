//上报的位置点结构
#include "pos_fcgi.h"
#include <boost/crc.hpp>
#include "zconf.h"
#include "zlib.h"
#include "globaldeclear.h"
#pragma comment(lib,"zlib.lib")
/*========================
函数功能：将位置数据,按FCGI接口协议封包
参数：
	[in]posData		位置数据
	[in]posNum		位置数据个数
========================*/
int post_demo(CNV_POS_POSITION *posData,int posNum,unsigned char isZip,string datakey,int src,char** out_buf,int* out_len)
{
	int  i=0,result=0,offset=0;
	uLongf	 postDataLen=0, packPostDataLen=0;							//postDataLen:压缩前数据长度,packPostDataLen:压缩后数据长度
	char *positionData=NULL, *packpositionData=NULL;				//positionData:压缩前数据,packpositionData:压缩后数据
	unsigned int checkSum=0,realPosNumLics=0,realPosNumNolics=0;	//checkSum:校验码,realPosNumLics:有车牌位置个数,realPosNumNolics:无车牌位置个数
	CNV_REQUEST_HEAD_FCGI *pFcgiReqHead=NULL;			//接口公共头
	CNV_DEV_HEAD_LICS	  *pDevHeadLics=NULL;			//存在附加信息时设备结构
	CNV_DEV_HEAD_FCGI_NOLICS *pDevHeadNolics=NULL;		//不存在附加信息时设备结构
	CNV_POS_FCGI		  *pFcgiPos	   =NULL;			//位置数据结构
	//组织数据
	postDataLen=posNum*(sizeof(CNV_DEV_HEAD_LICS)+sizeof(CNV_POS_FCGI));	//压缩前数据,按全部存在附加信息开内存,实时修改数据长度
	packPostDataLen = sizeof(CNV_REQUEST_HEAD_FCGI)+postDataLen+datakey.length()+sizeof(checkSum);	//压缩后:公共头+数据+密钥+校验码
	positionData=new char[postDataLen];///*(K_CHAR *)*/(char*)malloc(postDataLen);				//压缩前数据,预开长度>=需要长度
	packpositionData=new char[packPostDataLen];///*(K_CHAR *)*/(char*)malloc(packPostDataLen);		//压缩后数据,预开长度>=需要长度
	memset(positionData,0,postDataLen);
	memset(packpositionData,0,packPostDataLen);
	//公共请求头
	pFcgiReqHead = (CNV_REQUEST_HEAD_FCGI *)packpositionData;
	pFcgiReqHead->Src 	  = src;
	pFcgiReqHead->CSystem = 1;
	pFcgiReqHead->Protocol= 1;
	pFcgiReqHead->Num 	  = posNum;
	pFcgiReqHead->IsZip   = isZip;
	//数据内容：将“设备头+位置数据”依次存储起来,统一压缩
	offset = 0;
	for(i=0;i<posNum;i++)
	{
		if(posData[i].DUID<=0)		//设备ID非正数,过滤掉该位置数据,不再上传
		{
			//LOG_SYS_DEBUG("PosData Duid Error:%d",posData[i].DUID);
			continue;
		}
		if(strlen(posData[i].LicenceNum)<=0)
		{	//不存在车牌号时的设备头
			pDevHeadNolics = (CNV_DEV_HEAD_FCGI_NOLICS *)(positionData+offset);
			pDevHeadNolics->CareType = 3;
			pDevHeadNolics->PosNum   = 1;
			pDevHeadNolics->FCuid[15]= 0;	//车牌标志位
			sprintf(pDevHeadNolics->FCuid,"%d",posData[i].DUID);
			realPosNumNolics ++;
			offset += sizeof(CNV_DEV_HEAD_FCGI_NOLICS);
		}
		else
		{	//存在车牌号时的设备头
			pDevHeadLics = (CNV_DEV_HEAD_LICS *)(positionData+offset);
			pDevHeadLics->CareType = 3;
			pDevHeadLics->PosNum   = 1;
			pDevHeadLics->FCuid[15]= 1;	//车牌标志位
			sprintf(pDevHeadLics->FCuid,"%d",posData[i].DUID);
			memcpy(pDevHeadLics->License,posData[i].LicenceNum,11);	//接口协议：最多复制11个字符
			realPosNumLics ++;
			offset += sizeof(CNV_DEV_HEAD_LICS);
		}
		//位置点
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
	//参数修改：实际传入个数,实际位置数据长度
	pFcgiReqHead->Num = realPosNumLics + realPosNumNolics;
	postDataLen = realPosNumLics*(sizeof(CNV_DEV_HEAD_LICS)+sizeof(CNV_POS_FCGI)) + realPosNumNolics*(sizeof(CNV_DEV_HEAD_FCGI_NOLICS)+sizeof(CNV_POS_FCGI));
	//压缩后位置数据
	if (pFcgiReqHead->IsZip)
	{
		result = compress((Bytef *)(packpositionData+sizeof(CNV_REQUEST_HEAD_FCGI)), 
			(uLongf *)&(packPostDataLen), 
			(Bytef *)positionData, 
			postDataLen);
	//拷贝密钥
		memcpy(packpositionData+sizeof(CNV_REQUEST_HEAD_FCGI)+packPostDataLen,datakey.c_str(),datakey.length());
		packPostDataLen += sizeof(CNV_REQUEST_HEAD_FCGI)+datakey.length();//strlen(gHttpRequestPara.Key);

	}
	else
	{
	//拷贝位置数据
		memcpy(packpositionData+sizeof(CNV_REQUEST_HEAD_FCGI),positionData,postDataLen);
	//拷贝密钥
		memcpy(packpositionData+sizeof(CNV_REQUEST_HEAD_FCGI)+postDataLen,datakey.c_str(),datakey.length());
		packPostDataLen = sizeof(CNV_REQUEST_HEAD_FCGI)+postDataLen+datakey.length();//strlen(gHttpRequestPara.Key);

	}
	//memcpy(packpositionData+sizeof(CNV_REQUEST_HEAD_FCGI)+packPostDataLen,datakey.c_str(),datakey.length());
	//计算crc32的数据长度：压缩后数据长度+公共头+密钥
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
	//拷贝校验码
	memcpy(packpositionData+packPostDataLen,&checkSum,sizeof(checkSum));
	packPostDataLen += sizeof(checkSum);
	//释放压缩前内存
	if (NULL!=positionData)	
	{
		delete []positionData;
		//free(positionData);
		positionData=NULL;
	}
	
	//压缩后数据,使用完后释放......
	*out_buf=packpositionData;
	*out_len=packPostDataLen;
	return 0;
}