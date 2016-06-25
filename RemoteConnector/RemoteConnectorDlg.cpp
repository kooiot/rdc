
// RemoteConnectorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RemoteConnector.h"
#include "RemoteConnectorDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CRemoteConnectorDlg �Ի���



CRemoteConnectorDlg::CRemoteConnectorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_REMOTECONNECTOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteConnectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IP, m_editIP);
	DDX_Control(pDX, IDC_EDIT_PORT, m_editPort);
}

BEGIN_MESSAGE_MAP(CRemoteConnectorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CRemoteConnectorDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CRemoteConnectorDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_LISTDEV, &CRemoteConnectorDlg::OnBnClickedButtonListdev)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CRemoteConnectorDlg ��Ϣ�������

BOOL CRemoteConnectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_editIP.SetWindowText("127.0.0.1");
	m_editPort.SetWindowText("6600");
	RC_Init();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRemoteConnectorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CRemoteConnectorDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_hApi != NULL)
		RC_Disconnect(m_hApi);
	RC_Close();
	CDialogEx::OnClose();
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CRemoteConnectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRemoteConnectorDlg::OnBnClickedButtonConnect()
{
	CString ip;
	m_editIP.GetWindowText(ip);
	CString port;
	m_editPort.GetWindowText(port);

	m_hApi = RC_Connect(ip, atoi(port), "admin", "admin");
	if (m_hApi != 0) {
		MessageBox("Connected");
	}
	else {
		MessageBox("Failed");
	}
}


void CRemoteConnectorDlg::OnBnClickedButtonDisconnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	RC_Disconnect(m_hApi);
	m_hApi = NULL;
	MessageBox("Disconnected");
}


void CRemoteConnectorDlg::OnBnClickedButtonListdev()
{
	DeviceInfo* pInfo = new DeviceInfo[1024];
	int num = RC_ListDevices(m_hApi, pInfo, 1024);
	if (num >= 0) {
		MessageBox("OK");
	}
	else {
		MessageBox("Failed");
	}
	delete[] pInfo;
}
