#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxdtctl.h"


// CUsersDlg 对话框

class CUsersDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUsersDlg)

public:
	CUsersDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUsersDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USERS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CAccMgrApi* m_pAccApi;
	UserInfo* m_Users;
	int m_CurSel;
protected:
	// User lists control
	CListCtrl m_listUsers;
	CEdit m_editID;
	CEdit m_editPasswd;
	CEdit m_editName;
	CEdit m_editDesc;
	CEdit m_editEmail;
	CEdit m_editPhone;
	CComboBox m_boxLevel;
	CEdit m_editCreateTime;
	CDateTimeCtrl m_dtValid;
	CButton m_chkValid;
protected:
	virtual BOOL OnInitDialog();
	void BindUser(int nCur, bool bEdit);
	void DumpUser(int nCur);

protected:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonShowPass();
	afx_msg void OnBnClickedCheckValid();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnNMDblclkListUsers(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListUsers(NMHDR *pNMHDR, LRESULT *pResult);
};
