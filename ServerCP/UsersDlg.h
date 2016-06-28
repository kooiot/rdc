#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxdtctl.h"


// CUsersDlg �Ի���

class CUsersDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUsersDlg)

public:
	CUsersDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUsersDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USERS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	void BindUser(int nCur);
	void DumpUser(int nCur);
public:
	CAccApi* m_pAccApi;
	UserInfo* m_Users;
	int m_CurSel;
private:
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
public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonShowPass();
	afx_msg void OnNMDblclkListUsers(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_chkValid;
	afx_msg void OnBnClickedCheckValid();
	afx_msg void OnBnClickedButtonSave();
};