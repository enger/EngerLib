// TrayIocn.cpp : 实现文件
//

//#include "stdafx.h"
//#include "resource.h"
#include "TrayIocn.h"


// CTrayIocn

CTrayIocn::CTrayIocn()
{
	memset((char*)&m_nd,0,sizeof(NOTIFYICONDATA));
}

CTrayIocn::~CTrayIocn()
{
	RemoveIcon();
}
 
  
// CTrayIocn 消息处理程序

/***************************************
* 将图标加入系统托盘
* 参数:
* hWnd - 窗体句柄
* hIcon  - 所要加入的图标句柄
* title     - 悬停于系统托盘之上时所要显示的消息
* uCallbackMessage - 消息标识(需用户自己定义消息)
***************************************/
void CTrayIocn::AddIcon(HWND hWnd, HICON hIcon, UINT uCallbackMessage,const TCHAR *title)
{
	m_parentWnd=hWnd;

	m_nd.cbSize = sizeof(NOTIFYICONDATA);
	m_nd.hWnd = hWnd;
	m_nd.uID = GetWindowLong(hWnd,GWL_ID);//IDR_MAINFRAME;
	m_nd.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
	m_nd.uCallbackMessage = uCallbackMessage;
	m_nd.hIcon = hIcon;
	wcscpy_s(m_nd.szTip,sizeof(m_nd.szTip), title);
	//Sends a message to the taskbar's status area. MSDN
	Shell_NotifyIcon(NIM_ADD, &m_nd);       //加入图标
}

//删除系统托盘图标
void CTrayIocn::RemoveIcon()
{
	if (m_nd.cbSize>0)
	{
		Shell_NotifyIcon(NIM_DELETE, &m_nd);
		memset((char*)&m_nd,0,sizeof(NOTIFYICONDATA));
	}
}

//添加WinProc消息函数
LRESULT CTrayIocn::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	UINT msg=m_nd.uCallbackMessage;
	// TODO: Add your specialized code here and/or call the base class
	if(message == msg)
	{
 
		//如果是用户定义的消息 
		if(lParam==WM_LBUTTONDOWN)
		{
			//添加自己的函数

			//鼠标单击图标时主窗口出现 
			//RemoveIcon();			 //删除系统图标
			//IsWindowVisible(m_parentWnd);
			if (IsWindowVisible(m_parentWnd))
			{
				ShowWindow(m_parentWnd,SW_HIDE);    //显示窗口
			}
			else
			{
				ShowWindow(m_parentWnd,SW_SHOWNA);    //显示窗口
				SetForegroundWindow(m_parentWnd); 
			}
		} 
		else if(lParam==WM_RBUTTONDOWN)
		{
			//添加自己的函数

			//鼠标右键单击弹出选单 
			/*
			CMenu menu; 
			menu.LoadMenu(IDR_MY_MENU); //载入事先定义的选单 
			CMenu*pMenu=menu.GetSubMenu(0); 
			CPoint pos; 
			GetCursorPos(&pos);
			pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,pos.x,pos.y,AfxGetMainWnd());
			*/
		}
 
	}  
	return 1;//CWnd::WindowProc(message, wParam, lParam);
}