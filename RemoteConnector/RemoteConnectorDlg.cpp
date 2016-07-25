
// RemoteConnectorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RemoteConnector.h"
#include "RemoteConnectorDlg.h"
#include "afxdialogex.h"
#include "SerialDlg.h"
#include "TCPDlg.h"
#include "UDPDlg.h"
#include "UVTcpServer.h"
#include "UVUdp.h"
#include <cassert>
#include "PluginPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


std::string GetModuleFilePath()
{
	char szFile[MAX_PATH] = { 0 };
	int dwRet = ::GetModuleFileName(NULL, szFile, 255);
	if (dwRet != 0)
	{
		printf("Module File Name: %s \n", szFile);
		std::string str = szFile;
		size_t nPos = str.rfind('\\');
		if (nPos != std::string::npos)
		{
			str = str.substr(0, nPos);
		}
		return str;
	}
	return "";
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CRemoteConnectorDlg 对话框



CRemoteConnectorDlg::CRemoteConnectorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_REMOTECONNECTOR_DIALOG, pParent)
{
	m_Devices = new DeviceInfo[RC_MAX_ONLINE_DEVICE];
	memset(m_Devices, 0, sizeof(DeviceInfo) * RC_MAX_ONLINE_DEVICE);
	memset(m_ConnectionInfos, 0, sizeof(ConnectionInfo*) * RC_MAX_CONNECTION);
	memset(m_LocalConnectionInfos, 0, sizeof(ConnectionInfo*) * RC_MAX_CONNECTION);
	memset(m_StreamPorts, 0, sizeof(IStreamHandler*) * RC_MAX_CONNECTION);
	m_bStop = false;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CRemoteConnectorDlg::~CRemoteConnectorDlg()
{
	delete[] m_Devices;
}

void CRemoteConnectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IP, m_editIP);
	DDX_Control(pDX, IDC_EDIT_PORT, m_editPort);
	DDX_Control(pDX, IDC_LIST_DEV, m_listDevs);
	DDX_Control(pDX, IDC_LIST_CONNECTIONS, m_listConnections);
	DDX_Control(pDX, IDC_EDIT_USER, m_editUser);
	DDX_Control(pDX, IDC_EDIT_PASSWD, m_editPasswd);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_btnConnect);
	DDX_Control(pDX, IDC_BUTTON_DISCONNECT, m_btnDisconnect);
	DDX_Control(pDX, IDC_BUTTON_LISTDEV, m_btnListDevs);
}

BEGIN_MESSAGE_MAP(CRemoteConnectorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CRemoteConnectorDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CRemoteConnectorDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_LISTDEV, &CRemoteConnectorDlg::OnBnClickedButtonListdev)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CRemoteConnectorDlg::OnBnClickedButtonAdd)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CONNECTIONS, &CRemoteConnectorDlg::OnNMDblclkListConnections)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CRemoteConnectorDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_ADD_SERIAL, &CRemoteConnectorDlg::OnBnClickedButtonAddSerial)
	ON_BN_CLICKED(IDC_BUTTON_ADD_TCP, &CRemoteConnectorDlg::OnBnClickedButtonAddTcp)
	ON_BN_CLICKED(IDC_BUTTON_ADD_UDP, &CRemoteConnectorDlg::OnBnClickedButtonAddUdp)
END_MESSAGE_MAP()


// CRemoteConnectorDlg 消息处理程序

BOOL CRemoteConnectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
#ifdef _DEBUG
	m_editIP.SetWindowText("127.0.0.1");
#else
	m_editIP.SetWindowText("123.57.13.218");
#endif
	m_editPort.SetWindowText("6600");
	m_editUser.SetWindowText("test");
	m_editPasswd.SetWindowText("test");

	m_listDevs.InsertColumn(0, "Name", LVCFMT_LEFT, 120);
	m_listDevs.InsertColumn(1, "SN", LVCFMT_LEFT, 240);
	m_listDevs.InsertColumn(2, "Desc", LVCFMT_LEFT, 240);
	m_listDevs.SetExtendedStyle(m_listDevs.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	m_listConnections.InsertColumn(0, "RT", LVCFMT_LEFT, 60);
	m_listConnections.InsertColumn(0, "LT", LVCFMT_LEFT, 60);
	m_listConnections.InsertColumn(1, "DevSN", LVCFMT_LEFT, 360);
	m_listConnections.SetExtendedStyle(m_listConnections.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	
	m_btnConnect.EnableWindow(TRUE);
	m_btnDisconnect.EnableWindow(FALSE);
	m_btnListDevs.EnableWindow(FALSE);

	std::string plugin_folder = GetModuleFilePath() + "\\plugins";
	m_PluginLoader.Load(plugin_folder.c_str());

	m_VSPortMgr.Init();
	RC_Init();

	m_UVLoop = uv_default_loop();
	m_pThread = new std::thread([this]() {
		while (!m_bStop) {
			uv_run(m_UVLoop, UV_RUN_ONCE);
		}
	});

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRemoteConnectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRemoteConnectorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CRemoteConnectorDlg::OnClose()
{
	m_bStop = true;
	if (m_pThread && m_pThread->joinable()) {
		m_pThread->join();
	}

	m_VSPortMgr.Close();

	if (m_hApi != NULL)
		RC_Disconnect(m_hApi);
	RC_Close();
	CDialogEx::OnClose();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRemoteConnectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRemoteConnectorDlg::StreamDataCallback(RC_CHANNEL channel, void * buf, size_t len, void * prv)
{
	CRemoteConnectorDlg* pThis = (CRemoteConnectorDlg*)prv;
	if (!pThis)
		return;
	pThis->__StreamDataCallback(channel, buf, len);
}

void CRemoteConnectorDlg::__StreamDataCallback(RC_CHANNEL channel, void * buf, size_t len)
{
	TRACE("[STREAM_DATA] [%d] len %d\n", channel, len);
	IStreamHandler* Handler = m_StreamPorts[channel];
	if (Handler) {
		Handler->OnData((void*)buf, len);
	}
}

void CRemoteConnectorDlg::StreamEventCallback(RC_CHANNEL channel, StreamEvent evt, const char* msg, void * prv)
{
	CRemoteConnectorDlg* pThis = (CRemoteConnectorDlg*)prv;
	if (!pThis)
		return;
	pThis->__StreamEventCallback(channel, evt, msg);
}

const static char* StreamEventNames[] = {
	"SE_CONNECT",
	"SE_DISCONNECT",
	"SE_CHANNEL_OPENED",
	"SE_CHANNEL_CLOSED",
	"SE_CHANNEL_NOT_SUPPORT",
	"SE_CHANNEL_OPEN_FAILED",
	"SE_CHANNEL_READ_ERROR",
	"SE_CLOSE",
	"SE_TIMEOUT",
};

void CRemoteConnectorDlg::__StreamEventCallback(RC_CHANNEL channel, StreamEvent evt, const char* msg)
{
	TRACE("[STREAM_EVENT] %s[%d] %s\n", StreamEventNames[evt], evt, msg);
	MessageBox(msg, StreamEventNames[evt]);
	if (evt == SE_CHANNEL_CLOSED 
		|| evt == SE_CHANNEL_NOT_SUPPORT
		|| evt == SE_CHANNEL_OPEN_FAILED) {

		int rc = RC_CloseChannel(m_hApi, channel);
		if (rc == 0) {
			RemoveConnection(channel);
		}
	}
}

int CRemoteConnectorDlg::OnLog(RC_CHANNEL channel, const char * type, const char * content)
{
	TRACE("[LOG] [%d]\t [%s] %s\n", channel, type, content);
	return 0;
}

int CRemoteConnectorDlg::Send(RC_CHANNEL channel, void * buf, size_t len)
{
	return RC_StreamSend(m_hApi, channel, buf, len);
}

const static char* CTNames[] = {
	"TEST",
	"SERIAL",
	"TCPC",
	"UDP",
	"TCPS",
	"PLUGIN",
	"",
};
void CRemoteConnectorDlg::AddConnection(ConnectionInfo * info, ConnectionInfo * bind)
{
	if (m_StreamPorts[info->Channel] != NULL) {
		MessageBox("EEE");
		delete info;
		delete bind;
		return;
	}
	IStreamHandler* pHandler = NULL;
	const char* LocalType = CTNames[info->Type];
	const char* RemoteType = CTNames[bind->Type];
	if (bind->Type == CT_SERIAL) {
		pHandler = m_VSPortMgr.CreatePort(info->Channel, *this, bind->Serial.dev);
	}
	if (bind->Type == CT_TEST) {
		auto dlg = new CTestPortDlg(info->Channel, *this);
		dlg->ShowWindow(SW_SHOW);
		pHandler = dlg;
	}
	if (bind->Type == CT_UDP) {
		pHandler = new UVUdp(m_UVLoop, info->Channel, *this, bind->UDP);
	}
	if (bind->Type == CT_TCPS) {
		pHandler = new UVTcpServer(m_UVLoop, info->Channel, *this, bind->TCPServer);
	}
	if (bind->Type == CT_PLUGIN) {
		auto api = m_PluginLoader.Find(bind->Plugin.Name);
		if (api)
			pHandler = new PluginPort(info->Channel, *this, bind->Plugin, api);
	}
	if (!pHandler || !pHandler->Open()) {
		delete info;
		delete bind;
		return;
	}

	m_ConnectionInfos[info->Channel] = info;
	m_LocalConnectionInfos[info->Channel] = bind;
	m_StreamPorts[info->Channel] = pHandler;

	int nIndex = m_listConnections.GetItemCount();
	nIndex = m_listConnections.InsertItem(nIndex, RemoteType);
	m_listConnections.SetItemText(nIndex, 1, LocalType);
	m_listConnections.SetItemText(nIndex, 2, info->DevSN);
	m_listConnections.SetItemData(nIndex, info->Channel);
}

void CRemoteConnectorDlg::RemoveConnection(RC_CHANNEL channel)
{
	if (m_StreamPorts[channel] == NULL) {
		return;
	}
	IStreamHandler* pHandler = m_StreamPorts[channel];
	ConnectionInfo * info = m_ConnectionInfos[channel];
	ConnectionInfo * local_info = m_LocalConnectionInfos[channel];
	pHandler->Close();
	if (local_info->Type == CT_SERIAL) {
		m_VSPortMgr.FreePort((VSPort*)pHandler);
	}
	else {
		delete pHandler;
	}
	delete local_info;
	delete info;
	m_StreamPorts[channel] = NULL;
	m_ConnectionInfos[channel] = NULL;
	m_LocalConnectionInfos[channel] = NULL;
}

DeviceInfo* CRemoteConnectorDlg::GetSelDeviceInfo()
{
	if (m_hApi == NULL)
		return NULL;

	POSITION pos = m_listDevs.GetFirstSelectedItemPosition();
	if (pos == NULL) {
		MessageBox("选择一个设备先", "提示", MB_OK | MB_ICONASTERISK);
		return NULL;
	}
	int nItem = m_listDevs.GetNextSelectedItem(pos);
	if (nItem < 0 || nItem > 2048) {
		MessageBox("超出设备列表", "提示", MB_OK | MB_ICONERROR);
		return NULL;
	}

	DeviceInfo& info = m_Devices[nItem];
	if (strlen(info.SN) == 0) {
		MessageBox("没有该设备", "提示", MB_OK | MB_ICONERROR);
		return NULL;
	}
	return &info;
}

void CRemoteConnectorDlg::OnBnClickedButtonConnect()
{
	CString ip;
	m_editIP.GetWindowText(ip);
	CString port;
	m_editPort.GetWindowText(port);
	CString user;
	m_editUser.GetWindowText(user);
	CString passwd;
	m_editPasswd.GetWindowText(passwd);

	m_hApi = RC_Connect(ip, atoi(port), user, passwd);
	if (m_hApi != 0) {
		RC_SetStreamCallback(m_hApi, StreamDataCallback, StreamEventCallback, this);
		m_btnConnect.EnableWindow(FALSE);
		m_btnDisconnect.EnableWindow(TRUE);
		m_btnListDevs.EnableWindow(TRUE);
		OnBnClickedButtonListdev();
	}
}


void CRemoteConnectorDlg::OnBnClickedButtonDisconnect()
{
	// TODO: 在此添加控件通知处理程序代码
	RC_Disconnect(m_hApi);
	m_hApi = NULL;
	m_btnConnect.EnableWindow(TRUE);
	m_btnDisconnect.EnableWindow(FALSE);
	m_btnListDevs.EnableWindow(FALSE);
	m_listDevs.DeleteAllItems();
}


void CRemoteConnectorDlg::OnBnClickedButtonListdev()
{
	m_listDevs.DeleteAllItems();

	DeviceInfo* pInfo = m_Devices;
	memset(pInfo, 0, sizeof(DeviceInfo) * RC_MAX_ONLINE_DEVICE);
	int num = RC_ListDevices(m_hApi, pInfo, RC_MAX_ONLINE_DEVICE);
	if (num >= 0) {
		for (int i = 0; i < num; ++i) {
			int n = m_listDevs.InsertItem(i, pInfo[i].Name);
			m_listDevs.SetItemText(n, 1, pInfo[i].SN);
			m_listDevs.SetItemText(n, 2, pInfo[i].Desc);
		}
	}
	else {
		MessageBox("Failed");
	}
}



void CRemoteConnectorDlg::OnNMDblclkListConnections(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (-1 == pNMItemActivate->iItem)
		return;

	int channel = m_listConnections.GetItemData(pNMItemActivate->iItem);
	if (channel < 0 || channel > RC_MAX_CONNECTION)
		return;

	ConnectionInfo * info = m_ConnectionInfos[channel];
	ConnectionInfo * local_info = m_LocalConnectionInfos[channel];
	if (info->Type = CT_SERIAL) {
		CSerialDlg dlg;
		dlg.m_Info = info->Serial;
		dlg.m_LocalInfo = local_info->Serial;
		dlg.m_Editable = false;
		if (IDOK == dlg.DoModal()) {
			
		}
	}
	else if (info->Type = CT_TCPC) {
		CTCPDlg dlg;
		dlg.m_Info = info->TCPClient;
		dlg.m_LocalInfo = local_info->TCPServer;
		dlg.m_Editable = false;
		if (IDOK == dlg.DoModal()) {

		}
	}
	else if (info->Type = CT_UDP) {
		CUDPDlg dlg;
		dlg.m_Info = info->UDP;
		dlg.m_LocalInfo = local_info->UDP;
		dlg.m_Editable = false;
		if (IDOK == dlg.DoModal()) {

		}
	}
	else {
		CSerialDlg dlg;
		dlg.m_Info = info->Serial;
		dlg.m_LocalInfo = local_info->Serial;
		dlg.m_Editable = false;
		if (IDOK == dlg.DoModal()) {

		}
	}
	*pResult = 0;
}


void CRemoteConnectorDlg::OnBnClickedButtonDelete()
{
	POSITION pos = m_listConnections.GetFirstSelectedItemPosition();
	if (pos == NULL) {
		MessageBox("选择一个连接先", "提示", MB_OK | MB_ICONASTERISK);
		return;
	}
	int nItem = m_listConnections.GetNextSelectedItem(pos);
	if (nItem < 0 || nItem > RC_MAX_CONNECTION) {
		MessageBox("超出连接列表", "提示", MB_OK | MB_ICONERROR);
		return;
	}
	int nChannel = m_listConnections.GetItemData(nItem);

	IStreamHandler* pHandler = m_StreamPorts[nChannel];
	ConnectionInfo* pInfo = m_ConnectionInfos[nChannel];
	ConnectionInfo * local_info = m_LocalConnectionInfos[nChannel];
	if (!pInfo || !pHandler)
	{
		MessageBox("Connection already close!!", "提示", MB_OK | MB_ICONERROR);
		return;
	}
	assert(nChannel == pInfo->Channel);
	int rc = RC_CloseChannel(m_hApi, nChannel);
	if (rc == 0) {
		MessageBox("OK");
		RemoveConnection(nChannel);
		m_listConnections.DeleteItem(nItem);
	}
}


void CRemoteConnectorDlg::OnBnClickedButtonAdd()
{
	DeviceInfo *info = GetSelDeviceInfo();
	if (!info) {
		MessageBox("请选择要连接的设备", "错误", MB_OK | MB_ICONERROR);
		return;
	}

	RC_CHANNEL channel = RC_ConnectTest(m_hApi, info->SN);
	if (channel < 0) {
		MessageBox("创建测试连接失败", "错误", MB_OK | MB_ICONERROR);
		return;
	}

	assert(m_ConnectionInfos[channel] == NULL);
	ConnectionInfo* ci = new ConnectionInfo();
	ConnectionInfo* lci = new ConnectionInfo();
	ci->Type = CT_TEST;
	memcpy(ci->DevSN, info->SN, RC_MAX_SN_LEN);
	ci->Channel = channel;

	lci->Type = CT_TEST;
	memcpy(lci->DevSN, info->SN, RC_MAX_SN_LEN);
	lci->Channel = channel;
	AddConnection(ci, lci);
}

void CRemoteConnectorDlg::OnBnClickedButtonAddSerial()
{
	DeviceInfo *info = GetSelDeviceInfo();
	if (!info) {
		MessageBox("请选择要连接的设备", "错误", MB_OK | MB_ICONERROR);
		return;
	}

	CSerialDlg dlg;
	if (IDOK == dlg.DoModal()) {
		RC_CHANNEL channel = RC_ConnectSerial(m_hApi, info->SN, &dlg.m_Info);
		if (channel < 0) {
			MessageBox("创建串口连接失败", "错误", MB_OK | MB_ICONERROR);
			return;
		}
		assert(m_ConnectionInfos[channel] == NULL);
		ConnectionInfo* ci = new ConnectionInfo();
		ConnectionInfo* lci = new ConnectionInfo();
		ci->Type = CT_SERIAL;
		memcpy(ci->DevSN, info->SN, RC_MAX_SN_LEN);
		ci->Serial = dlg.m_Info;
		ci->Channel = channel;

		lci->Type = CT_SERIAL;
		memcpy(lci->DevSN, info->SN, RC_MAX_SN_LEN);
		lci->Serial = dlg.m_LocalInfo;
		AddConnection(ci, lci);
	}
}


void CRemoteConnectorDlg::OnBnClickedButtonAddTcp()
{
	DeviceInfo *info = GetSelDeviceInfo();
	if (!info) {
		MessageBox("请选择要连接的设备", "错误", MB_OK | MB_ICONERROR);
		return;
	}

	//static TCPClientInfo server_info;
	//static TCPClientInfo localInfo;
	//CTCPDlg dlg;
	//dlg.m_Info = server_info;
	//dlg.m_LocalInfo = localInfo;
	//dlg.DoModal();
	//server_info = dlg.m_Info;
	//localInfo = dlg.m_LocalInfo;

	CTCPDlg dlg;
	if (IDOK == dlg.DoModal()) {
		RC_CHANNEL channel = RC_ConnectTCPC(m_hApi, info->SN, &dlg.m_Info);
		if (channel < 0) {
			MessageBox("创建TCP连接失败", "错误", MB_OK | MB_ICONERROR);
			return;
		}
		assert(m_ConnectionInfos[channel] == NULL);
		ConnectionInfo* ci = new ConnectionInfo();
		ConnectionInfo* lci = new ConnectionInfo();
		ci->Type = CT_TCPC;
		memcpy(ci->DevSN, info->SN, RC_MAX_SN_LEN);
		ci->TCPClient = dlg.m_Info;
		ci->Channel = channel;

		lci->Type = CT_TCPS;
		memcpy(lci->DevSN, info->SN, RC_MAX_SN_LEN);
		lci->TCPServer = dlg.m_LocalInfo;
		AddConnection(ci, lci);
	}
}


void CRemoteConnectorDlg::OnBnClickedButtonAddUdp()
{
	DeviceInfo *info = GetSelDeviceInfo();
	if (!info) {
		MessageBox("请选择要连接的设备", "错误", MB_OK | MB_ICONERROR);
		return;
	}

	CUDPDlg dlg;
	if (IDOK == dlg.DoModal()) {
		RC_CHANNEL channel = RC_ConnectUDP(m_hApi, info->SN, &dlg.m_Info);
		if (channel < 0) {
			MessageBox("创建UDP连接失败", "错误", MB_OK | MB_ICONERROR);
			return;
		}
		assert(m_ConnectionInfos[channel] == NULL);
		ConnectionInfo* ci = new ConnectionInfo();
		ConnectionInfo* lci = new ConnectionInfo();
		ci->Type = CT_UDP;
		memcpy(ci->DevSN, info->SN, RC_MAX_SN_LEN);
		ci->UDP = dlg.m_Info;
		ci->Channel = channel;

		lci->Type = CT_UDP;
		memcpy(lci->DevSN, info->SN, RC_MAX_SN_LEN);
		lci->UDP = dlg.m_LocalInfo;
		AddConnection(ci, lci);
	}
}
