
// ServerCPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerCP.h"
#include "ServerCPDlg.h"
#include "afxdialogex.h"
#include <koo_process.h>
#include <zmq.h>

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
	m_pProcess(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerCPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CServerCPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER_EVENT, &CServerCPDlg::OnEventMsg)
	ON_MESSAGE(WM_USER_DATA, &CServerCPDlg::OnDataMsg)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CServerCPDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CServerCPDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


// CServerCPDlg 消息处理程序

bool CServerCPDlg::OnData(int channel, const unsigned char * data, size_t len)
{
	std::string* p = new std::string((char*)data, len);
	this->PostMessage(WM_USER_DATA, (WPARAM)channel, (LPARAM)p);
	return true;
}

bool CServerCPDlg::OnEvent(StreamEvent event)
{
	this->PostMessage(WM_USER_DATA, NULL, (LPARAM)event);
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

	// TODO: 在此添加额外的初始化代码
	m_pProcess = new koo_process("AccServer", "", "AccServer.exe", "", true);
	m_pProcess->start();
	m_pMapperProcess = new koo_process("Mapper", "", "Mapper.exe", "", true);
	m_pMapperProcess->start();

	m_CTX = zmq_ctx_new();

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
	if (m_pProcess)
		m_pProcess->stop();
	k_kill_process("StreamServer.exe");
	CDialogEx::OnClose();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CServerCPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





HRESULT CServerCPDlg::OnEventMsg(WPARAM wParam, LPARAM lParam)
{
	StreamEvent se = (StreamEvent)lParam;

	return 0L;
}

HRESULT CServerCPDlg::OnDataMsg(WPARAM wParam, LPARAM lParam)
{
	int channel = (int)wParam;
	std::string* p = (std::string*)lParam;

	delete p;
	return 0L;
}

void CServerCPDlg::OnBnClickedButtonConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	m_pAccApi = new CAccApi(m_CTX);
	bool br = m_pAccApi->Connect("127.0.0.1", 6600, "admin", "admin");
	if (br) {
		MessageBox("Connected");
		StreamProcess sp;
		int rc = m_pAccApi->GetStreamServer(&sp);
		if (rc == 0) {
			m_pStreamApi = new CStreamApi(*this, sp.Index, CLIENT_TYPE);
			br = m_pStreamApi->Connect(sp.StreamIP, sp.Port);
			if (br) {
				MessageBox("Stream Connected");
			}
			else {
				MessageBox("Stream Faied");
			}
		}
	}
	else {
		MessageBox("Faied");
	}
}


void CServerCPDlg::OnBnClickedButtonSend()
{
	// TODO: 在此添加控件通知处理程序代码
}
