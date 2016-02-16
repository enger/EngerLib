#include <DataRouterZip.h>
#include <NetDataMaker.h>
#include <boost/crc.hpp>
#include <zconf.h>
#include <zlib.h>

using namespace engerlib;
using namespace std;
CDataRouterZip::CDataRouterZip(void)
{
}


CDataRouterZip::~CDataRouterZip(void)
{
}

void CDataRouterZip::Apply()
{
	shared_ptr<CBufferObj> prt_current_pbuffer=nullptr;

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
		//找头
		size_t currentidx=0;
		while (currentidx<prt_current_pbuffer->buff_length())
		{
			bool bfound=false;
			size_t oridatalen=0;
			size_t datalen=0;
			if (CNetDataMaker::_head==(&(*prt_current_pbuffer->ptr_buffer))[currentidx])
			{
				//压缩前长度和压缩后长度
				if (currentidx+sizeof(CNetDataMaker::_head)+sizeof(size_t)*2<prt_current_pbuffer->buff_length())
				{
					 oridatalen=*((size_t*)(&*(prt_current_pbuffer->ptr_buffer)+currentidx+sizeof(CNetDataMaker::_head)));
					 datalen=*((size_t*)((&(*prt_current_pbuffer->ptr_buffer))+currentidx+sizeof(CNetDataMaker::_head)+sizeof(size_t)));

					//找到一段数据，校验
					if (currentidx+sizeof(CNetDataMaker::_head)+sizeof(size_t)*2+datalen+sizeof(crc32type)+sizeof(CNetDataMaker::_tail)<=prt_current_pbuffer->buff_length())
					{
						//校验
						int idxcheck=currentidx+sizeof(CNetDataMaker::_head)+sizeof(size_t)*2+datalen;
						crc32type crc32value=*((crc32type*)(&(*prt_current_pbuffer->ptr_buffer)+idxcheck));
						boost::crc_32_type crc32;
						crc32.process_bytes(&*(prt_current_pbuffer->ptr_buffer)+currentidx,sizeof(CNetDataMaker::_head)+sizeof(size_t)*2+datalen);    
						crc32type crc32check = crc32();
						//found
						if (crc32value==crc32check)
						//int idx=currentidx+sizeof(CNetDataMaker::_head)+sizeof(size_t)*2+datalen+sizeof(crc32type);
						//if (CNetDataMaker::_tail==(&(*prt_current_pbuffer->ptr_buffer))[idx])
						{
							bfound=true;
							size_t uncomlen=oridatalen;
							shared_ptr<CBufferObj> databuff = shared_ptr<CBufferObj> (new CBufferObj(0,uncomlen));
							int comRet = uncompress((Bytef*)(&*(databuff->ptr_buffer)),(uLongf*)&uncomlen,
								(Bytef*)(&*(prt_current_pbuffer->ptr_buffer)+currentidx+sizeof(CNetDataMaker::_head)+sizeof(size_t)*2),datalen);
							if (Z_OK==comRet)
							{
								PushBackOutBuffer(databuff);
							}
						}
					}
				}
			}
			if (bfound)
			{
				currentidx+=sizeof(CNetDataMaker::_head)+sizeof(size_t)*2+datalen+sizeof(CNetDataMaker::_tail);
			}
			else
			{
				currentidx++;
			}
		}
		//处理完成，清理
		if (currentidx>=prt_current_pbuffer->buff_length()-1)
		{
			prt_current_pbuffer=nullptr;
		}
		else
		{
			size_t newlen = prt_current_pbuffer->buff_length()-currentidx;
			shared_ptr<CBufferObj> ptr_new_buff = shared_ptr<CBufferObj>(new CBufferObj((&*prt_current_pbuffer->ptr_buffer)+currentidx,newlen));
			prt_current_pbuffer=ptr_new_buff;
		}
	}
}