#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CServicesDlg 对话框

class CServicesDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CServicesDlg)

public:
	CServicesDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CServicesDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVICES_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CListCtrl m_listServices;
	CEdit m_editName;
	CEdit m_editDesc;
	CEdit m_editExec;
	CEdit m_editWorkDir;
	CComboBox m_cbMode;
	CEdit m_editArgs;

	int m_CurSel;

protected:
	void BindService(int nCur, bool bEdit);
	void DumpService(int nCur);

protected:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnLvnItemchangedListServices(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListServices(NMHDR *pNMHDR, LRESULT *pResult);
};
