// PluginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RemoteConnector.h"
#include "PluginDlg.h"
#include "afxdialogex.h"


// CPluginDlg �Ի���

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
}


BEGIN_MESSAGE_MAP(CPluginDlg, CDialogEx)
END_MESSAGE_MAP()


// CPluginDlg ��Ϣ�������


BOOL CPluginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (auto& str : m_List) {
		m_cbList.AddString(str.c_str());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CPluginDlg::OnOK()
{
	m_cbList.GetWindowText(m_Info.Name, RC_MAX_PLUGIN_NAME_LEN);
	m_editConfig.GetWindowText(m_Info.Data, RC_MAX_PLUGIN_DATA_LEN);
	CDialogEx::OnOK();
}
