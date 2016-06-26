
// ServerCPDlg.h : 头文件
//

#pragma once

class koo_process;

#define WM_USER_DATA WM_USER + 100
#define WM_USER_EVENT WM_USER + 100

// CServerCPDlg 对话框
class CServerCPDlg : public CDialogEx, IStreamHandler
{
// 构造
public:
	CServerCPDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVERCP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	koo_process* m_pProcess;
	koo_process* m_pMapperProcess;
	CAccApi* m_pAccApi;
	CStreamApi* m_pStreamApi;
	void* m_CTX;

	virtual bool OnData(int channel, const unsigned char* data, size_t len);
	virtual bool OnEvent(StreamEvent event);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HRESULT OnEventMsg(WPARAM wParam, LPARAM lParam);
	afx_msg HRESULT OnDataMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonSend();
};
