#pragma once
#include "afxpropertygridctrl.h"


// CNewChannelDlg 对话框

class CNewChannelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewChannelDlg)

public:
	CNewChannelDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNewChannelDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEW_CHANEL_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	CMFCPropertyGridCtrl m_wndPropList;
	CFont m_fntPropList;

	void InitPropList();
	void SetPropListFont();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
