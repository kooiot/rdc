
// RemoteConnector.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRemoteConnectorApp: 
// �йش����ʵ�֣������ RemoteConnector.cpp
//

class CRemoteConnectorApp : public CWinApp
{
public:
	CRemoteConnectorApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRemoteConnectorApp theApp;