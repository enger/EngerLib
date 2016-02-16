
// DataRevTransDlg.h : ͷ�ļ�
//

#pragma once
#include <TrayIocn.h>
#include <ProManager.h>

// CDataRevTransDlg �Ի���
class CDlgCityState;
class CDataRevTransDlg : public CDialogEx
{
// ����
public:
	CDataRevTransDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DATAREVTRANS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	string m_strModelName;
	CTime m_startTime;
	CTrayIocn m_trayiocn;
	CProManager m_proManager;
	shared_ptr<CProInfo> m_prtProinfo;
	CString m_szThisSerNumber;
	CDlgCityState* m_pDlgCityState;
protected:
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnRestartExplorer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
};
