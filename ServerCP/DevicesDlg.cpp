// DevicesDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ServerCP.h"
#include "DevicesDlg.h"
#include "afxdialogex.h"


// CDevicesDlg �Ի���

IMPLEMENT_DYNAMIC(CDevicesDlg, CDialogEx)

CDevicesDlg::CDevicesDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DEVICES_DIALOG, pParent)
{
	m_Devs = new DeviceInfo[2048];
}

CDevicesDlg::~CDevicesDlg()
{
	delete[] m_Devs;
}

void CDevicesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEVICES, m_listDevs);
	DDX_Control(pDX, IDC_EDIT_SN, m_editSN);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_DESC, m_editDesc);
	DDX_Control(pDX, IDC_EDIT_CREATE_TIME, m_editCreateTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_VALID, m_dtValid);
}

void CDevicesDlg::BindUser(int nCur)
{
}


BEGIN_MESSAGE_MAP(CDevicesDlg, CDialogEx)
END_MESSAGE_MAP()


// CDevicesDlg ��Ϣ�������


BOOL CDevicesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	m_listDevs.InsertColumn(0, "SN", LVCFMT_LEFT, 120);
	m_listDevs.InsertColumn(1, "Name", LVCFMT_LEFT, 120);
	m_listDevs.InsertColumn(2, "Desc", LVCFMT_LEFT, 120);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	memset(m_Devs, 0, sizeof(DeviceInfo) * 2048);
	int num = m_pAccApi->ListDevices(m_Devs, 2048, false);

	for (int i = 0; i < num; ++i) {
		int n = m_listDevs.InsertItem(i, m_Devs[i].SN);
		m_listDevs.SetItemText(n, 1, m_Devs[i].Name);
		m_listDevs.SetItemText(n, 2, m_Devs[i].Desc);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
