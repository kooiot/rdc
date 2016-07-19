#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CUDPDlg 对话框

class CUDPDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUDPDlg)

public:
	CUDPDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUDPDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TCP_DIALOG };
#endif
	UDPInfo m_Info;
	UDPInfo m_LocalInfo;
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
