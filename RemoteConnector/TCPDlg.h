#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CTCPDlg �Ի���

class CTCPDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTCPDlg)

public:
	CTCPDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTCPDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TCP_DIALOG };
#endif
	TCPClientInfo m_Info;
	TCPServerInfo m_LocalInfo;
	bool m_Editable;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
