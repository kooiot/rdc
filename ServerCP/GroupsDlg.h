#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxdtctl.h"


// CGroupsDlg 对话框

class CGroupsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGroupsDlg)

public:
	CGroupsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGroupsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GROUPS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CAccMgrApi* m_pAccApi;
	GroupInfo* m_Groups;
	DeviceInfo* m_Devs;
	int* m_GroupDevices;
	int m_CurSel;

private:
	CListCtrl m_listGroups;
	CEdit m_editName;
	CEdit m_editDesc;
	CEdit m_editCreateTime;
	CDateTimeCtrl m_dtValid;
	CButton m_chkValid;
	CListCtrl m_listDevices;

public:
	virtual BOOL OnInitDialog();
	void BindGroup(int nCur, bool bEdit);
	void DumpGroup(int nCur);

	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedCheckValid();
	afx_msg void OnNMDblclkListGroups(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListGroups(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListDevices(NMHDR *pNMHDR, LRESULT *pResult);
};
