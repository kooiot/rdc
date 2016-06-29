
// RemoteConnectorDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CRemoteConnectorDlg 对话框
class CRemoteConnectorDlg : public CDialogEx
{
// 构造
public:
	CRemoteConnectorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTECONNECTOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	RC_HANDLE m_hApi;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonListdev();
	CEdit m_editIP;
	CEdit m_editPort;
	CListCtrl m_listDevs;
};
