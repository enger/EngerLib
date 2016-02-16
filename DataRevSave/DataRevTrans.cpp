
// DataRevTrans.cpp : 定义应用程序的类行为。
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


// CDataRevTransApp 构造

CDataRevTransApp::CDataRevTransApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CDataRevTransApp 对象

CDataRevTransApp theApp;

// CDataRevTransApp 初始化

BOOL CDataRevTransApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	//size_t ssize = sizeof(GPS_INFO);
	//ssize=ssize;
	//GPS_INFO gpsinfo;
	//sprintf_s(gpsinfo.strGPSID,"肏屄！");
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

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CDlgModelManage dlg;
	//CDataRevTransDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

