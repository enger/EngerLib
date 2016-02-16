#pragma once
#include "afxcmn.h"


// CDlgCityState �Ի���
class CProManager;
class CDlgCityState : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCityState)

public:
	CDlgCityState(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCityState();

// �Ի�������
	enum { IDD = IDD_DIALOG_OTHER_CITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listCityState;
	CProManager* m_pProManager;
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnOK();
	void UpdataCityState(void);
	virtual BOOL OnInitDialog();
};
