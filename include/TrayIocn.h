#pragma once
#include <config.h>
#include <afx.h>
#include <afxwin.h>
//  

/************************************************************************/
/* 
  
	CTrayIocn 类. 托盘图标类.(shell32.dll)

	使用方法
	1.类里声明成员	CTrayIocn m_trayiocn;

	2.在需要添加托盘图标的地方
		ShowWindow(SW_HIDE);    //隐藏dlg窗口(可以不要)
		m_trayiocn.AddIcon(m_hWnd,m_hIcon,WM_USER+1234,"我这是标题");        //添加系统图标

	3.需要给托盘图标类提供事件消息.即在窗口消息循环里让m_trayiocn.WindowProc(message,wParam,lParam); 接收msg

	//添加WinProc消息函数
	LRESULT CiconDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
	{
		//托盘图标类接收/处理消息
		m_trayiocn.WindowProc(message,wParam,lParam);
		return CDialog::WindowProc(message, wParam, lParam);
	}

	4.需要删除托盘图标的地方,默认的CTrayIocn类实现了点击托盘图标后删除图标的处理办法.
	//鼠标单击图标时主窗口出现 
	m_trayiocn.RemoveIcon();			 //删除系统图标
	ShowWindow(hWnd,SW_SHOW);    //显示dlg窗口(可以不要)


	当然还有点没解决的.如果托盘图标显示时,把explorer进程杀了,再启动,托盘图标就没了..需要自己搞个检测.判断图标还显示没,没显示则再添加.

*/
/************************************************************************/

class ENGERLIB_EXP CTrayIocn 
{ 

	NOTIFYICONDATA m_nd; //NOTIFYICONDATA结构
public:
	CTrayIocn();
	virtual ~CTrayIocn();	 
	void AddIcon(HWND hWnd, HICON hIcon, UINT uCallbackMessage,const TCHAR *title);
	void RemoveIcon();
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
protected: 
	HWND m_parentWnd;
};


