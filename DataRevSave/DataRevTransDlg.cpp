
// DataRevTransDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DataRevTrans.h"
#include "DataRevTransDlg.h"
#include "DlgCityState.h"
#include "afxdialogex.h"
#include <globaldeclear.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDataRevTransDlg 对话框




CDataRevTransDlg::CDataRevTransDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDataRevTransDlg::IDD, pParent)
	,m_prtProinfo(nullptr)
	,m_pDlgCityState(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDataRevTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDataRevTransDlg, CDialogEx)
	ON_REGISTERED_MESSAGE(WMEX_TASKBARCREATED, OnRestartExplorer)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_START, &CDataRevTransDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CDataRevTransDlg::OnBnClickedButtonStop)
	ON_WM_HOTKEY()
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDataRevTransDlg 消息处理程序

BOOL CDataRevTransDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_strModelName=GetModelName();
	SetWindowTextA(m_hWnd,m_strModelName.c_str());

	m_trayiocn.AddIcon(m_hWnd,m_hIcon,WM_USER+1234,MultCharToWideChar(m_strModelName).c_str());

	//m_prtProinfo = std::shared_ptr<CProInfo>(new CProInfo("tsdata.dll"));//m_proManager.GetProInfo();
	////m_proManager.AddPro("tsdata.dll",false);
	////m_prtProinfo = m_proManager.GetProInfo("tsdata.dll");
	////m_proManager.ClearPro();
	//if (m_prtProinfo!=nullptr )
	//{
	//	//SetDlgItemInt(IDC_EDIT_PORT,m_prtProinfo->GetPort());
	//}
	m_startTime = CTime::GetCurrentTime();
	SetTimer(IDS_UPDATE_ELPS_TIMER,1000,NULL);

	m_pDlgCityState = new CDlgCityState();
	m_pDlgCityState->Create(IDD_DIALOG_OTHER_CITY,this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
LRESULT CDataRevTransDlg::OnRestartExplorer(WPARAM wParam, LPARAM lParam)
{
	//添加系统图标
	m_trayiocn.AddIcon(m_hWnd,m_hIcon,WM_USER+1234,MultCharToWideChar(m_strModelName).c_str());
	return 1;
}

void CDataRevTransDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if(nID==SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);    //隐藏窗口
		m_trayiocn.AddIcon(m_hWnd,m_hIcon,WM_USER+1234,MultCharToWideChar(m_strModelName).c_str());        //添加系统图标
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDataRevTransDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDataRevTransDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDataRevTransDlg::OnClose()
{
	CString szTemp;
	szTemp.Format(TEXT("是否退出“%s”？"),MultCharToWideChar(m_strModelName).c_str());
	if (MessageBox(szTemp,TEXT("退出"),MB_YESNO|MB_ICONQUESTION)==IDYES)
	{
		m_proManager.Stop();
		CDialog::OnClose();
	}
}


void CDataRevTransDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case IDS_UPDATE_STATES:
		{
			//CStringA szASize(GetFormateDataSize(m_prtProinfo->GetRCVDataSize()).c_str());
			//SetDlgItemTextA(m_hWnd,IDC_EDIT_DATA_SIZE,szASize);
			//string strstate = m_proManager.GetRunningState();
			//SetWindowTextA(GetDlgItem(IDC_STATIC_RUNING_STATE)->m_hWnd,strstate.c_str());
		}
		break;
	case IDS_UPDATE_ELPS_TIMER:
		{
			{
				CString sztime;
				CTime timeNow = CTime::GetCurrentTime();
				CTimeSpan timeSpan = timeNow - m_startTime;
				LONGLONG llDays = timeSpan.GetDays();
				LONG	 lHours = timeSpan.GetHours();
				LONG	 lMinutes = timeSpan.GetMinutes();
				LONG	 lSenconds = timeSpan.GetSeconds();
				if (llDays>0)
				{
					sztime.Format(TEXT("运行时间: %I64d天 %02d:%02d:%02d"),llDays,lHours,lMinutes,lSenconds); //输出时、分、秒
				}
				else
				{
					sztime.Format(TEXT("运行时间: %02d:%02d:%02d"),lHours,lMinutes,lSenconds); //输出时、分、秒
				}
				SetDlgItemText(IDC_STATIC_RUN_TIME,sztime);

				//string strstate = m_proManager.GetRunningState();
				//wstring wstrState=MultCharToWideChar(strstate);
				//CString runstate;
				//runstate.Format(TEXT("%s\r\n%s"),sztime,wstrState.c_str());
				//SetDlgItemText(IDC_EDIT_STATE,runstate);
			}
		}
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CDataRevTransDlg::OnOK()
{
}


BOOL CDataRevTransDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE: //ESC
			return TRUE;
		}
	}  
	return CDialogEx::PreTranslateMessage(pMsg);
}


LRESULT CDataRevTransDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	m_trayiocn.WindowProc(message,wParam,lParam);
	return CDialogEx::WindowProc(message, wParam, lParam);
}


void CDataRevTransDlg::OnDropFiles(HDROP hDropInfo)
{
	UpdateData(TRUE);
	char   cFileName[256];   
	UINT   uFileCount;   
	uFileCount   = ::DragQueryFileA(hDropInfo,   -1, cFileName,   sizeof(cFileName));   
	//拖拉的文件个数 
	if (uFileCount==1)
	{
		::DragQueryFileA(hDropInfo,0,cFileName,   sizeof(cFileName));   //取得每个文件的文件名   
	}
	::DragFinish(hDropInfo); 

	POINT point;
	GetCursorPos(&point); 
	RECT rect;
	GetWindowRect(&rect);
	int width=rect.right-rect.left;
	int offsetx=point.x-rect.left;
	if (offsetx*2>width)
	{
		m_proManager.AddPro(string(cFileName));
	}
	else
	{
		std::shared_ptr<CProInfo> pro = std::shared_ptr<CProInfo>(new CProInfo((string(cFileName))));//m_proManager.GetProInfo();

		m_proManager.RemovePro(pro->m_prtDataProcesser->m_strModelKey);
	}
	CDialogEx::OnDropFiles(hDropInfo);
}



void CDataRevTransDlg::OnBnClickedButtonStart()
{
	//STARTUPINFO si;
	//ZeroMemory(&si,sizeof(si));
	//si.cb = sizeof(si);
	//si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	//si.lpReserved    = NULL;
	//si.cbReserved2   = 0;
	//si.lpReserved2   = NULL; 
	//TCHAR cmdLine[] = TEXT("E:\\work\\engerlib\\EngerLib\\Debug\\datashell.exe test");
	//PROCESS_INFORMATION ProcessInformation;
	//ZeroMemory(&ProcessInformation,sizeof(ProcessInformation));


	//int ret=CreateProcess(NULL,cmdLine,NULL,NULL,1,CREATE_NEW_CONSOLE,NULL,NULL,&si,&ProcessInformation);
	SetTimer(IDS_UPDATE_STATES,1000,NULL);
	//m_prtProinfo->Start();
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
}


void CDataRevTransDlg::OnBnClickedButtonStop()
{
	KillTimer(IDS_UPDATE_STATES);
	//m_prtProinfo->Stop();
	//m_proManager.Stop();
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
}


void CDataRevTransDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	if (nHotKeyId== 1001||nHotKeyId == 1002)
	{
		if (m_pDlgCityState==NULL)
		{
			m_pDlgCityState = new CDlgCityState();
			m_pDlgCityState->Create(IDD_DIALOG_OTHER_CITY,this);
		}
		m_pDlgCityState->m_pProManager=&m_proManager;
		m_pDlgCityState->ShowWindow(SW_SHOW);
	}
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


int CDataRevTransDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	RegisterHotKey(m_hWnd,1001,MOD_CONTROL|MOD_SHIFT|MOD_ALT,'K');
	RegisterHotKey(m_hWnd,1002,MOD_CONTROL|MOD_SHIFT|MOD_ALT,'k');
	return 0;
}


void CDataRevTransDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	m_pDlgCityState->DestroyWindow();
	delete m_pDlgCityState;
	m_pDlgCityState=NULL;
	UnregisterHotKey(m_hWnd, 1001);
	UnregisterHotKey(m_hWnd, 1002);
}
