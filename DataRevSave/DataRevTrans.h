
// DataRevTrans.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDataRevTransApp:
// �йش����ʵ�֣������ DataRevTrans.cpp
//

class CDataRevTransApp : public CWinApp
{
public:
	CDataRevTransApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDataRevTransApp theApp;