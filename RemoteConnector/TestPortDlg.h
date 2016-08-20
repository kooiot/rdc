#pragma once
#include <Handlers.h>
#include "afxwin.h"

// CTestPortDlg �Ի���

class CTestPortDlg : public CDialogEx, public IPort
{
	DECLARE_DYNAMIC(CTestPortDlg)

public:
	CTestPortDlg(RC_CHANNEL channel,
		IPortHandler* Handler,
		CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTestPortDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TP_DIALOG };
#endif

protected:
	virtual bool Open() {
		return true;
	}
	virtual void Close() { return; }

	virtual int Write(void* buf, size_t len);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnShowRecvData(WPARAM wParam, LPARAM lParam);
	LRESULT OnShowRecvEvent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonSend();
	CEdit m_editRecv;
	CEdit m_editSend;
};
