#pragma once


// CDlgModelDetail �Ի���
#include <ProManager.h>
#include "afxwin.h"
class CDlgModelDetail : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgModelDetail)

public:
	CDlgModelDetail(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgModelDetail();

// �Ի�������
	enum { IDD = IDD_DIALOG_MODEL_DETAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CProManager* m_pProManager;
	string		m_strModelName;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	void UpdataModelState();
	CEdit m_editState;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
