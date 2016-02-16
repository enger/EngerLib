
// DataRevTrans.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "DataRevTrans.h"
#include "DataRevTransDlg.h"
#include "DlgModelManage.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDataRevTransApp

BEGIN_MESSAGE_MAP(CDataRevTransApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDataRevTransApp ����

CDataRevTransApp::CDataRevTransApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CDataRevTransApp ����

CDataRevTransApp theApp;

// CDataRevTransApp ��ʼ��

BOOL CDataRevTransApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	//size_t ssize = sizeof(GPS_INFO);
	//ssize=ssize;
	//GPS_INFO gpsinfo;
	//sprintf_s(gpsinfo.strGPSID,"�H�£�");
	//gpsinfo.ucStateMask=0xF;
	//gpsinfo.ucType=0xF;
	//gpsinfo.ulGPSTime=14123123;
	//gpsinfo.ulLatitude=13500000;
	//gpsinfo.ulLongitude=23000000;
	//gpsinfo.ulRCVTime=1410034;
	//gpsinfo.usDir=0x3F;
	//gpsinfo.usSpeed=0x88;

	//char* pbuffer = (char*)&gpsinfo;
	//size_t idx=0;
	//unsigned long	ulGPSTime=*((unsigned long*)(pbuffer+idx));
	//idx+=sizeof(unsigned long);
	//unsigned long	ulRCVTime=*((unsigned long*)(pbuffer+idx));
	//idx+=sizeof(unsigned long);
	//unsigned long	ulLongitude=*((unsigned long*)(pbuffer+idx));
	//idx+=sizeof(unsigned long);
	//unsigned long	ulLatitude=*((unsigned long*)(pbuffer+idx));
	//idx+=sizeof(unsigned long);
	//unsigned short	usSpeed=*((unsigned short*)(pbuffer+idx));
	//idx+=sizeof(unsigned short);
	//unsigned short	usDir=*((unsigned short*)(pbuffer+idx));
	//idx+=sizeof(unsigned short);
	//unsigned char	ucType=*((unsigned char*)(pbuffer+idx));
	//idx+=sizeof(unsigned char);
	//unsigned char	ucStateMask=*((unsigned char*)(pbuffer+idx));
	//idx+=sizeof(unsigned char);
	//char* strGPSID=((pbuffer+idx));

	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CDlgModelManage dlg;
	//CDataRevTransDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

