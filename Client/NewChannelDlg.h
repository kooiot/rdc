#pragma once
#include "afxpropertygridctrl.h"


// CNewChannelDlg �Ի���

class CNewChannelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewChannelDlg)

public:
	CNewChannelDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNewChannelDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEW_CHANEL_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
