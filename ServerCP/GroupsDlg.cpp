// GroupsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerCP.h"
#include "GroupsDlg.h"
#include "afxdialogex.h"


// CGroupsDlg 对话框

IMPLEMENT_DYNAMIC(CGroupsDlg, CDialogEx)

CGroupsDlg::CGroupsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GROUPS_DIALOG, pParent)
{
	m_Groups = new GroupInfo[2048];
	m_Devs = new DeviceInfo[2048];
	m_GroupDevices = new int[2048];
}

CGroupsDlg::~CGroupsDlg()
{
	delete[] m_Groups;
	delete[] m_Devs;
	delete[] m_GroupDevices;
}

void CGroupsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GROUPS, m_listGroups);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_DESC, m_editDesc);
	DDX_Control(pDX, IDC_EDIT_CREATE_TIME, m_editCreateTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_VALID, m_dtValid);
	DDX_Control(pDX, IDC_CHECK_VALID, m_chkValid);
	DDX_Control(pDX, IDC_LIST_DEVICES, m_listDevices);
}


BEGIN_MESSAGE_MAP(CGroupsDlg, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GROUPS, &CGroupsDlg::OnNMDblclkListGroups)
	ON_BN_CLICKED(IDC_CHECK_VALID, &CGroupsDlg::OnBnClickedCheckValid)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CGroupsDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CGroupsDlg::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CGroupsDlg::OnBnClickedButtonSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GROUPS, &CGroupsDlg::OnLvnItemchangedListGroups)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DEVICES, &CGroupsDlg::OnLvnItemchangedListDevices)
END_MESSAGE_MAP()


// CGroupsDlg 消息处理程序


BOOL CGroupsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_listGroups.InsertColumn(0, "Name", LVCFMT_LEFT, 120);
	m_listGroups.InsertColumn(1, "Desc", LVCFMT_LEFT, 240);
	m_listGroups.SetExtendedStyle(m_listGroups.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	// TODO:  在此添加额外的初始化
	m_listDevices.InsertColumn(0, "Name", LVCFMT_LEFT, 120);
	m_listDevices.InsertColumn(0, "SN", LVCFMT_LEFT, 249);
	m_listDevices.SetExtendedStyle(m_listDevices.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	memset(m_Devs, 0, sizeof(DeviceInfo) * 2048);
	int num = m_pAccApi->ListDevices(m_Devs, 2048, false);
	for (int i = 0; i < num; ++i) {
		int n = m_listDevices.InsertItem(i, m_Devs[i].Name);
		m_listDevices.SetItemText(n, 1, m_Devs[i].SN);
	}

	memset(m_Groups, 0, sizeof(GroupInfo) * 2048);
	num = m_pAccApi->ListGroups(m_Groups, 2048);

	for (int i = 0; i < num; ++i) {
		int n = m_listGroups.InsertItem(i, m_Groups[i].Name);
		m_listGroups.SetItemText(n, 1, m_Groups[i].Desc);
	}

	m_CurSel = -2;
	BindGroup(-1, false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CGroupsDlg::BindGroup(int nCur, bool bEdit)
{
	m_CurSel = nCur;

	if (nCur < 0 || !bEdit) {
		m_listDevices.EnableWindow(FALSE);
		m_editName.EnableWindow(FALSE);
		m_editDesc.EnableWindow(FALSE);
		m_editCreateTime.EnableWindow(FALSE);
		m_dtValid.EnableWindow(FALSE);
		m_chkValid.EnableWindow(FALSE);
		m_chkValid.SetCheck(0);
	}
	else {
		m_listDevices.EnableWindow(TRUE);
		m_editName.EnableWindow(TRUE);
		m_editDesc.EnableWindow(TRUE);
		m_editCreateTime.EnableWindow(FALSE);
		m_dtValid.EnableWindow(TRUE);
		m_chkValid.EnableWindow(TRUE);
		m_chkValid.SetCheck(0);
	}
	if (nCur < 0)
		return;

	GroupInfo& info = m_Groups[nCur];
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
	
	memset(m_GroupDevices, 0, sizeof(int) * 2048);

	int num = 0;
	if (info.Index > 0)
		num = m_pAccApi->ListGroupDevices(info.Index, m_GroupDevices, 2048);
	else
		m_listDevices.EnableWindow(FALSE);

	for (int i = 0; i < m_listDevices.GetItemCount(); ++i) {
		BOOL check = FALSE;
		for (int j = 0; j < num; ++j) {
			if (m_Devs[i].Index == m_GroupDevices[j]) {
				check = TRUE;
				break;
			}
		}
		m_listDevices.SetCheck(i, check);
		m_listDevices.SetItemData(i, check);
	}
	m_GroupDeviceCount = num;
}

void CGroupsDlg::DumpGroup(int nCur)
{
	GroupInfo& info = m_Groups[nCur];

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

	m_listGroups.SetItemText(nCur, 0, info.Name);
	m_listGroups.SetItemText(nCur, 1, info.Desc);
}

void CGroupsDlg::OnBnClickedButtonAdd()
{
	int nCur = m_listGroups.GetItemCount();
	int n = m_listGroups.InsertItem(nCur, "New Group");
	ASSERT(m_Groups[n].Index == 0 && m_Groups[n].CreateTime == 0);
	m_Groups[n].Index = -1;
	m_Groups[n].CreateTime = time(NULL);
	m_Groups[n].ValidTime = 0;

	m_listGroups.SetItemState(n, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
	BindGroup(nCur, true);
}

void CGroupsDlg::OnBnClickedButtonDel()
{
	if (m_CurSel == -1) {
		return;
	}

	if (LVIS_SELECTED != m_listGroups.GetItemState(m_CurSel, LVIS_SELECTED)) {
		MessageBox("EEEEE");
		return;
	}

	GroupInfo& info = m_Groups[m_CurSel];
	if (info.Index != -1) {
		int rc = m_pAccApi->DeleteGroup(info.Name);
		if (rc != 0) {
			MessageBox("Delete Group Failed");
			return;
		}
	}
	m_listGroups.DeleteItem(m_CurSel);
	for (int i = m_CurSel + 1; i < 2048; ++i) {
		m_Groups[i - 1] = m_Groups[i];
		if (m_Groups[i].Index == -1)
			break;
	}
}


void CGroupsDlg::OnBnClickedButtonSave()
{
	if (m_CurSel == -1) {
		return;
	}

	if (LVIS_SELECTED != m_listGroups.GetItemState(m_CurSel, LVIS_SELECTED)) {
		MessageBox("EEEEE");
		return;
	}
	DumpGroup(m_CurSel);

	GroupInfo& info = m_Groups[m_CurSel];
	if (info.Index == -1) {
		// New User
		info.Index = m_pAccApi->AddGroup(&info);
		if (info.Index == -1) {
			MessageBox("Add Group Failed");
		}
		else {
			MessageBox("Add Group Done!");
		}
	}
	else {
		int rc = m_pAccApi->ModifyGroup(&info);
		if (rc != 0) {
			MessageBox("Modify Group Failed");
		}
		else {
			MessageBox("Modify Group Done!");
		}
	}
}

void CGroupsDlg::OnBnClickedCheckValid()
{
	m_dtValid.EnableWindow(m_chkValid.GetCheck());
}


void CGroupsDlg::OnNMDblclkListGroups(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	BindGroup(pNMItemActivate->iItem, true);
	*pResult = 0;
}

void CGroupsDlg::OnLvnItemchangedListGroups(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	BindGroup(pNMLV->iItem, false);
	*pResult = 0;
}


void CGroupsDlg::OnLvnItemchangedListDevices(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int nCur = pNMLV->iItem;
	if (nCur >= 0 && m_CurSel >= 0) {
		BOOL check = m_listDevices.GetCheck(nCur);
		if (check != (BOOL)m_listDevices.GetItemData(nCur))
		{
			if (check)
				m_pAccApi->AddDeviceToGroup(m_Groups[m_CurSel].Index, m_Devs[nCur].Index);
			else
				m_pAccApi->RemoveDeviceToGroup(m_Groups[m_CurSel].Index, m_Devs[nCur].Index);
		}
	}
	*pResult = 0;
}
