#pragma once
#include "afxwin.h"
#include <list>
#include <string>

// CPluginDlg �Ի���

class CPluginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPluginDlg)

public:
	CPluginDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPluginDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLUGIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_cbList;
	CEdit m_editConfig;
	std::list<std::string> m_List;
	PluginInfo m_Info;
	PluginInfo m_LocalInfo;
	virtual void OnOK();
	afx_msg void OnBnClickedButtonSlrm();
	CEdit m_edtConfigLocal;
};
