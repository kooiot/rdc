// DevicesDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerCP.h"
#include "DevicesDlg.h"
#include "afxdialogex.h"


// CDevicesDlg 对话框

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
	DDX_Control(pDX, IDC_CHECK_VALID, m_chkValid);
}

BEGIN_MESSAGE_MAP(CDevicesDlg, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DEVICES, &CDevicesDlg::OnNMDblclkListDevices)
	ON_BN_CLICKED(IDC_CHECK_VALID, &CDevicesDlg::OnBnClickedCheckValid)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDevicesDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CDevicesDlg::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDevicesDlg::OnBnClickedButtonSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DEVICES, &CDevicesDlg::OnLvnItemchangedListDevices)
END_MESSAGE_MAP()


// CDevicesDlg 消息处理程序


BOOL CDevicesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_listDevs.InsertColumn(0, "Name", LVCFMT_LEFT, 120);
	m_listDevs.InsertColumn(1, "SN", LVCFMT_LEFT, 240);
	m_listDevs.InsertColumn(2, "Desc", LVCFMT_LEFT, 240);
	m_listDevs.SetExtendedStyle(m_listDevs.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	memset(m_Devs, 0, sizeof(DeviceInfo) * 2048);
	int num = m_pAccApi->ListDevices(m_Devs, 2048, false);

	for (int i = 0; i < num; ++i) {
		int n = m_listDevs.InsertItem(i, m_Devs[i].Name);
		m_listDevs.SetItemText(n, 1, m_Devs[i].SN);
		m_listDevs.SetItemText(n, 2, m_Devs[i].Desc);
	}

	m_CurSel = -2;
	BindDevice(-1, false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDevicesDlg::BindDevice(int nCur, bool bEdit)
{
	m_CurSel = nCur;

	if (nCur < 0 || !bEdit) {
		m_editSN.EnableWindow(FALSE);
		m_editName.EnableWindow(FALSE);
		m_editDesc.EnableWindow(FALSE);
		m_editCreateTime.EnableWindow(FALSE);
		m_dtValid.EnableWindow(FALSE);
		m_chkValid.EnableWindow(FALSE);
		m_chkValid.SetCheck(0);
	}
	else {
		m_editSN.EnableWindow(FALSE);
		m_editName.EnableWindow(TRUE);
		m_editDesc.EnableWindow(TRUE);
		m_editCreateTime.EnableWindow(FALSE);
		m_dtValid.EnableWindow(TRUE);
		m_chkValid.EnableWindow(TRUE);
		m_chkValid.SetCheck(0);
	}
	if (nCur < 0)
		return;

	DeviceInfo& info = m_Devs[nCur];
	m_editSN.SetWindowText(info.SN);
	if (info.Index == -1)
		m_editSN.EnableWindow(bEdit ? TRUE : FALSE);
	m_editName.SetWindowText(info.Name);
	m_editDesc.SetWindowText(info.Desc);
	m_editCreateTime.SetWindowText(time2str(&info.CreateTime).c_str());

	CTime valid(info.ValidTime);
	m_dtValid.SetTime(&valid);

	if (info.ValidTime != 0) {
		m_dtValid.EnableWindow(bEdit ? TRUE : FALSE);
		m_chkValid.SetCheck(1);
	}
	else {
		m_dtValid.EnableWindow(FALSE);
		m_chkValid.SetCheck(0);
	}
}

void CDevicesDlg::DumpDevice(int nCur)
{
	DeviceInfo& info = m_Devs[nCur];

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
		info.ValidTime = 0;
	}

	m_listDevs.SetItemText(nCur, 0, info.Name);
	m_listDevs.SetItemText(nCur, 1, info.SN);
	m_listDevs.SetItemText(nCur, 2, info.Desc);
}


const char* newGUID()
{
	static char buf[64] = { 0 };
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf(buf, sizeof(buf)
			, "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
			, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
			, guid.Data4[6], guid.Data4[7]
		);
	}
	return (const char*)buf;
}
void CDevicesDlg::OnBnClickedButtonAdd()
{
	int nCur = m_listDevs.GetItemCount();
	const char* uuid = newGUID();
	int n = m_listDevs.InsertItem(nCur, "");
	m_listDevs.SetItemText(n, 1, uuid);
	ASSERT(m_Devs[n].Index == 0 && m_Devs[n].CreateTime == 0);
	m_Devs[n].Index = -1;
	m_Devs[n].CreateTime = time(NULL);
	m_Devs[n].ValidTime = 0;

	sprintf(m_Devs[n].SN, "%s", uuid);
	m_listDevs.SetItemState(n, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
	BindDevice(nCur, true);
}

void CDevicesDlg::OnBnClickedButtonDel()
{
	if (m_CurSel == -1) {
		return;
	}

	if (LVIS_SELECTED != m_listDevs.GetItemState(m_CurSel, LVIS_SELECTED)) {
		MessageBox("EEEEE");
		return;
	}

	DeviceInfo& info = m_Devs[m_CurSel];
	if (info.Index != -1) {
		int rc = m_pAccApi->DeleteDevice(info.SN);
		if (rc != 0) {
			MessageBox("Delete Device Failed");
			return;
		}
	}
	m_listDevs.DeleteItem(m_CurSel);
	for (int i = m_CurSel + 1; i < 2048; ++i) {
		m_Devs[i - 1] = m_Devs[i];
		if (m_Devs[i].Index == -1)
			break;
	}
}


void CDevicesDlg::OnBnClickedButtonSave()
{
	if (m_CurSel == -1) {
		return;
	}

	if (LVIS_SELECTED != m_listDevs.GetItemState(m_CurSel, LVIS_SELECTED)) {
		MessageBox("EEEEE");
		return;
	}
	DumpDevice(m_CurSel);

	DeviceInfo& info = m_Devs[m_CurSel];
	if (info.Index == -1) {
		// New Device
		info.Index = m_pAccApi->AddDevice(&info);
		if (info.Index == -1) {
			MessageBox("Add Device Failed");
		} 
		else {
			MessageBox("Add Device Done!");
		}
	}
	else {
		int rc = m_pAccApi->ModifyDevice(&info);
		if (rc != 0) {
			MessageBox("Modify Device Failed");
		}
		else {
			MessageBox("Modify Device Done!");
		}
	}
}

void CDevicesDlg::OnBnClickedCheckValid()
{
	m_dtValid.EnableWindow(m_chkValid.GetCheck());
}


void CDevicesDlg::OnNMDblclkListDevices(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	BindDevice(pNMItemActivate->iItem, true);
	*pResult = 0;
}

void CDevicesDlg::OnLvnItemchangedListDevices(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	BindDevice(pNMLV->iItem, false);
	*pResult = 0;
}
