#pragma once

#include <TrayIocn.h>
#include <ProManager.h>
#include "afxcmn.h"
#include "afxwin.h"
#include <ProManager.h>
#include <map>
#include "DlgModelDetail.h"
// CDlgModelManage 对话框

class CDlgModelManage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgModelManage)

public:
	CDlgModelManage(CWnd* pParent = NULL);   // 标准构造函数

// 对话框数据
	enum { IDD = IDD_DIALOG_MODEL_MANAGE };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

protected:
	HICON m_hIcon;
	CListCtrl m_listModelState;
	CButton m_btUpdate;
	CButton m_btStopResume;

	CTrayIocn m_trayiocn;
	string m_strModelName;
	CProManager m_ProManager;

	map<string,shared_ptr<CDlgModelDetail>> m_mapDlgModelDetail;
protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg LRESULT OnRestartExplorer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnBnClickedButtonStopResume();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();

public:
	void UpdataModelState();
	afx_msg void OnNMRClickListModelState(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextStart();
	afx_msg void OnContextStop();
	afx_msg void OnNMDblclkListModelState(NMHDR *pNMHDR, LRESULT *pResult);
};
