
// ServerCPDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class koo_process;

#define WM_USER_DATA WM_USER + 100
#define WM_USER_EVENT WM_USER + 100

// CServerCPDlg �Ի���
class CServerCPDlg : public CDialogEx, IStreamHandler
{
// ����
public:
	CServerCPDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CServerCPDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVERCP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
#ifdef CONSOLE_RUN
	std::string m_ServerIP;
	int m_ServerRepPort;
	int m_ServerPubPort;
	int m_ServerStreamPort;
	int m_StreamServerPort;

	koo_process* m_pProcess;
	koo_process* m_pStreamProcess;
	koo_process* m_pMapperProcess;
#endif
	CAccApi* m_pAccApi;
	CStreamApi* m_pStreamApi;
	void* m_CTX;
	UserInfo* m_Users;
	DeviceInfo* m_Devices;

	virtual bool OnData(int channel, void * data, size_t len);
	virtual bool OnEvent(int channel, StreamEvent event, const char* msg);

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnEventMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDataMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonUsers();
	afx_msg void OnBnClickedButtonDevs();
	afx_msg void OnBnClickedButtonRd();
	afx_msg void OnBnClickedButtonRusers();
private:
	CListCtrl m_listDevices;
	CListCtrl m_listUsers;
public:
	CTabCtrl m_TabCtrl;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_editServerInfo;
	afx_msg void OnBnClickedButtonSm();
};
