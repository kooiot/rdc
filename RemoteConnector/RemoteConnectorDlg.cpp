
// RemoteConnectorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RemoteConnector.h"
#include "RemoteConnectorDlg.h"
#include "afxdialogex.h"
#include "SerialDlg.h"

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
	m_Devices = new DeviceInfo[RC_MAX_ONLINE_DEVICE];
	memset(m_Devices, 0, sizeof(DeviceInfo) * RC_MAX_ONLINE_DEVICE);
	memset(m_ConnectionInfos, 0, sizeof(ConnectionInfo*) * RC_MAX_CONNECTION);
	memset(m_StreamPorts, 0, sizeof(IStreamHandler*) * RC_MAX_CONNECTION);
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
	ON_BN_CLICKED(IDC_BUTTON_START, &CRemoteConnectorDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CRemoteConnectorDlg::OnBnClickedButtonStop)
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

	m_listDevs.InsertColumn(0, "Name", LVCFMT_LEFT, 120);
	m_listDevs.InsertColumn(1, "SN", LVCFMT_LEFT, 240);
	m_listDevs.InsertColumn(2, "Desc", LVCFMT_LEFT, 240);
	m_listDevs.SetExtendedStyle(m_listDevs.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	m_listConnections.InsertColumn(0, "Type", LVCFMT_LEFT, 60);
	m_listConnections.InsertColumn(1, "DevSN", LVCFMT_LEFT, 120);
	m_listConnections.SetExtendedStyle(m_listConnections.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	
	m_VSPortMgr.Init();
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
	m_VSPortMgr.Close();

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

void CRemoteConnectorDlg::StreamDataCallback(RC_CHANNEL channel, void * buf, size_t len, void * prv)
{
	CRemoteConnectorDlg* pThis = (CRemoteConnectorDlg*)prv;
	if (!pThis)
		return;
	pThis->__StreamDataCallback(channel, buf, len);
}

void CRemoteConnectorDlg::__StreamDataCallback(RC_CHANNEL channel, void * buf, size_t len)
{
	IStreamHandler* Handler = m_StreamPorts[channel];
	if (Handler) {
		Handler->OnData((void*)buf, len);
	}
}

void CRemoteConnectorDlg::StreamEventCallback(StreamEvent evt, void * prv)
{
	CRemoteConnectorDlg* pThis = (CRemoteConnectorDlg*)prv;
	if (!pThis)
		return;
	pThis->__StreamEventCallback(evt);
}

void CRemoteConnectorDlg::__StreamEventCallback(StreamEvent evt)
{
	TRACE("[STREAM_EVENT] [%d]\n", evt);
	// FIXME:
}

int CRemoteConnectorDlg::OnLog(RC_CHANNEL channel, const char * type, const char * content)
{
	TRACE("[LOG] [%d]\t [%s] %s\n", channel, type, content);
	// FIXME:
	return 0;
}

int CRemoteConnectorDlg::Send(RC_CHANNEL channel, void * buf, size_t len)
{
	return RC_StreamSend(m_hApi, channel, buf, len);
}

void CRemoteConnectorDlg::AddConnection(ConnectionInfo * info)
{
	if (m_StreamPorts[info->Channel] != NULL) {
		MessageBox("EEE");
		return;
	}
	IStreamHandler* pHandler = NULL;
	if (info->Type == CT_SERIAL) {
		pHandler = m_VSPortMgr.CreatePort(info->Channel, *this, "COM4");
	}
	if (!pHandler)
		return;

	m_StreamPorts[info->Channel] = pHandler;

	int nIndex = m_listConnections.GetItemCount();
	nIndex = m_listConnections.InsertItem(nIndex, "����");
	m_listConnections.SetItemText(nIndex, 1, info->DevSN);
	m_listConnections.SetItemData(nIndex, info->Channel);
}

void CRemoteConnectorDlg::RemoveConnection(RC_CHANNEL channel)
{
	if (m_StreamPorts[channel] != NULL) {
		return;
	}
	IStreamHandler* pHandler = m_StreamPorts[channel];
	ConnectionInfo * info = m_ConnectionInfos[channel];
	if (info->Type == CT_SERIAL) {
		m_VSPortMgr.FreePort((VSPort*)pHandler);
	}
}

void CRemoteConnectorDlg::OnBnClickedButtonConnect()
{
	CString ip;
	m_editIP.GetWindowText(ip);
	CString port;
	m_editPort.GetWindowText(port);

	m_hApi = RC_Connect(ip, atoi(port), "admin", "admin");
	if (m_hApi != 0) {
		RC_SetStreamCallback(m_hApi, StreamDataCallback, StreamEventCallback, this);
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


void CRemoteConnectorDlg::OnBnClickedButtonAdd()
{
	if (m_hApi == NULL)
		return;

	POSITION pos = m_listDevs.GetFirstSelectedItemPosition();
	if (pos == NULL) {
		MessageBox("ѡ��һ���豸��", "��ʾ", MB_OK | MB_ICONASTERISK);
		return;
	}
	int nItem = m_listDevs.GetNextSelectedItem(pos);
	if (nItem < 0 || nItem > 2048) {
		MessageBox("�����豸�б�", "��ʾ", MB_OK | MB_ICONERROR);
		return;
	}

	DeviceInfo& info = m_Devices[nItem];
	if (strlen(info.SN) == 0) {
		MessageBox("û�и��豸", "��ʾ", MB_OK | MB_ICONERROR);
		return;
	}
	CSerialDlg dlg;
	if (IDOK == dlg.DoModal()) {
		RC_CHANNEL channel = RC_ConnectSerial(m_hApi, info.SN, &dlg.m_Info);
		if (channel < 0) {
			MessageBox("��������", "��ʾ", MB_OK | MB_ICONERROR);
			return;
		}
		assert(m_ConnectionInfos[channel] == NULL);
		ConnectionInfo* ci = new ConnectionInfo();
		ci->Type = CT_SERIAL;
		memcpy(ci->DevSN, info.SN, RC_MAX_SN_LEN);
		ci->Serial = dlg.m_Info;
		ci->Channel = channel;
		m_ConnectionInfos[channel] = ci;
		AddConnection(ci);
	}
}


void CRemoteConnectorDlg::OnNMDblclkListConnections(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int channel = m_listConnections.GetItemData(pNMItemActivate->iItem);
	if (channel < 0 || channel > RC_MAX_CONNECTION)
		return;

	ConnectionInfo* pi = m_ConnectionInfos[channel];
	if (pi->Type = CT_SERIAL) {
		CSerialDlg dlg;
		dlg.m_Info = pi->Serial;
		dlg.m_Editable = false;
		if (IDOK == dlg.DoModal()) {
			
		}
	}
	*pResult = 0;
}


void CRemoteConnectorDlg::OnBnClickedButtonDelete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CRemoteConnectorDlg::OnBnClickedButtonStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CRemoteConnectorDlg::OnBnClickedButtonStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
