#include "Data.h"
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

CData::CData()
	:m_ulGPSTime(0)
	,m_strCityFlag("")
	,m_strGPSID("")
	,m_dLongitude(0)
	,m_dLatitude(0)
	,m_dSpeed(0)
	,m_dDir(0)
	,m_iType(0)
	,m_iLoadState(0)
	,m_iValid(0)
	,m_ulRCVTime(0)
{
}
CData::~CData(void)
{
}
size_t CData::get_buffer_size()
{
	size_t s=0;
	s+=sizeof(m_ulGPSTime);
	s+=sizeof(unsigned int);
	s+=m_strGPSID.length()+1;
	s+=sizeof(unsigned int);//longitude
	s+=sizeof(unsigned int);//latitude
	s+=sizeof(unsigned short);//m_dSpeed
	s+=sizeof(unsigned short);//m_dDir
	s+=sizeof(unsigned short);//m_iType
	s+=sizeof(unsigned char);//m_iLoadState,m_iValid
	s+=sizeof(m_ulRCVTime);
	return s;
}

std::shared_ptr<CBufferObj> CData::GetFormateData()
{
	if (m_strGPSID.length()==0)
	{
		return nullptr;
	}

	//size_t buffersize=get_buffer_size();
	//std::shared_ptr<CBufferObj> retbuf = std::shared_ptr<CBufferObj>(new CBufferObj(0,buffersize));
	//size_t idx=0;
	//memcpy_s(&*(retbuf->ptr_buffer)+idx,buffersize-idx,&m_ulGPSTime,sizeof(m_ulGPSTime));
	//idx+=sizeof(m_ulGPSTime);
	//unsigned int len=m_strGPSID.length()+1;
	//memcpy_s(&*(retbuf->ptr_buffer)+idx,buffersize-idx,&len,sizeof(len));
	//idx+=sizeof(len);
	//memcpy_s(&*(retbuf->ptr_buffer)+idx,buffersize-idx,m_strGPSID.c_str(),m_strGPSID.length());
	//idx+=m_strGPSID.length()+1;

	//unsigned int uilong = (unsigned int)(m_dLongitude*1e6);
	//unsigned int uilat = (unsigned int)(m_dLatitude*1e6);
	//unsigned short usspeed=(unsigned short)(m_dSpeed*1e2);
	//unsigned short usdir=(unsigned short)(m_dDir*1e2);
	//unsigned short ustype=(unsigned short)m_iType;

	//unsigned char lsvlmask=0;
	//lsvlmask|=(unsigned char)m_iLoadState;
	//lsvlmask|=((unsigned char)m_iValid<<1);

	//memcpy_s(&*(retbuf->ptr_buffer)+idx,buffersize-idx,&uilong,sizeof(uilong));
	//idx+=sizeof(uilong);

	//memcpy_s(&*(retbuf->ptr_buffer)+idx,buffersize-idx,&uilat,sizeof(uilat));
	//idx+=sizeof(uilat);

	//memcpy_s(&*(retbuf->ptr_buffer)+idx,buffersize-idx,&usspeed,sizeof(usspeed));
	//idx+=sizeof(usspeed);

	//memcpy_s(&*(retbuf->ptr_buffer)+idx,buffersize-idx,&usdir,sizeof(usdir));
	//idx+=sizeof(usdir);

	//memcpy_s(&*(retbuf->ptr_buffer)+idx,buffersize-idx,&ustype,sizeof(ustype));
	//idx+=sizeof(ustype);

	//memcpy_s(&*(retbuf->ptr_buffer)+idx,buffersize-idx,&lsvlmask,sizeof(lsvlmask));
	//idx+=sizeof(lsvlmask);

	return nullptr;
}

string CData::GetTimeID()
{
	using namespace boost::posix_time;
	using namespace std;
	using namespace boost::gregorian;
	ptime ptRCV(boost::date_time::c_local_adjustor<ptime>::utc_to_local(from_time_t(m_ulRCVTime)));
	string strRcvTime = to_iso_string(ptRCV);
	std::remove( strRcvTime.begin(), strRcvTime.end(), 'T'); 
	return strRcvTime.substr(0,12);
}

