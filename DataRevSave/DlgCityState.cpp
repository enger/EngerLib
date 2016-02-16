// DlgCityState.cpp : 实现文件
//

#include "stdafx.h"
#include "DataRevTrans.h"
#include "DlgCityState.h"
#include "afxdialogex.h"
#include <ProManager.h>
#include <globaldeclear.h>
// CDlgCityState 对话框

IMPLEMENT_DYNAMIC(CDlgCityState, CDialogEx)

CDlgCityState::CDlgCityState(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCityState::IDD, pParent)
	,m_pProManager(NULL)
{

}

CDlgCityState::~CDlgCityState()
{
}

void CDlgCityState::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CITY_STATE, m_listCityState);
}


BEGIN_MESSAGE_MAP(CDlgCityState, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgCityState 消息处理程序


void CDlgCityState::OnClose()
{
	ShowWindow(SW_HIDE);
}


void CDlgCityState::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case IDS_UPDATE_OHTER_STATES:
		UpdataCityState();
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CDlgCityState::OnOK()
{
}


void CDlgCityState::UpdataCityState(void)
{
	//if (m_pProManager==NULL) 
	//{
	//	return;
	//}
	//const map<string,shared_ptr<CProInfo>>& mapPro = m_pProManager->GetPros();
	//map<string,shared_ptr<CProInfo>>::const_iterator cite = mapPro.begin();
	//int citycount = m_listCityState.GetItemCount();
	//for (int j=citycount-1;j>=0;j--)
	//{
	//	CString sztmep=m_listCityState.GetItemText(j,0);
	//	string strsernum=WideCharToMultiChar(wstring(sztmep.GetBuffer()));
	//	sztmep.ReleaseBuffer();
	//	cite=mapPro.find(strsernum);
	//	if (cite==mapPro.end())
	//	{
	//		m_listCityState.DeleteItem(j);
	//	}
	//}

	//
	//for (cite = mapPro.begin();cite!=mapPro.end();cite++)
	//{
	//	int c=m_listCityState.GetItemCount();
	//	CString sztmep;
	//	bool bHasTheCity=false;
	//	CString szSerNum(MultCharToWideChar((cite)->second->GetSerNum()).c_str());
	//	for (int j=0;j<c;j++)
	//	{
	//		sztmep=m_listCityState.GetItemText(j,0);
	//		
	//		if (szSerNum.CompareNoCase(sztmep)==0)
	//		{
	//			bHasTheCity=true;
	//			//时长
	//			string strdatasize = GetFormateDataSize(cite->second->GetRCVDataSize());
	//			CString szTemp;
	//			szTemp.Format(TEXT("%s"),MultCharToWideChar(strdatasize).c_str());
	//			m_listCityState.SetItemText(j,2,szTemp);
	//			strdatasize= GetFormateDataSize(cite->second->GetSendDataSize());
	//			szTemp.Format(TEXT("%s"),MultCharToWideChar(strdatasize).c_str());
	//			m_listCityState.SetItemText(j,3,szTemp);
	//			//int iMinute = _tstoi(vecItems[3].GetBuffer());
	//			//m_listCityState.SetItemText(j,1,MinuteFormateTime(iMinute));
	//			//m_listCityState.SetItemText(j,3,szTime);

	//			////修改图标
	//			//LVITEM Itemattr;
	//			//Itemattr.iItem=j;  //指定哪一行
	//			//Itemattr.mask   =   LVIF_IMAGE;//|LVIF_TEXT;
	//			//Itemattr.iSubItem=0;
	//			//Itemattr.cchTextMax=MAX_PATH;
	//			////Itemattr.pszText=vecItems.at(3).GetBuffer();
	//			////要把当前图标改成m_Imagelist 中下标为 4 的图标。
	//			//if (vecItems.at(2).CompareNoCase(TEXT("正常"))==0)
	//			//{
	//			//	Itemattr.iImage =2;
	//			//}
	//			//else if (vecItems.at(2).CompareNoCase(TEXT("无法连接"))==0)
	//			//{
	//			//	Itemattr.iImage =0;
	//			//	alarm_pool_prt->schedule(boost::bind(&CDataMonitorClientDlg::AddAlarmMSG,this,vecCities[i],!IsNotAlarmExist(vecItems[1])));
	//			//}
	//			//else if (vecItems.at(2).CompareNoCase(TEXT("无数据"))==0)
	//			//{
	//			//	int timedurition = _tstoi(vecItems.at(3));
	//			//	if (timedurition<5)
	//			//	{
	//			//		Itemattr.iImage =1;
	//			//	}
	//			//	else
	//			//	{
	//			//		Itemattr.iImage =0;
	//			//		alarm_pool_prt->schedule(boost::bind(&CDataMonitorClientDlg::AddAlarmMSG,this,vecCities[i],!IsNotAlarmExist(vecItems[1])));
	//			//	}

	//			//}
	//			//m_listCityState.SetItem(&Itemattr);
	//			break;
	//		}
	//	}
	//	//没有这个城市添加之
	//	if (!bHasTheCity)
	//	{
	//		CString szTemp;
	//		int idx = m_listCityState.GetItemCount();
	//		m_listCityState.InsertItem(idx,szSerNum);
	//		//szTemp.Format(TEXT("%d %s"),idx+1,vecItems[0]);
	//		//if (vecItems.at(2).CompareNoCase(TEXT("正常"))==0)
	//		//{
	//		//	m_listCityState.InsertItem(idx,szTemp,2);//Itemattr.iImage =2;
	//		//}
	//		//else if (vecItems.at(2).CompareNoCase(TEXT("无法连接"))==0)
	//		//{
	//		//	m_listCityState.InsertItem(idx,szTemp,0);//Itemattr.iImage =0;
	//		//	alarm_pool_prt->schedule(boost::bind(&CDataMonitorClientDlg::AddAlarmMSG,this,vecCities[i],true));
	//		//}
	//		//else if (vecItems.at(2).CompareNoCase(TEXT("无数据"))==0)
	//		//{
	//		//	int timedurition = _tstoi(vecItems.at(3));
	//		//	if (timedurition<5)
	//		//	{
	//		//		m_listCityState.InsertItem(idx,szTemp,1);//Itemattr.iImage =1;
	//		//	}
	//		//	else
	//		//	{
	//		//		m_listCityState.InsertItem(idx,szTemp,0);//Itemattr.iImage =0;
	//		//		alarm_pool_prt->schedule(boost::bind(&CDataMonitorClientDlg::AddAlarmMSG,this,vecCities[i],!IsNotAlarmExist(vecItems[1])));
	//		//	}

	//		//}			
	//		//int iMinute = _tstoi(vecItems[3].GetBuffer());
	//		m_listCityState.SetItemText(idx,1,MultCharToWideChar(cite->second->GetName()).c_str());
	//		m_listCityState.SetItemText(idx,2,TEXT("0"));
	//		m_listCityState.SetItemText(idx,3,TEXT("0"));
	//		m_listCityState.SetCheck(idx);
	//	}

	//}
}


BOOL CDlgCityState::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rt;
	m_listCityState.GetClientRect (&rt);
	m_listCityState.SetExtendedStyle(m_listCityState.GetExtendedStyle() |LVS_EX_CHECKBOXES| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);   
	m_listCityState.InsertColumn (0,_T("序列号"),LVCFMT_LEFT,200);
	m_listCityState.InsertColumn (1,_T("城市名"),LVCFMT_LEFT,200);
	m_listCityState.InsertColumn (2,_T("接收数据量"),LVCFMT_LEFT,80);
	m_listCityState.InsertColumn (3,_T("发送数据量"),LVCFMT_LEFT,140);
	m_listCityState.InsertColumn (4,_T("更新时间"),LVCFMT_LEFT,100);

	SetTimer(IDS_UPDATE_OHTER_STATES,1000,NULL);
	return TRUE;
}
