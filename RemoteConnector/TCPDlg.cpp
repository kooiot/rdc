// TCPDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RemoteConnector.h"
#include "TCPDlg.h"
#include "afxdialogex.h"


// CTCPDlg �Ի���

IMPLEMENT_DYNAMIC(CTCPDlg, CDialogEx)

CTCPDlg::CTCPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TCP_DIALOG, pParent)
{

}

CTCPDlg::~CTCPDlg()
{
}

void CTCPDlg::DoDataExchange(CDataExchange* pDX)
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
	DDX_Control(pDX, IDC_CHECK_LOCAL_BIND, m_checkLocalBind);
}


BEGIN_MESSAGE_MAP(CTCPDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_DEV_BIND, &CTCPDlg::OnBnClickedCheckDevBind)
	ON_BN_CLICKED(IDC_CHECK_LOCAL_BIND, &CTCPDlg::OnBnClickedCheckLocalBind)
END_MESSAGE_MAP()


#define PARSE_IP_ADDRESS(ipstr, ip1, ip2, ip3, ip4) \
	sscanf(strlen(ipstr) == 0 ? "0.0.0.0" : ipstr, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
// CTCPDlg ��Ϣ��������

BOOL CTCPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int ip1, ip2, ip3, ip4;
	PARSE_IP_ADDRESS(m_Info.server.sip, &ip1, &ip2, &ip3, &ip4);
	m_ipDevR.SetAddress(ip1, ip2, ip3, ip4);
	PARSE_IP_ADDRESS(m_Info.local.sip, &ip1, &ip2, &ip3, &ip4);
	m_ipDevL.SetAddress(ip1, ip2, ip3, ip4);
	PARSE_IP_ADDRESS(m_LocalInfo.server.sip, &ip1, &ip2, &ip3, &ip4);
	m_ipLocalR.SetAddress(ip1, ip2, ip3, ip4);
	PARSE_IP_ADDRESS(m_LocalInfo.local.sip, &ip1, &ip2, &ip3, &ip4);
	m_ipLocalL.SetAddress(ip1, ip2, ip3, ip4);
	
	char temp[128];
	sprintf(temp, "%d", m_Info.server.port);
	m_portDevR.SetWindowText(temp);
	sprintf(temp, "%d", m_Info.local.port);
	m_portDevL.SetWindowText(temp);
	sprintf(temp, "%d", m_LocalInfo.server.port);
	m_portLocalR.SetWindowText(temp);
	sprintf(temp, "%d", m_LocalInfo.local.port);
	m_portLocalL.SetWindowText(temp);

	if (m_Info.local.port != 0) {
		m_checkDevBind.SetCheck(BTNS_CHECK);
	}
	else {
		m_ipDevL.EnableWindow(FALSE);
		m_portDevL.EnableWindow(FALSE);
	}
	if (m_LocalInfo.local.port != 0) {
		m_checkLocalBind.SetCheck(BTNS_CHECK);
	}
	else {
		m_ipLocalL.EnableWindow(FALSE);
		m_portLocalL.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}



void CTCPDlg::OnOK()
{
	BYTE ip1, ip2, ip3, ip4 = 0;
	m_ipDevR.GetAddress(ip1, ip2, ip3, ip4);
	sprintf(m_Info.server.sip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
	m_ipDevL.GetAddress(ip1, ip2, ip3, ip4);
	sprintf(m_Info.local.sip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
	m_ipLocalR.GetAddress(ip1, ip2, ip3, ip4);
	sprintf(m_LocalInfo.server.sip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
	m_ipLocalL.GetAddress(ip1, ip2, ip3, ip4);
	sprintf(m_LocalInfo.local.sip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);

	CString str;
	m_portDevR.GetWindowText(str);
	sscanf(str, "%d", &m_Info.server.port);
	m_portDevL.SetWindowText(str);
	sscanf(str, "%d", &m_Info.local.port);
	m_portLocalR.SetWindowText(str);
	sscanf(str, "%d", &m_LocalInfo.server.port);
	m_portLocalL.SetWindowText(str);
	sscanf(str, "%d", &m_LocalInfo.local.port);

	if (BTNS_CHECK != m_checkDevBind.GetCheck()) {
		m_Info.local.port = 0;
	}
	if (BTNS_CHECK != m_checkLocalBind.GetCheck()) {
		m_LocalInfo.local.port = 0;
	}

	CDialogEx::OnOK();
}


void CTCPDlg::OnBnClickedCheckDevBind()
{
	BOOL bc = m_checkDevBind.GetCheck();
	m_ipDevL.EnableWindow(bc);
	m_portDevL.EnableWindow(bc);
}


void CTCPDlg::OnBnClickedCheckLocalBind()
{
	BOOL bc = m_checkLocalBind.GetCheck();
	m_ipLocalL.EnableWindow(bc);
	m_portLocalL.EnableWindow(bc);
}