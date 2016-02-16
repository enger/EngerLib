// DlgModelDetail.cpp : 实现文件
//

#include "stdafx.h"
#include "DataRevTrans.h"
#include "DlgModelDetail.h"
#include "afxdialogex.h"

#include <globaldeclear.h>
//boost
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp>  
#include <boost/property_tree/xml_parser.hpp>
// CDlgModelDetail 对话框
using namespace std;
using namespace boost;
using namespace boost::property_tree;

IMPLEMENT_DYNAMIC(CDlgModelDetail, CDialogEx)

CDlgModelDetail::CDlgModelDetail(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgModelDetail::IDD, pParent)
	,m_pProManager(NULL)
{

}

CDlgModelDetail::~CDlgModelDetail()
{
}

void CDlgModelDetail::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DETAIL, m_editState);
}


BEGIN_MESSAGE_MAP(CDlgModelDetail, CDialogEx)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDlgModelDetail 消息处理程序


void CDlgModelDetail::OnTimer(UINT_PTR nIDEvent)
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


BOOL CDlgModelDetail::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetTimer(IDS_UPDATE_STATES,1000,NULL);
	return TRUE; 
}


void CDlgModelDetail::OnClose()
{
	KillTimer(IDS_UPDATE_STATES);
	ShowWindow(SW_HIDE);
}
void CDlgModelDetail::OnOK()
{
	KillTimer(IDS_UPDATE_STATES);
	ShowWindow(SW_HIDE);
}
void CDlgModelDetail::OnDestroy()
{
	KillTimer(IDS_UPDATE_STATES);
	ShowWindow(SW_HIDE);
}
void CDlgModelDetail::UpdataModelState()
{
	if (!IsWindowVisible())
	{
		return;
	}
	if (m_strModelName.length()==0 || m_pProManager==NULL)
	{
		return;
	}
	string strRunState=m_pProManager->GetRunningState(m_strModelName);
	if (strRunState.length()==0)
	{
		return;
	}
	stringstream strss(strRunState);
	ptree pt;
	read_xml(strss, pt);  
	strss.clear();
	strss.str("");

	ptree ptModel = pt.get_child("Model");
	ptree ptRcv=ptModel.get_child("Recive");
	ptree ptSendArry=ptModel.get_child("Send");
	string strModelKey=ptModel.get<string>("<xmlattr>.ModelKey");

	strss<<"模块名	模块编码"<<"\r\n";
	strss<<m_strModelName<<"	"<<strModelKey<<"\r\n";
	strss<<"\r\n"<<"==============="<<"\r\n";
	strss<<"\r\n";

	string strRCVName =  ptRcv.get<string>("<xmlattr>.Name"); 
	string strRCVAdd = ptRcv.get<string>("<xmlattr>.Address"); 
	string strRCVProtocol=ptRcv.get<string>("Protocol"); 
	unsigned long long ullRcvSize = ptRcv.get<unsigned long long>("RcvSize");
	string strRcvSize =GetFormateDataSize(ullRcvSize);
	strss<<"接收"<<"\r\n";
	strss<<"名称 地址	协议	数据量"<<"\r\n";
	strss<<strRCVName<<"	"<<strRCVAdd<<"	"<<strRCVProtocol<<"	"<<strRcvSize<<"\r\n";
	strss<<"\r\n"<<"-----------------"<<"\r\n"<<"\r\n";

	for (ptree::iterator itSend = ptSendArry.begin(); itSend != ptSendArry.end(); ++itSend)
	{
		ptree ptSend=itSend->second;
		string strSendName =  ptSend.get<string>("<xmlattr>.Name"); 
		string strSendAdd = ptSend.get<string>("<xmlattr>.Address"); 
		string strSendProtocol=ptSend.get<string>("Protocol"); 
		unsigned long long ullSendSize = ptSend.get<unsigned long long>("SendSize");
		string strSendSize =GetFormateDataSize(ullSendSize);
		strss<<"发送"<<"\r\n";
		strss<<"名称 地址	协议	数据量"<<"\r\n";
		strss<<strSendName<<"	"<<strSendAdd<<"	"<<strSendProtocol<<"	"<<strSendSize<<"\r\n";
		strss<<"\r\n"<<"-----------------"<<"\r\n"<<"\r\n";
		if (strSendProtocol=="ServerTCP")
		{
			strss<<"客户端"<<"\r\n";
			ptree ptClientsArry=ptSend.get_child("Clients");
			for (ptree::iterator itClient = ptClientsArry.begin(); itClient != ptClientsArry.end(); ++itClient)
			{
				strss<<itClient->second.get_value<string>()<<"\r\n";
			}
		}
	}
	CString szTextOut(MultCharToWideChar(strss.str()).c_str());
	m_editState.SetWindowTextW(szTextOut);
}

void CDlgModelDetail::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	RECT r;
	GetClientRect(&r);
	CRect rect(r);
	if (GetDlgItem(IDC_EDIT_DETAIL)==NULL)
	{
		return;
	}
	//模块状态列表
	RECT rModelState;
	rModelState.left=r.left+10;
	rModelState.right=r.right-10;
	rModelState.top=r.top+10;
	rModelState.bottom=r.bottom-10;
	CRect rectModelState(rModelState);
	m_editState.MoveWindow(&rectModelState,TRUE);
}

void CDlgModelDetail::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		SetTimer(IDS_UPDATE_STATES,1000,NULL);
	}
	else
	{
		KillTimer(IDS_UPDATE_STATES);
	}
}
