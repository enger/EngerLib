#include "NetDataMaker.h"
#include <boost/crc.hpp>
#include <zconf.h>
#include <zlib.h>

//byte CNetDataMaker::_head=(0x23);
//byte CNetDataMaker::_tail=(0x24);
CNetDataMaker::CNetDataMaker(void)
	:_maxcount(100)
{
}
CNetDataMaker::~CNetDataMaker(void)
{
}
std::shared_ptr<CBufferObj> CNetDataMaker::get_data(const vector<shared_ptr<CData>> & vecoridata)
{
	if (vecoridata.size()==0)
	{
		return nullptr;
	}	
	string strdata="";
	for (size_t idx=0;idx<vecoridata.size();idx++)
	{
		strdata.append(vecoridata[idx]->ToString());
	}
	shared_ptr<char> compressbuf = shared_ptr<char>(new char[strdata.length()]);
	size_t bufLen=strdata.length();
	memset(&*compressbuf,0,strdata.length());
	size_t orilen=strdata.length();
	int comRet = compress((Bytef*)(&*compressbuf),(uLongf*)&bufLen,(Bytef*)strdata.c_str(),bufLen);
	if (Z_OK==comRet)
	{
		//ͷ��ԭʼ���ݳ��ȣ�ѹ�����ݳ��ȣ�ѹ������,У��,β
		size_t buffe_size = (sizeof(_head)+sizeof(size_t)*2+bufLen+sizeof(crc32type)+sizeof(_tail));
		std::shared_ptr<CBufferObj> ret_buffer=std::shared_ptr<CBufferObj>(new CBufferObj(0,buffe_size));
		size_t retidx=0;

		//ͷ
		memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
			ret_buffer->buff_length()-retidx,
			&_head,sizeof(_head));
		retidx+=sizeof(_head);
		//ԭʼ���ݳ���
		memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
			ret_buffer->buff_length()-retidx,
			(&orilen),sizeof(orilen));
		retidx+=sizeof(orilen);
		//ѹ�����ݳ���
		memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
			ret_buffer->buff_length()-retidx,
			(&bufLen),sizeof(bufLen));
		retidx+=sizeof(bufLen);
		////ѹ������
		memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
			ret_buffer->buff_length()-retidx,
			(&*compressbuf),bufLen);
		retidx+=bufLen;
		//У��
		boost::crc_32_type crc32;

		crc32.process_bytes(&*(ret_buffer->ptr_buffer),retidx);    
		crc32type boostcrc32 = crc32();
		memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
			ret_buffer->buff_length()-retidx,
			&boostcrc32,sizeof(boostcrc32));
		retidx+=sizeof(boostcrc32);
		//β
		memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
			ret_buffer->buff_length()-retidx,
			&_tail,sizeof(_tail));
		retidx+=sizeof(_tail);
		return ret_buffer;
		//size_t retidx=0;
		//size_t pageheadidx=0;
		//boost::crc_32_type crc32;
		//for (size_t idx=0;idx<vectemp.size();idx+=_maxcount)
		//{
		//	size_t nowcnt=min(vectemp.size()-idx,_maxcount);
		//	pageheadidx=retidx;
		//	//ͷ
		//	//����
		//	memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
		//		ret_buffer->buff_length()-retidx,
		//		&nowcnt,sizeof(nowcnt));
		//	retidx+=sizeof(nowcnt);
		//	//����
		//	for (size_t i=0;i<nowcnt;i++)
		//	{
		//		memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
		//			ret_buffer->buff_length()-retidx,
		//			&*(vectemp[idx+i]->ptr_buffer),vectemp[idx+i]->buff_length());
		//		retidx+=vectemp[idx+i]->buff_length();
		//	}
		//	//У��
		//	crc32.process_bytes(&*(ret_buffer->ptr_buffer)+pageheadidx,retidx-pageheadidx);    
		//	crc32type boostcrc32 = crc32();
		//	memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
		//		ret_buffer->buff_length()-retidx,
		//		&boostcrc32,sizeof(boostcrc32));
		//	retidx+=sizeof(boostcrc32);
		//	//β
		//	memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
		//		ret_buffer->buff_length()-retidx,
		//		&_tail,sizeof(_tail));
		//	retidx+=sizeof(_head);
		//}

	}
	//vector<shared_ptr<CBufferObj>> vectemp;
	//size_t realdatasize=0;
	//for (size_t idx=0;idx<vecoridata.size();idx++)
	//{
	//	shared_ptr<CBufferObj>& tempbuffer =vecoridata[idx]->GetFormateData();
	//	if (tempbuffer)
	//	{
	//		vectemp.push_back(tempbuffer);
	//		realdatasize+=tempbuffer->buff_length();
	//	}
	//}
	//if (realdatasize==0)
	//{
	//	return nullptr;
	//}
	//size_t onegpssize=100;
	//size_t pagecnt=vecoridata.size()/(_maxcount);
	//size_t lastpagecnt = vecoridata.size()%(_maxcount);
	//if (lastpagecnt>0)
	//{
	//	pagecnt++;
	//}
	////ͷ�����ݳ��ȣ�У��,β
	//size_t buffe_size = 
	//	pagecnt*(sizeof(_head)+sizeof(size_t)+sizeof(crc32type)+sizeof(_tail))
	//	+realdatasize;//ʵ����������С

	//std::shared_ptr<CBufferObj> ret_buffer=std::shared_ptr<CBufferObj>(new CBufferObj(0,buffe_size));
	//size_t retidx=0;
	//size_t pageheadidx=0;
	//boost::crc_32_type crc32;
	//for (size_t idx=0;idx<vectemp.size();idx+=_maxcount)
	//{
	//	size_t nowcnt=min(vectemp.size()-idx,_maxcount);
	//	pageheadidx=retidx;
	//	//ͷ
	//	memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
	//		ret_buffer->buff_length()-retidx,
	//		&_head,sizeof(_head));
	//	retidx+=sizeof(_head);
	//	//����
	//	memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
	//		ret_buffer->buff_length()-retidx,
	//		&nowcnt,sizeof(nowcnt));
	//	retidx+=sizeof(nowcnt);
	//	//����
	//	for (size_t i=0;i<nowcnt;i++)
	//	{
	//		memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
	//			ret_buffer->buff_length()-retidx,
	//			&*(vectemp[idx+i]->ptr_buffer),vectemp[idx+i]->buff_length());
	//		retidx+=vectemp[idx+i]->buff_length();
	//	}
	//	//У��
	//	crc32.process_bytes(&*(ret_buffer->ptr_buffer)+pageheadidx,retidx-pageheadidx);    
	//	crc32type boostcrc32 = crc32();
	//	memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
	//		ret_buffer->buff_length()-retidx,
	//		&boostcrc32,sizeof(boostcrc32));
	//	retidx+=sizeof(boostcrc32);
	//	//β
	//	memcpy_s(&*(ret_buffer->ptr_buffer)+retidx,
	//		ret_buffer->buff_length()-retidx,
	//		&_tail,sizeof(_tail));
	//	retidx+=sizeof(_head);
	//}
	return nullptr;
}
