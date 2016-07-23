#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxdtctl.h"


// CDevicesDlg 对话框

class CDevicesDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDevicesDlg)

public:
	CDevicesDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDevicesDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVICES_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CAccApi* m_pAccApi;
	DeviceInfo* m_Devs;
	int m_CurSel;

private:
	CListCtrl m_listDevs;
	CEdit m_editSN;
	CEdit m_editName;
	CEdit m_editDesc;
	CEdit m_editCreateTime;
	CDateTimeCtrl m_dtValid;
	CButton m_chkValid;

public:
	virtual BOOL OnInitDialog();
	void BindDevice(int nCur, bool bEdit);
	void DumpDevice(int nCur);

	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedCheckValid();
	afx_msg void OnNMDblclkListDevices(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListDevices(NMHDR *pNMHDR, LRESULT *pResult);
};
