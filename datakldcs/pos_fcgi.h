#include <string>
using namespace std;
typedef struct _tagCNV_POS_POSITION
{
	int				lX;             //����
	int				lY;             //γ��
	int				lSpeed;         //�ٶ�
	short			lZ;             //�߳�
	short			lDirection;     //��ʻ����������
	unsigned int	lTime;          //�ɼ�ʱ��
	int				CareType;		//����
	int				Remark;			//��������
	int				DUID;			//�豸ID
	int				KUID;			//�û�ID
	char			LicenceNum[64];	//���ƺ�
}CNV_POS_POSITION;

//FCGI�ӿ�ʹ�ýṹ
//���󹫹�ͷ
typedef struct _tagCNV_REQUEST_HEAD_FCGI
{
	int				Src;		//��Դ
	unsigned char	CSystem;	//����ϵϵͳ:1����WGS84
	unsigned char	Protocol;	//Э��汾,��ǰ�汾Ϊ1
	unsigned char	Num;		//�豸����
	unsigned char	IsZip;		//�Ƿ�ѹ��:1ѹ��,0��ѹ��
}CNV_REQUEST_HEAD_FCGI;

//�豸ͷ
typedef struct _tagCNV_DEV_HEAD_LICS
{
	char	FCuid[16];		//�ն�Ψһ��ʶ��ʹ��Duid,û�к��Ը�λ�õ�
	//ǰ15�ֽ����ڴ洢Ψһ��ʶ,���һ���ֽڴ洢�Ƿ��и�����Ϣ
	//���һ���ֽڣ���0λ��ʾ�Ƿ��г���,1������,ʹ�øýṹ		��1-7λ����
	int		CareType;		//����
	int		PosNum;			//λ�õ����
	char	License[12];	//������Ϣ
}CNV_DEV_HEAD_LICS;

typedef struct _tagCNV_DEV_HEAD_FCGI_NOLICS
{
	char	FCuid[16];	//ͬ��
	int		CareType;
	int		PosNum;
}CNV_DEV_HEAD_FCGI_NOLICS;

//λ������
typedef struct _tagCNV_POS_FCGI
{
	int				X;			//����
	int				Y;			//γ��
	int				Speed;		//�ٶ�
	short			High;		//�߳�
	short			Direction;	//��ʻ����
	unsigned int	Time;		//�ɼ�ʱ��
	int				Remark;		//��������
}CNV_POS_FCGI;

int post_demo(CNV_POS_POSITION *posData,int posNum,unsigned char isZip,string datakey,int src,char** out_buf,int* out_len);