
// RemoteConnectorDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "VSPortMgr.h"
#include "TestPortDlg.h"
#include <vector>
#include <thread>
#include <map>
#include <uv.h>
#include <PluginLoader.h>

class IStreamHandler;
// CRemoteConnectorDlg 对话框
class CRemoteConnectorDlg : public CDialogEx, public IPortHandler
{
// 构造
public:
	CRemoteConnectorDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CRemoteConnectorDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTECONNECTOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	RC_HANDLE m_hApi;
	DeviceInfo* m_Devices;
	ConnectionInfo* m_ConnectionInfos[RC_MAX_CONNECTION];
	ConnectionInfo* m_LocalConnectionInfos[RC_MAX_CONNECTION];
	IStreamHandler* m_StreamPorts[RC_MAX_CONNECTION];
	VSPortMgr m_VSPortMgr;
	std::map<RC_HANDLE, CTestPortDlg*> m_TestPorts;
	uv_loop_t * m_UVLoop;
	std::thread * m_pThread;
	bool m_bStop;
	CPluginLoader m_PluginLoader;

	CEdit m_editIP;
	CEdit m_editPort;
	CListCtrl m_listDevs;
	CListCtrl m_listConnections;

	CEdit m_editUser;
	CEdit m_editPasswd;
	CButton m_btnConnect;
	CButton m_btnDisconnect;
	CButton m_btnListDevs;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	static void __stdcall StreamDataCallback(RC_CHANNEL channel, void * buf, size_t len, void* prv);
	void __stdcall __StreamDataCallback(RC_CHANNEL channel, void * buf, size_t len);
	static void __stdcall StreamEventCallback(RC_CHANNEL channel, StreamEvent evt, const char* msg, void* prv);
	void __stdcall __StreamEventCallback(RC_CHANNEL channel, StreamEvent evt, const char* msg);

	virtual int OnLog(RC_CHANNEL channel, const char* type, const char* content);
	virtual int Send(RC_CHANNEL channel, void* buf, size_t len);

	void AddConnection(ConnectionInfo* info, ConnectionInfo * bind);
	void RemoveConnection(RC_CHANNEL channel);

	DeviceInfo* GetSelDeviceInfo();

protected:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonListdev();
	afx_msg void OnNMDblclkListConnections(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonAddSerial();
	afx_msg void OnBnClickedButtonAddTcp();
	afx_msg void OnBnClickedButtonAddUdp();
};
