#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CTCPDlg 对话框

class CTCPDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTCPDlg)

public:
	CTCPDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTCPDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TCP_DIALOG };
#endif
	TCPClientInfo m_Info;
	TCPServerInfo m_LocalInfo;
	bool m_Editable;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:
	CIPAddressCtrl m_ipDevR;
	CIPAddressCtrl m_ipDevL;
	CIPAddressCtrl m_ipLocalR;
	CIPAddressCtrl m_ipLocalL;
	CEdit m_portDevR;
	CEdit m_portDevL;
	CEdit m_portLocalR;
	CEdit m_portLocalL;
	CButton m_checkDevBind;
	CButton m_checkRemotetLimit;

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedCheckDevBind();
	afx_msg void OnBnClickedCheckLocalBind();
};
