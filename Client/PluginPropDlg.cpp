// PluginPropDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "PluginPropDlg.h"
#include "afxdialogex.h"


// CPluginPropDlg �Ի���

IMPLEMENT_DYNAMIC(CPluginPropDlg, CDialogEx)

CPluginPropDlg::CPluginPropDlg(CString strConfig, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PLUGIN_PROP_DLG, pParent)
	, m_strConfig(strConfig)
{

}

CPluginPropDlg::~CPluginPropDlg()
{
}

void CPluginPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CONFIG, m_strConfig);
}

CString CPluginPropDlg::GetConfig() const
{
	return m_strConfig;
}


BEGIN_MESSAGE_MAP(CPluginPropDlg, CDialogEx)
END_MESSAGE_MAP()


// CPluginPropDlg ��Ϣ�������
