// PluginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RemoteConnector.h"
#include "PluginDlg.h"
#include "afxdialogex.h"
#include <DataJson.h>
#include <sstream>

// CPluginDlg 对话框

IMPLEMENT_DYNAMIC(CPluginDlg, CDialogEx)

CPluginDlg::CPluginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PLUGIN_DIALOG, pParent)
{

}

CPluginDlg::~CPluginDlg()
{
}

void CPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LIST, m_cbList);
	DDX_Control(pDX, IDC_EDIT_CONFIG, m_editConfig);
	DDX_Control(pDX, IDC_EDIT_CONFIG_LOCAL, m_edtConfigLocal);
}


BEGIN_MESSAGE_MAP(CPluginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SLRM, &CPluginDlg::OnBnClickedButtonSlrm)
END_MESSAGE_MAP()


// CPluginDlg 消息处理程序


BOOL CPluginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (auto& str : m_List) {
		m_cbList.AddString(str.c_str());
	}
	m_cbList.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CPluginDlg::OnOK()
{
	m_cbList.GetWindowText(m_Info.Name, RC_MAX_PLUGIN_NAME_LEN);
	m_editConfig.GetWindowText(m_Info.Data, RC_MAX_PLUGIN_DATA_LEN);
	CDialogEx::OnOK();
}


void CPluginDlg::OnBnClickedButtonSlrm()
{
	ConnectionInfo tcp1;
	memset(&tcp1, 0, sizeof(ConnectionInfo));
	tcp1.Type = CT_TCPC;
	tcp1.Channel = 0;
	tcp1.TCPClient.remote.port = 9200;
	sprintf(tcp1.TCPClient.remote.sip, "%s", "127.0.0.1");

	ConnectionInfo tcp2;
	memset(&tcp2, 0, sizeof(ConnectionInfo));
	tcp2.Type = CT_TCPC;
	tcp2.Channel = 0;
	tcp2.TCPClient.remote.port = 9210;
	sprintf(tcp2.TCPClient.remote.sip, "%s", "127.0.0.1");


	ConnectionInfo udp1;
	memset(&udp1, 0, sizeof(ConnectionInfo));
	udp1.Type = CT_UDP;
	udp1.Channel = 0;
	udp1.TCPClient.remote.port = 9800;
	sprintf(udp1.TCPClient.remote.sip, "%s", "127.0.0.1");
	udp1.TCPClient.bind.port = 9801;
	sprintf(udp1.TCPClient.bind.sip, "%s", "127.0.0.1");

	json j;
	j[0] = KOO_GEN_JSON(tcp1);
	j[1] = KOO_GEN_JSON(tcp2);
	j[2] = KOO_GEN_JSON(udp1);

	std::stringstream ss;
	j >> ss;

	m_editConfig.SetWindowText(ss.str().c_str());
}
