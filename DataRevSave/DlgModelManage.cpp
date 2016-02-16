// ModelManage.cpp : 实现文件
//

#include "stdafx.h"
#include "DataRevTrans.h"
#include "DlgModelManage.h"
#include "afxdialogex.h"
#include <globaldeclear.h>
#include "DlgModelDetail.h"

#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp>  
#include <boost/property_tree/xml_parser.hpp>

// CModelManage 对话框

IMPLEMENT_DYNAMIC(CDlgModelManage, CDialogEx)

CDlgModelManage::CDlgModelManage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgModelManage::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDlgModelManage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODEL_STATE, m_listModelState);
	DDX_Control(pDX, IDC_BUTTON_UPDATE, m_btUpdate);
	DDX_Control(pDX, IDC_BUTTON_STOP_RESUME, m_btStopResume);
}


BEGIN_MESSAGE_MAP(CDlgModelManage, CDialogEx)
	ON_REGISTERED_MESSAGE(WMEX_TASKBARCREATED, OnRestartExplorer)
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_WM_HOTKEY()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CDlgModelManage::OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_STOP_RESUME, &CDlgModelManage::OnBnClickedButtonStopResume)
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_MODEL_STATE, &CDlgModelManage::OnNMRClickListModelState)
	ON_COMMAND(ID_CONTEXT_START, &CDlgModelManage::OnContextStart)
	ON_COMMAND(ID_CONTEXT_STOP, &CDlgModelManage::OnContextStop)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MODEL_STATE, &CDlgModelManage::OnNMDblclkListModelState)
END_MESSAGE_MAP()


// CModelManage 消息处理程序

BOOL CDlgModelManage::OnInitDialog()
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
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_strModelName=GetModelName();
	SetWindowTextA(m_hWnd,m_strModelName.c_str());
	m_trayiocn.AddIcon(m_hWnd,m_hIcon,WM_USER+1234,MultCharToWideChar(m_strModelName).c_str());
	ShowWindow(SW_MAXIMIZE);
	CRect rt;
	m_listModelState.GetClientRect (&rt);
	m_listModelState.SetExtendedStyle(m_listModelState.GetExtendedStyle() |LVS_EX_HEADERDRAGDROP| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES|LVS_EX_AUTOSIZECOLUMNS);   
	m_listModelState.InsertColumn (0,_T("模块名"),LVCFMT_LEFT,100);
	m_listModelState.InsertColumn (1,_T("编号"),LVCFMT_LEFT,200);
	m_listModelState.InsertColumn (2,_T("城市名"),LVCFMT_LEFT,180);
	m_listModelState.InsertColumn (3,_T("PID"),LVCFMT_LEFT,80);
	m_listModelState.InsertColumn (4,_T("接收地址"),LVCFMT_LEFT,180);
	m_listModelState.InsertColumn (5,_T("接收量"),LVCFMT_LEFT,150);
	m_listModelState.InsertColumn (6,_T("状态"),LVCFMT_LEFT,100);
	//m_listModelState.SetImageList(&m_imgList,LVSIL_SMALL);
	m_ProManager.Initiate();
	SetTimer(IDS_UPDATE_STATES,1000,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
LRESULT CDlgModelManage::OnRestartExplorer(WPARAM wParam, LPARAM lParam)
{
	//添加系统图标
	m_trayiocn.AddIcon(m_hWnd,m_hIcon,WM_USER+1234,MultCharToWideChar(m_strModelName).c_str());
	return 1;
}
void CDlgModelManage::OnSysCommand(UINT nID, LPARAM lParam)
{
	 if(nID==SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);    //隐藏窗口
		m_trayiocn.AddIcon(m_hWnd,m_hIcon,WM_USER+1234,MultCharToWideChar(m_strModelName).c_str());        //添加系统图标
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
HCURSOR CDlgModelManage::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CDlgModelManage::OnClose()
{
	CString szTemp;
	szTemp.Format(TEXT("是否退出“%s”？"),MultCharToWideChar(m_strModelName).c_str());
	if (MessageBox(szTemp,TEXT("退出"),MB_YESNO|MB_ICONQUESTION)==IDYES)
	{
		//m_proManager.Stop();
		CDialogEx::OnClose();
	}

}

void CDlgModelManage::OnOK()
{
}

BOOL CDlgModelManage::PreTranslateMessage(MSG* pMsg)
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

LRESULT CDlgModelManage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	m_trayiocn.WindowProc(message,wParam,lParam);
	return CDialogEx::WindowProc(message, wParam, lParam);
}
void CDlgModelManage::OnDropFiles(HDROP hDropInfo)
{
	UpdateData(TRUE);
	char   cFileName[256];   
	UINT   uFileCount;   
	uFileCount   = ::DragQueryFileA(hDropInfo,   -1, cFileName,   sizeof(cFileName));   
	//拖拉的文件个数 
	if (uFileCount==1)
	{
		::DragQueryFileA(hDropInfo,0,cFileName,   sizeof(cFileName));
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
		m_ProManager.AddPro(string(cFileName));
	}
	else
	{
		std::shared_ptr<CProInfo> pro = std::shared_ptr<CProInfo>(new CProInfo((string(cFileName))));//m_proManager.GetProInfo();
		m_ProManager.RemovePro(pro->m_prtDataProcesser->m_strModelKey);
	}
	CDialogEx::OnDropFiles(hDropInfo);
}
void CDlgModelManage::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	if (nHotKeyId== 1001||nHotKeyId == 1002)
	{
		this->ShowWindow(SW_SHOW);
	}

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

int CDlgModelManage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	RegisterHotKey(m_hWnd,1001,MOD_CONTROL|MOD_SHIFT|MOD_ALT,'K');
	RegisterHotKey(m_hWnd,1002,MOD_CONTROL|MOD_SHIFT|MOD_ALT,'k');

	return 0;
}


void CDlgModelManage::OnDestroy()
{
	CDialogEx::OnDestroy();
	UnregisterHotKey(m_hWnd, 1001);
	UnregisterHotKey(m_hWnd, 1002);
}

void CDlgModelManage::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	RECT r;
	GetClientRect(&r);
	//r.bottom-=20;
	CRect rect(r);
	if (GetDlgItem(IDC_LIST_MODEL_STATE)==NULL)
	{
		return;
	}
	//模块状态列表
	RECT rModelState;
	m_listModelState.GetWindowRect(&rModelState);
	ScreenToClient(&rModelState);
	rModelState.bottom=r.bottom-40;
	rModelState.right=r.right-10;
	CRect rectModelState(rModelState);
	m_listModelState.MoveWindow(&rectModelState,TRUE);

	//更新按钮
	RECT rStartBTN;
	m_btUpdate.GetWindowRect(&rStartBTN);
	ScreenToClient(&rStartBTN);
	int width = rStartBTN.right-rStartBTN.left;
	int heith=rStartBTN.bottom-rStartBTN.top;
	int l=rectModelState.left+0.5*(rectModelState.Width()-3*width);
	int t=rectModelState.bottom+10;
	CRect rectStartBTN(l,t,l+width,t+heith);
	m_btUpdate.MoveWindow(&rectStartBTN,TRUE);
	//停止或恢复按钮
	CRect rectStopResumeBTN(l+2*width,t,l+3*width,t+heith);
	m_btStopResume.MoveWindow(&rectStopResumeBTN,TRUE);
}


void CDlgModelManage::OnBnClickedButtonUpdate()
{
	
}


void CDlgModelManage::OnBnClickedButtonStopResume()
{
	TCHAR text[MAX_PATH];
	m_btStopResume.GetWindowTextW(text,MAX_PATH);
	if (wcscmp(text,TEXT("停止"))==0)
	{
		m_btStopResume.SetWindowTextW(TEXT("恢复"));
	}
	else
	{
		m_btStopResume.SetWindowTextW(TEXT("停止"));
	}
}

void CDlgModelManage::UpdataModelState()
{
	if (!IsWindowVisible())
	{
		return;
	}
	map<string,shared_ptr<CProcessProInfo>> mapProcessInfo;
	m_ProManager.GetProcess(mapProcessInfo);
	map<string,shared_ptr<CProcessProInfo>>::iterator ite = mapProcessInfo.begin();
	for (;ite!=mapProcessInfo.end();ite++)
	{
		int c=m_listModelState.GetItemCount();
		CString szMapModelName(MultCharToWideChar(ite->first).c_str());
		CString szListModelName;
		bool bHasTheCity=false;
		for (int j=0;j<c;j++)
		{
			szListModelName=m_listModelState.GetItemText(j,0);
			if (szMapModelName.Compare(szListModelName)==0)
			{
				bHasTheCity=true;
				if (ite->second!=nullptr)
				{
					string strRunState=m_ProManager.GetRunningState(ite->first);
					CString szModelKey(MultCharToWideChar(ite->second->modelKey).c_str());
					m_listModelState.SetItemText(j,1,szModelKey);
					m_listModelState.SetItemText(j,6,ite->second->isRunning?TEXT("正常"):TEXT("错误"));

					CString sztemp;
					sztemp.Format(TEXT("%d"),ite->second->pi.dwProcessId);
					m_listModelState.SetItemText(j,3,sztemp);
					if (strRunState.length()>0)
					{
						stringstream strss(strRunState);
						boost::property_tree::ptree pt;
						boost::property_tree::read_xml(strss, pt);  
						boost::property_tree::ptree ptModel = pt.get_child("Model");
						boost::property_tree::ptree ptRcv=ptModel.get_child("Recive");
						string strName =  ptRcv.get<string>("<xmlattr>.Name"); 
						string strAdd = ptRcv.get<string>("<xmlattr>.Address"); 
						unsigned long long ullRcvSize = ptRcv.get<unsigned long long>("RcvSize");
						m_listModelState.SetItemText(j,2,MultCharToWideChar(strName).c_str());
						m_listModelState.SetItemText(j,4,MultCharToWideChar(strAdd).c_str());
						string strRcvSize =GetFormateDataSize(ullRcvSize);
						m_listModelState.SetItemText(j,5,MultCharToWideChar(strRcvSize).c_str());
					}
				}
				else
				{
					m_listModelState.SetItemText(j,6,TEXT("错误"));
				}
				break;
			}
		}
		//没有这个城市添加之
		if (!bHasTheCity)
		{
			int idx = m_listModelState.GetItemCount();
			m_listModelState.InsertItem(idx,szMapModelName);
		}
	}
	m_listModelState.UpdateData(TRUE);
}
void CDlgModelManage::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case IDS_UPDATE_STATES:
		UpdataModelState();
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CDlgModelManage::OnNMRClickListModelState(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	{
		POSITION pos=m_listModelState.GetFirstSelectedItemPosition();
		if(pos==NULL)
		{
			return;
		}
		int nId=(int)m_listModelState.GetNextSelectedItem(pos);
		//得到行号，通过POSITION转化
		////得到列中的内容（0表示第一列，同理1,2,3...表示第二，三，四...列）
		//str=m_listcontrol.GetItemText(nId,0);
		//str=m_listcontrol.GetItemText(nId,1);
	}
	CMenu menu; 
	menu.LoadMenu(IDR_MENU_CONTEXT); //载入事先定义的选单 
	CMenu*pMenu=menu.GetSubMenu(0); 
	CPoint pos; 
	GetCursorPos(&pos);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,pos.x,pos.y,AfxGetMainWnd());
	*pResult = 0;
}


void CDlgModelManage::OnContextStart()
{
	POSITION pos=m_listModelState.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{
		return;
	}
	int nId=(int)m_listModelState.GetNextSelectedItem(pos);
	//得到行号，通过POSITION转化
	////得到列中的内容（0表示第一列，同理1,2,3...表示第二，三，四...列）
	CString szModelName = m_listModelState.GetItemText(nId,0);
	string strModelName(WideCharToMultiChar(wstring(szModelName.GetBuffer())));
	szModelName.ReleaseBuffer();
	m_ProManager.Start(strModelName);
}


void CDlgModelManage::OnContextStop()
{
	POSITION pos=m_listModelState.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{
		return;
	}
	int nId=(int)m_listModelState.GetNextSelectedItem(pos);
	//得到行号，通过POSITION转化
	////得到列中的内容（0表示第一列，同理1,2,3...表示第二，三，四...列）
	CString szModelName = m_listModelState.GetItemText(nId,0);
	string strModelName(WideCharToMultiChar(wstring(szModelName.GetBuffer())));
	szModelName.ReleaseBuffer();
	m_ProManager.Stop(strModelName);
}


void CDlgModelManage::OnNMDblclkListModelState(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if(-1 != pNMItemActivate->iItem)  
	{  
		CString szModelName=m_listModelState.GetItemText(pNMItemActivate->iItem,0);
		string strModelName=WideCharToMultiChar(wstring(szModelName.GetBuffer()));
		map<string,shared_ptr<CDlgModelDetail>>::iterator ite = m_mapDlgModelDetail.find(strModelName);
		if (ite==m_mapDlgModelDetail.end())
		{
			shared_ptr<CDlgModelDetail> dlgModelDetail = shared_ptr<CDlgModelDetail>(new CDlgModelDetail());			
			dlgModelDetail->Create(IDD_DIALOG_MODEL_DETAIL,this);
			std::pair<map<string,shared_ptr<CDlgModelDetail>>::iterator,bool > ret;
			ret=m_mapDlgModelDetail.insert(make_pair(strModelName,dlgModelDetail));
			if (ret.second)
			{
				ite=ret.first;
			}
		}
		if (ite!=m_mapDlgModelDetail.end())
		{
			ite->second->m_pProManager=&m_ProManager;
			ite->second->m_strModelName=ite->first;
			ite->second->ShowWindow(SW_SHOW);
		}
	}
	*pResult = 0;
}
