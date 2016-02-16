#pragma once
#include <config.h>
#include <afx.h>
#include <afxwin.h>
//  

/************************************************************************/
/* 
  
	CTrayIocn ��. ����ͼ����.(shell32.dll)

	ʹ�÷���
	1.����������Ա	CTrayIocn m_trayiocn;

	2.����Ҫ�������ͼ��ĵط�
		ShowWindow(SW_HIDE);    //����dlg����(���Բ�Ҫ)
		m_trayiocn.AddIcon(m_hWnd,m_hIcon,WM_USER+1234,"�����Ǳ���");        //���ϵͳͼ��

	3.��Ҫ������ͼ�����ṩ�¼���Ϣ.���ڴ�����Ϣѭ������m_trayiocn.WindowProc(message,wParam,lParam); ����msg

	//���WinProc��Ϣ����
	LRESULT CiconDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
	{
		//����ͼ�������/������Ϣ
		m_trayiocn.WindowProc(message,wParam,lParam);
		return CDialog::WindowProc(message, wParam, lParam);
	}

	4.��Ҫɾ������ͼ��ĵط�,Ĭ�ϵ�CTrayIocn��ʵ���˵������ͼ���ɾ��ͼ��Ĵ���취.
	//��굥��ͼ��ʱ�����ڳ��� 
	m_trayiocn.RemoveIcon();			 //ɾ��ϵͳͼ��
	ShowWindow(hWnd,SW_SHOW);    //��ʾdlg����(���Բ�Ҫ)


	��Ȼ���е�û�����.�������ͼ����ʾʱ,��explorer����ɱ��,������,����ͼ���û��..��Ҫ�Լ�������.�ж�ͼ�껹��ʾû,û��ʾ�������.

*/
/************************************************************************/

class ENGERLIB_EXP CTrayIocn 
{ 

	NOTIFYICONDATA m_nd; //NOTIFYICONDATA�ṹ
public:
	CTrayIocn();
	virtual ~CTrayIocn();	 
	void AddIcon(HWND hWnd, HICON hIcon, UINT uCallbackMessage,const TCHAR *title);
	void RemoveIcon();
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
protected: 
	HWND m_parentWnd;
};


