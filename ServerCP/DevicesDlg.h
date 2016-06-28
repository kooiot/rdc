#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxdtctl.h"


// CDevicesDlg �Ի���

class CDevicesDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDevicesDlg)

public:
	CDevicesDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDevicesDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVICES_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	void BindUser(int nCur);
public:
	CAccApi* m_pAccApi;
	DeviceInfo* m_Devs;

	virtual BOOL OnInitDialog();
	CListCtrl m_listDevs;
	CEdit m_editSN;
	CEdit m_editName;
	CEdit m_editDesc;
	CEdit m_editCreateTime;
	CDateTimeCtrl m_dtValid;
};
