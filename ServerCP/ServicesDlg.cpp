// ServicesDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerCP.h"
#include "ServicesDlg.h"
#include "afxdialogex.h"


// CServicesDlg 对话框

IMPLEMENT_DYNAMIC(CServicesDlg, CDialogEx)

CServicesDlg::CServicesDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVICES_DIALOG, pParent)
{

}

CServicesDlg::~CServicesDlg()
{
}

void CServicesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVICES, m_listServices);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_DESC, m_editDesc);
	DDX_Control(pDX, IDC_EDIT_EXEC, m_editExec);
	DDX_Control(pDX, IDC_EDIT_WORKDIR, m_editWorkDir);
	DDX_Control(pDX, IDC_COMBO_MODE, m_cbMode);
	DDX_Control(pDX, IDC_EDIT_ARGS, m_editArgs);
}


BEGIN_MESSAGE_MAP(CServicesDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CServicesDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CServicesDlg::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CServicesDlg::OnBnClickedButtonSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SERVICES, &CServicesDlg::OnLvnItemchangedListServices)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SERVICES, &CServicesDlg::OnNMDblclkListServices)
END_MESSAGE_MAP()


// CServicesDlg 消息处理程序


BOOL CServicesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_CurSel = -1;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CServicesDlg::OnOK()
{
	CDialogEx::OnOK();
}


void CServicesDlg::BindService(int nCur, bool bEdit)
{
	m_CurSel = nCur;

	if (nCur < 0 || !bEdit) {
		m_editName.EnableWindow(FALSE);
		m_editDesc.EnableWindow(FALSE);
		m_editExec.EnableWindow(FALSE);
		m_editWorkDir.EnableWindow(FALSE);
		m_editArgs.EnableWindow(FALSE);
	}
	else {
		m_editName.EnableWindow(TRUE);
		m_editDesc.EnableWindow(TRUE);
		m_editExec.EnableWindow(TRUE);
		m_editWorkDir.EnableWindow(TRUE);
		m_editArgs.EnableWindow(TRUE);
	}
	if (nCur < 0)
		return;
/*
	DeviceInfo& info = m_Devs[nCur];
	m_editName.SetWindowText(info.Name);
	m_editDesc.SetWindowText(info.Desc);
	m_editCreateTime.SetWindowText(time2str_utc(&info.CreateTime).c_str());

	if (info.ValidTime != -1) {
		char buffer[80];
		struct tm * timeinfo = gmtime(&info.ValidTime);
		strftime(buffer, 80, "%Y/%m/%d", timeinfo);
		m_dtValid.EnableWindow(bEdit ? TRUE : FALSE);
		m_dtValid.SetWindowText(buffer);
		m_chkValid.SetCheck(1);
	}
	else {
		m_dtValid.EnableWindow(FALSE);
		m_chkValid.SetCheck(0);
	}*/
}

void CServicesDlg::DumpService(int nCur)
{
	/*DeviceInfo& info = m_Devs[nCur];

	m_editSN.GetWindowText(info.SN, RC_MAX_SN_LEN);
	m_editName.GetWindowText(info.Name, RC_MAX_NAME_LEN);
	m_editDesc.GetWindowText(info.Desc, RC_MAX_DESC_LEN);

	if (info.Index == -1)
		info.CreateTime = time(NULL);

	if (m_chkValid.GetCheck()) {
		CTime valid;
		m_dtValid.GetTime(valid);
		struct tm t;
		info.ValidTime = mktime(valid.GetGmtTm(&t));
	}
	else {
		info.ValidTime = -1;
	}

	m_listDevs.SetItemText(nCur, 0, info.SN);
	m_listDevs.SetItemText(nCur, 1, info.Name);
	m_listDevs.SetItemText(nCur, 2, info.Desc);*/
}

void CServicesDlg::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CServicesDlg::OnBnClickedButtonDel()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CServicesDlg::OnBnClickedButtonSave()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CServicesDlg::OnLvnItemchangedListServices(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	BindService(pNMLV->iItem, false);
	*pResult = 0;
}


void CServicesDlg::OnNMDblclkListServices(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	BindService(pNMItemActivate->iItem, true);
	*pResult = 0;
}
