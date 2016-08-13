#pragma once
#include "afxwin.h"
#include <list>
#include <string>

// CPluginDlg 对话框

class CPluginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPluginDlg)

public:
	CPluginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPluginDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLUGIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
