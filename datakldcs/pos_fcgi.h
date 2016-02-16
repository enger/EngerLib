#include <string>
using namespace std;
typedef struct _tagCNV_POS_POSITION
{
	int				lX;             //经度
	int				lY;             //纬度
	int				lSpeed;         //速度
	short			lZ;             //高程
	short			lDirection;     //行驶方向（正北）
	unsigned int	lTime;          //采集时间
	int				CareType;		//车种
	int				Remark;			//附加属性
	int				DUID;			//设备ID
	int				KUID;			//用户ID
	char			LicenceNum[64];	//车牌号
}CNV_POS_POSITION;

//FCGI接口使用结构
//请求公共头
typedef struct _tagCNV_REQUEST_HEAD_FCGI
{
	int				Src;		//来源
	unsigned char	CSystem;	//坐标系系统:1代表WGS84
	unsigned char	Protocol;	//协议版本,当前版本为1
	unsigned char	Num;		//设备个数
	unsigned char	IsZip;		//是否压缩:1压缩,0不压缩
}CNV_REQUEST_HEAD_FCGI;

//设备头
typedef struct _tagCNV_DEV_HEAD_LICS
{
	char	FCuid[16];		//终端唯一标识：使用Duid,没有忽略该位置点
	//前15字节用于存储唯一标识,最后一个字节存储是否有附加信息
	//最后一个字节：第0位表示是否有车牌,1代表有,使用该结构		第1-7位保留
	int		CareType;		//车种
	int		PosNum;			//位置点个数
	char	License[12];	//附加信息
}CNV_DEV_HEAD_LICS;

typedef struct _tagCNV_DEV_HEAD_FCGI_NOLICS
{
	char	FCuid[16];	//同上
	int		CareType;
	int		PosNum;
}CNV_DEV_HEAD_FCGI_NOLICS;

//位置数据
typedef struct _tagCNV_POS_FCGI
{
	int				X;			//经度
	int				Y;			//纬度
	int				Speed;		//速度
	short			High;		//高程
	short			Direction;	//行驶方向
	unsigned int	Time;		//采集时间
	int				Remark;		//附加属性
}CNV_POS_FCGI;

int post_demo(CNV_POS_POSITION *posData,int posNum,unsigned char isZip,string datakey,int src,char** out_buf,int* out_len);