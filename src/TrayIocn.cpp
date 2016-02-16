// TrayIocn.cpp : ʵ���ļ�
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
 
  
// CTrayIocn ��Ϣ�������

/***************************************
* ��ͼ�����ϵͳ����
* ����:
* hWnd - ������
* hIcon  - ��Ҫ�����ͼ����
* title     - ��ͣ��ϵͳ����֮��ʱ��Ҫ��ʾ����Ϣ
* uCallbackMessage - ��Ϣ��ʶ(���û��Լ�������Ϣ)
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
	Shell_NotifyIcon(NIM_ADD, &m_nd);       //����ͼ��
}

//ɾ��ϵͳ����ͼ��
void CTrayIocn::RemoveIcon()
{
	if (m_nd.cbSize>0)
	{
		Shell_NotifyIcon(NIM_DELETE, &m_nd);
		memset((char*)&m_nd,0,sizeof(NOTIFYICONDATA));
	}
}

//���WinProc��Ϣ����
LRESULT CTrayIocn::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	UINT msg=m_nd.uCallbackMessage;
	// TODO: Add your specialized code here and/or call the base class
	if(message == msg)
	{
 
		//������û��������Ϣ 
		if(lParam==WM_LBUTTONDOWN)
		{
			//����Լ��ĺ���

			//��굥��ͼ��ʱ�����ڳ��� 
			//RemoveIcon();			 //ɾ��ϵͳͼ��
			//IsWindowVisible(m_parentWnd);
			if (IsWindowVisible(m_parentWnd))
			{
				ShowWindow(m_parentWnd,SW_HIDE);    //��ʾ����
			}
			else
			{
				ShowWindow(m_parentWnd,SW_SHOWNA);    //��ʾ����
				SetForegroundWindow(m_parentWnd); 
			}
		} 
		else if(lParam==WM_RBUTTONDOWN)
		{
			//����Լ��ĺ���

			//����Ҽ���������ѡ�� 
			/*
			CMenu menu; 
			menu.LoadMenu(IDR_MY_MENU); //�������ȶ����ѡ�� 
			CMenu*pMenu=menu.GetSubMenu(0); 
			CPoint pos; 
			GetCursorPos(&pos);
			pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,pos.x,pos.y,AfxGetMainWnd());
			*/
		}
 
	}  
	return 1;//CWnd::WindowProc(message, wParam, lParam);
}