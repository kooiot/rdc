
// ServerCPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerCP.h"
#include "ServerCPDlg.h"
#include "afxdialogex.h"
#include <koo_process.h>
#include <zmq.h>
#include <cassert>
#include <json.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CServerCPDlg 对话框



CServerCPDlg::CServerCPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVERCP_DIALOG, pParent),
	m_pProcess(NULL),
	m_pMapperProcess(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Users = new UserInfo[2048];
	m_Devices = new DeviceInfo[2048];
}

CServerCPDlg::~CServerCPDlg()
{
	delete[] m_Users;
	delete[] m_Devices;
}

void CServerCPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ONLINE_DEVCIES, m_listDevices);
	DDX_Control(pDX, IDC_LIST_ONLINE_USERS, m_listUsers);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
	DDX_Control(pDX, IDC_EDIT_INFO, m_editServerInfo);
}

BEGIN_MESSAGE_MAP(CServerCPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER_EVENT, &CServerCPDlg::OnEventMsg)
	ON_MESSAGE(WM_USER_DATA, &CServerCPDlg::OnDataMsg)
	ON_BN_CLICKED(IDC_BUTTON_START, &CServerCPDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CServerCPDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_USERS, &CServerCPDlg::OnBnClickedButtonUsers)
	ON_BN_CLICKED(IDC_BUTTON_DEVS, &CServerCPDlg::OnBnClickedButtonDevs)
	ON_BN_CLICKED(IDC_BUTTON_RD, &CServerCPDlg::OnBnClickedButtonRd)
	ON_BN_CLICKED(IDC_BUTTON_RUSERS, &CServerCPDlg::OnBnClickedButtonRusers)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CServerCPDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CServerCPDlg 消息处理程序

bool CServerCPDlg::OnData(int channel, void * data, size_t len)
{
	std::string* p = new std::string((char*)data, len);
	this->PostMessage(WM_USER_DATA, (WPARAM)channel, (LPARAM)p);
	return true;
}

bool CServerCPDlg::OnEvent(int channel, StreamEvent event, const char* msg)
{
	this->SendMessage(WM_USER_DATA, (WPARAM)event, (LPARAM)msg);
	return true;
}

BOOL CServerCPDlg::OnInitDialog()
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

	m_TabCtrl.InsertItem(0, "Online Devices");
	m_TabCtrl.InsertItem(1, "Online Users");

	// TODO: 在此添加额外的初始化代码
	m_listDevices.InsertColumn(0, "Name", LVCFMT_LEFT, 120);
	m_listDevices.InsertColumn(1, "SN", LVCFMT_LEFT, 240);
	m_listDevices.InsertColumn(2, "Desc", LVCFMT_LEFT, 240);
	m_listDevices.SetExtendedStyle(m_listDevices.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_listDevices.ShowWindow(SW_SHOW);

	m_listUsers.InsertColumn(0, "ID", LVCFMT_LEFT, 120);
	m_listUsers.InsertColumn(1, "Name", LVCFMT_LEFT, 240);
	m_listUsers.InsertColumn(2, "Desc", LVCFMT_LEFT, 240);
	m_listUsers.SetExtendedStyle(m_listUsers.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_listUsers.ShowWindow(SW_HIDE);

	RECT rect;
	m_TabCtrl.GetClientRect(&rect);
	rect.top += 25;
	rect.left += 1;
	rect.right -= 3;
	rect.bottom -= 1;
	m_listDevices.SetParent(&m_TabCtrl);
	m_listUsers.SetParent(&m_TabCtrl);
	m_listDevices.MoveWindow(&rect);
	m_listUsers.MoveWindow(&rect);

	m_CTX = zmq_ctx_new();

#ifndef _DEBUG
	m_ServerIP = "123.57.13.218";
#else
	m_ServerIP = "127.0.0.1";
#endif
	m_ServerRepPort = 6600;
	m_ServerPubPort = 6601;
	m_ServerStreamPort = 6602;
	m_StreamServerPort = 6800;

	CString info;
	info.Format("IP:%s\r\nRep:%d\r\nPub:%d\r\nStream:%d\r\nSSID:%d\r\nSSPort:%d",
		m_ServerIP.c_str(),
		m_ServerRepPort,
		m_ServerPubPort,
		m_ServerStreamPort,
		RC_STREAM_SERVER_ID_BASE,
		m_StreamServerPort);
	m_editServerInfo.SetWindowText(info);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServerCPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerCPDlg::OnPaint()
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

void CServerCPDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_pMapperProcess)
		m_pMapperProcess->stop();
	if (m_pProcess) {
		m_pProcess->stop();
		k_kill_process("StreamServer.exe");
	}
	CDialogEx::OnClose();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CServerCPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CServerCPDlg::OnEventMsg(WPARAM wParam, LPARAM lParam)
{
	StreamEvent se = (StreamEvent)wParam;
	const char* msg = (char*)lParam;

	return 0L;
}

LRESULT CServerCPDlg::OnDataMsg(WPARAM wParam, LPARAM lParam)
{
	int channel = (int)wParam;
	std::string* p = (std::string*)lParam;

	delete p;
	return 0L;
}

void CServerCPDlg::OnBnClickedButtonStart()
{
	if (m_pProcess)
	{
		if (m_pMapperProcess)
			m_pMapperProcess->stop();
		if (m_pStreamProcess)
			m_pStreamProcess->stop();
		if (m_pProcess)
			m_pProcess->stop();
		//k_kill_process("StreamServer.exe");
		delete m_pMapperProcess;
		m_pMapperProcess = NULL;
		delete m_pStreamProcess;
		m_pStreamProcess = NULL;
		delete m_pProcess;
		m_pProcess = NULL;
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("Start");
	}
	else
	{
		std::stringstream acc;
		acc << m_ServerIP << " " << m_ServerRepPort << " " << m_ServerPubPort << " " << m_ServerStreamPort;
		m_pProcess = new koo_process("AccServer", "", "AccServer.exe", acc.str(), true);
		m_pProcess->start();

		std::stringstream spp;
		spp << RC_STREAM_SERVER_ID_BASE << " " << m_ServerIP << " " << m_ServerStreamPort << " " << m_ServerIP << " " << m_StreamServerPort;
		m_pStreamProcess = new koo_process("StreamServer", "", "StreamServer.exe", spp.str(), true);
		m_pStreamProcess->start();

		std::stringstream mpp;
		mpp << "\"4C05D6F6-92EA-4a23-8EFF-179F91CBAA6A\"" << " " << m_ServerIP << " " << m_ServerRepPort << " " << m_ServerPubPort;
		m_pMapperProcess = new koo_process("Mapper", "", "Mapper.exe", mpp.str(), true);
		m_pMapperProcess->start();
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("Stop");
	}
}

void CServerCPDlg::OnBnClickedButtonConnect()
{
	if (m_pAccApi) {
		if (m_pStreamApi)
			m_pStreamApi->Disconnect();
		delete m_pStreamApi;
		m_pStreamApi = NULL;
		if (m_pAccApi)
			m_pAccApi->Disconnect();
		delete m_pAccApi;
		m_pAccApi = NULL;
		GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowText("Connect");
	}
	else {
		// TODO: 在此添加控件通知处理程序代码
		m_pAccApi = new CAccApi(m_CTX);
		bool br = m_pAccApi->Connect(m_ServerIP.c_str(), m_ServerRepPort, "admin", "admin");
		if (br) {
			MessageBox("Connected");
			StreamProcess sp;
			int rc = m_pAccApi->GetStreamServer(&sp);
			if (rc == 0) {
				m_pStreamApi = new CStreamApi(*this, CLIENT_TYPE, sp.Index, sp.Mask);
				br = m_pStreamApi->Connect(sp.StreamIP, sp.Port);
				if (br) {
					MessageBox("Stream Connected");
					rc = m_pStreamApi->SendData(0, (unsigned char *)"Hello World", strlen("Hello World"));
					assert(rc >= 0);
				}
				else {
					MessageBox("Stream Faied");
				}
			}
		}
		else {
			MessageBox("Faied");
		}
		GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowText("Disconnect");
	}
}

void CServerCPDlg::OnBnClickedButtonUsers()
{
	CUsersDlg dlg;
	dlg.m_pAccApi = m_pAccApi;
	dlg.DoModal();
}


void CServerCPDlg::OnBnClickedButtonDevs()
{
	CDevicesDlg dlg;
	dlg.m_pAccApi = m_pAccApi;
	dlg.DoModal();
}


void CServerCPDlg::OnBnClickedButtonRd()
{
	m_listDevices.DeleteAllItems();
	memset(m_Devices, 0, sizeof(DeviceInfo) * 2048);
	int num = m_pAccApi->ListDevices(m_Devices, 2048, true);

	for (int i = 0; i < num; ++i) {
		int n = m_listDevices.InsertItem(i, m_Devices[i].Name);
		m_listDevices.SetItemText(n, 1, m_Devices[i].SN);
		m_listDevices.SetItemText(n, 2, m_Devices[i].Desc);
	}
}


void CServerCPDlg::OnBnClickedButtonRusers()
{
	m_listUsers.DeleteAllItems();
	memset(m_Users, 0, sizeof(UserInfo) * 2048);
	int num = m_pAccApi->ListUsers(m_Users, 2048, true);

	for (int i = 0; i < num; ++i) {
		int n = m_listUsers.InsertItem(i, m_Users[i].ID);
		m_listUsers.SetItemText(n, 1, m_Users[i].Name);
		m_listUsers.SetItemText(n, 2, m_Users[i].Desc);
	}
}



void CServerCPDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int CurSel = m_TabCtrl.GetCurSel();

	m_listDevices.ShowWindow(SW_HIDE);
	m_listUsers.ShowWindow(SW_HIDE);
	if (CurSel == 0) {
		m_listDevices.ShowWindow(SW_SHOW);
	}
	if (CurSel == 1) {
		m_listUsers.ShowWindow(SW_SHOW);
	}
	*pResult = 0;
}
