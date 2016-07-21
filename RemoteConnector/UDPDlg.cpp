// TCPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RemoteConnector.h"
#include "UDPDlg.h"
#include "afxdialogex.h"


// CUDPDlg 对话框

IMPLEMENT_DYNAMIC(CUDPDlg, CDialogEx)

CUDPDlg::CUDPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TCP_DIALOG, pParent)
{
	sprintf(m_Info.remote.sip, "%s", "127.0.0.1");
	m_Info.remote.port = 40000;
	sprintf(m_LocalInfo.bind.sip, "%s", "127.0.0.1");
	m_LocalInfo.bind.port = 40002;
}

CUDPDlg::~CUDPDlg()
{
}

void CUDPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_DEV_R, m_ipDevR);
	DDX_Control(pDX, IDC_IPADDRESS_DEV_L, m_ipDevL);
	DDX_Control(pDX, IDC_IPADDRESS_LOCAL_R, m_ipLocalR);
	DDX_Control(pDX, IDC_IPADDRESS_LOCAL_L, m_ipLocalL);
	DDX_Control(pDX, IDC_EDIT_DEV_R, m_portDevR);
	DDX_Control(pDX, IDC_EDIT_DEV_L, m_portDevL);
	DDX_Control(pDX, IDC_EDIT_LOCAL_R, m_portLocalR);
	DDX_Control(pDX, IDC_EDIT_LOCAL_L, m_portLocalL);
	DDX_Control(pDX, IDC_CHECK_DEV_BIND, m_checkDevBind);
	DDX_Control(pDX, IDC_CHECK_REMOTE_LIMIT, m_checkRemotetLimit);
}


BEGIN_MESSAGE_MAP(CUDPDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_DEV_BIND, &CUDPDlg::OnBnClickedCheckDevBind)
	ON_BN_CLICKED(IDC_CHECK_LOCAL_BIND, &CUDPDlg::OnBnClickedCheckLocalBind)
END_MESSAGE_MAP()


#define PARSE_IP_ADDRESS(ipstr, ip1, ip2, ip3, ip4) \
	sscanf(strlen(ipstr) == 0 ? "0.0.0.0" : ipstr, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
// CUDPDlg 消息处理程序

BOOL CUDPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int ip1, ip2, ip3, ip4;
	PARSE_IP_ADDRESS(m_Info.remote.sip, &ip1, &ip2, &ip3, &ip4);
	m_ipDevR.SetAddress(ip1, ip2, ip3, ip4);
	PARSE_IP_ADDRESS(m_Info.bind.sip, &ip1, &ip2, &ip3, &ip4);
	m_ipDevL.SetAddress(ip1, ip2, ip3, ip4);
	PARSE_IP_ADDRESS(m_LocalInfo.remote.sip, &ip1, &ip2, &ip3, &ip4);
	m_ipLocalR.SetAddress(ip1, ip2, ip3, ip4);
	PARSE_IP_ADDRESS(m_LocalInfo.bind.sip, &ip1, &ip2, &ip3, &ip4);
	m_ipLocalL.SetAddress(ip1, ip2, ip3, ip4);

	char temp[128];
	sprintf(temp, "%d", m_Info.remote.port);
	m_portDevR.SetWindowText(temp);
	sprintf(temp, "%d", m_Info.bind.port);
	m_portDevL.SetWindowText(temp);
	sprintf(temp, "%d", m_LocalInfo.remote.port);
	m_portLocalR.SetWindowText(temp);
	sprintf(temp, "%d", m_LocalInfo.bind.port);
	m_portLocalL.SetWindowText(temp);

	if (m_Info.bind.port != 0) {
		m_checkDevBind.SetCheck(BTNS_CHECK);
	}
	else {
		m_ipDevL.EnableWindow(FALSE);
		m_portDevL.EnableWindow(FALSE);
	}
	if (m_LocalInfo.remote.port != 0) {
		m_checkRemotetLimit.SetCheck(BTNS_CHECK);
	}
	else {
		m_ipLocalR.EnableWindow(FALSE);
		m_portLocalR.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



void CUDPDlg::OnOK()
{
	BYTE ip1, ip2, ip3, ip4 = 0;
	m_ipDevR.GetAddress(ip1, ip2, ip3, ip4);
	sprintf(m_Info.remote.sip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
	m_ipDevL.GetAddress(ip1, ip2, ip3, ip4);
	sprintf(m_Info.bind.sip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
	m_ipLocalR.GetAddress(ip1, ip2, ip3, ip4);
	sprintf(m_LocalInfo.remote.sip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
	m_ipLocalL.GetAddress(ip1, ip2, ip3, ip4);
	sprintf(m_LocalInfo.bind.sip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);

	CString str;
	m_portDevR.GetWindowText(str);
	sscanf(str, "%d", &m_Info.remote.port);
	m_portDevL.GetWindowText(str);
	sscanf(str, "%d", &m_Info.bind.port);
	m_portLocalR.GetWindowText(str);
	sscanf(str, "%d", &m_LocalInfo.remote.port);
	m_portLocalL.GetWindowText(str);
	sscanf(str, "%d", &m_LocalInfo.bind.port);

	if (BTNS_CHECK != m_checkDevBind.GetCheck()) {
		m_Info.bind.port = 0;
	}
	if (BTNS_CHECK != m_checkRemotetLimit.GetCheck()) {
		m_LocalInfo.remote.port = 0;
	}

	CDialogEx::OnOK();
}


void CUDPDlg::OnBnClickedCheckDevBind()
{
	BOOL bc = m_checkDevBind.GetCheck();
	m_ipDevL.EnableWindow(bc);
	m_portDevL.EnableWindow(bc);
}


void CUDPDlg::OnBnClickedCheckLocalBind()
{
	BOOL bc = m_checkRemotetLimit.GetCheck();
	m_ipLocalR.EnableWindow(bc);
	m_portLocalR.EnableWindow(bc);
}
