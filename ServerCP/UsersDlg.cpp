// UsersDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerCP.h"
#include "UsersDlg.h"
#include "afxdialogex.h"


// CUsersDlg 对话框

IMPLEMENT_DYNAMIC(CUsersDlg, CDialogEx)

CUsersDlg::CUsersDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_USERS_DIALOG, pParent)
{
	m_Users = new UserInfo[2048];
}

CUsersDlg::~CUsersDlg()
{
	delete[] m_Users;
}

void CUsersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_USERS, m_listUsers);
	DDX_Control(pDX, IDC_EDIT_ID, m_editID);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_DESC, m_editDesc);
	DDX_Control(pDX, IDC_EDIT_EMAIL, m_editEmail);
	DDX_Control(pDX, IDC_EDIT_PHONE, m_editPhone);
	DDX_Control(pDX, IDC_COMBO_LEVEL, m_boxLevel);
	DDX_Control(pDX, IDC_EDIT_CREATE_TIME, m_editCreateTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_VALID, m_dtValid);
	DDX_Control(pDX, IDC_EDIT_PASSWD, m_editPasswd);
	DDX_Control(pDX, IDC_CHECK_VALID, m_chkValid);
}

BEGIN_MESSAGE_MAP(CUsersDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CUsersDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CUsersDlg::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_PASS, &CUsersDlg::OnBnClickedButtonShowPass)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_USERS, &CUsersDlg::OnNMDblclkListUsers)
	ON_BN_CLICKED(IDC_CHECK_VALID, &CUsersDlg::OnBnClickedCheckValid)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CUsersDlg::OnBnClickedButtonSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_USERS, &CUsersDlg::OnLvnItemchangedListUsers)
END_MESSAGE_MAP()


// CUsersDlg 消息处理程序

BOOL CUsersDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_boxLevel.AddString("普通用户");
	m_boxLevel.AddString("企业账户");
	m_boxLevel.AddString("管理员");
	m_boxLevel.AddString("系统管理员");

	m_listUsers.InsertColumn(0, "ID", LVCFMT_LEFT, 60);
	m_listUsers.InsertColumn(1, "Name", LVCFMT_LEFT, 120);
	m_listUsers.InsertColumn(2, "Desc", LVCFMT_LEFT, 120);

	// TODO:  在此添加额外的初始化
	memset(m_Users, 0, sizeof(UserInfo) * 2048);
	int num = m_pAccApi->ListUsers(m_Users, 2048, false);

	for (int i = 0; i < num; ++i) {
		int n = m_listUsers.InsertItem(i, m_Users[i].ID);
		m_listUsers.SetItemText(n, 1, m_Users[i].Name);
		m_listUsers.SetItemText(n, 2, m_Users[i].Desc);
	}
	m_listUsers.SetExtendedStyle(m_listUsers.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	m_CurSel = -2;
	BindUser(-1, false);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CUsersDlg::BindUser(int nCur, bool bEdit)
{
	m_CurSel = nCur;

	if (nCur < 0 || !bEdit) {
		m_editID.EnableWindow(FALSE);
		m_editPasswd.EnableWindow(FALSE);
		m_editName.EnableWindow(FALSE);
		m_editDesc.EnableWindow(FALSE);
		m_editEmail.EnableWindow(FALSE);
		m_editPhone.EnableWindow(FALSE);
		m_boxLevel.EnableWindow(FALSE);
		m_editCreateTime.EnableWindow(FALSE);
		m_dtValid.EnableWindow(FALSE);
		m_chkValid.EnableWindow(FALSE);
		m_chkValid.SetCheck(0);
	}
	else {
		m_editID.EnableWindow(FALSE);
		m_editPasswd.EnableWindow(TRUE);
		m_editName.EnableWindow(TRUE);
		m_editDesc.EnableWindow(TRUE);
		m_editEmail.EnableWindow(TRUE);
		m_editPhone.EnableWindow(TRUE);
		m_boxLevel.EnableWindow(TRUE);
		m_editCreateTime.EnableWindow(FALSE);
		m_dtValid.EnableWindow(TRUE);
		m_chkValid.EnableWindow(TRUE);
		m_chkValid.SetCheck(0);
	}
	if (nCur < 0)
		return;

	UserInfo& info = m_Users[nCur];
	m_editID.SetWindowText(info.ID);
	if (info.Index == -1)
		m_editID.EnableWindow(bEdit ? TRUE : FALSE);
	m_editPasswd.SetWindowText(info.Passwd);
	m_editName.SetWindowText(info.Name);
	m_editDesc.SetWindowText(info.Desc);
	m_editEmail.SetWindowText(info.Email);
	m_editPhone.SetWindowText(info.Phone);
	if (info.Level >= 99)
		m_boxLevel.SetCurSel(3);
	else if (info.Level > 10)
		m_boxLevel.SetCurSel(2);
	else if (info.Level > 1)
		m_boxLevel.SetCurSel(1);
	else
		m_boxLevel.SetCurSel(0);
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
	}
}

void CUsersDlg::DumpUser(int nCur)
{
	UserInfo& info = m_Users[nCur];

	m_editID.GetWindowText(info.ID, RC_MAX_ID_LEN);
	m_editPasswd.GetWindowText(info.Passwd, RC_MAX_PASSWD_LEN);
	m_editName.GetWindowText(info.Name, RC_MAX_NAME_LEN);
	m_editDesc.GetWindowText(info.Desc, RC_MAX_DESC_LEN);
	m_editEmail.GetWindowText(info.Email, RC_MAX_EMAIL_LEN);
	m_editPhone.GetWindowText(info.Phone, RC_MAX_PHONE_LEN);

	if (m_boxLevel.GetCurSel() == 3)
		info.Level = 99;
	if (m_boxLevel.GetCurSel() == 2)
		info.Level = 10;
	if (m_boxLevel.GetCurSel() == 1)
		info.Level = 2;
	if (m_boxLevel.GetCurSel() == 0)
		info.Level = 1;

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

	m_listUsers.SetItemText(nCur, 0, info.ID);
	m_listUsers.SetItemText(nCur, 1, info.Name);
	m_listUsers.SetItemText(nCur, 2, info.Desc);
}


void CUsersDlg::OnBnClickedButtonAdd()
{
	int nCur = m_listUsers.GetItemCount();
	int n = m_listUsers.InsertItem(nCur, "New_User");
	ASSERT(m_Users[n].Index == 0 && m_Users[n].CreateTime == 0);
	m_Users[n].Index = -1;
	m_Users[n].CreateTime = time(NULL);
	m_Users[n].ValidTime = -1;
	sprintf(m_Users[n].ID, "%s", "New_User");
	//m_listUsers.SetItem(nCur, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
	m_listUsers.SetItemState(nCur, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
	BindUser(nCur, true);
}


void CUsersDlg::OnBnClickedButtonDel()
{
	if (m_CurSel == -1) {
		return;
	}

	if (LVIS_SELECTED != m_listUsers.GetItemState(m_CurSel, LVIS_SELECTED)) {
		MessageBox("EEEEE");
		return;
	}

	UserInfo& info = m_Users[m_CurSel];
	if (info.Index != -1) {
		int rc = m_pAccApi->DeleteUser(info.ID);
		if (rc != 0) {
			MessageBox("Delete User Failed");
			return;
		}
	}
	m_listUsers.DeleteItem(m_CurSel);
	for (int i = m_CurSel + 1; i < 2048; ++i) {
		m_Users[i - 1] = m_Users[i];
		if (m_Users[i].Index == -1)
			break;
	}
}

void CUsersDlg::OnBnClickedButtonShowPass()
{
	/*DWORD dw = m_editPasswd.GetStyle();
	dw ^= ES_PASSWORD;
	m_editPasswd.Setwindow*/
	/*m_editPasswd.ModifyStyle(ES_PASSWORD, 0);
	m_editPasswd.SetPasswordChar('*');
*/
	//m_editPasswd.SetPasswordChar('*');
}


void CUsersDlg::OnNMDblclkListUsers(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	BindUser(pNMItemActivate->iItem, true);
	*pResult = 0;
}


void CUsersDlg::OnBnClickedCheckValid()
{
	// TODO: 在此添加控件通知处理程序代码
	m_dtValid.EnableWindow(m_chkValid.GetCheck());
}


void CUsersDlg::OnBnClickedButtonSave()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_CurSel == -1) {
		return;
	}

	if (LVIS_SELECTED != m_listUsers.GetItemState(m_CurSel, LVIS_SELECTED)) {
		MessageBox("EEEEE");
		return;
	}
	DumpUser(m_CurSel);

	UserInfo& info = m_Users[m_CurSel];
	if (info.Index == -1) {
		// New User
		info.Index = m_pAccApi->AddUser(&info);
		if (info.Index == -1) {
			MessageBox("Add User Failed");
		}
		else {
			MessageBox("Add User Done!");
		}
	} else {
		int rc = m_pAccApi->ModifyUser(&info);
		if (rc != 0) {
			MessageBox("Modify User Failed");
		}
		else {
			MessageBox("Modify User Done!");
		}
	}
}


void CUsersDlg::OnLvnItemchangedListUsers(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	BindUser(pNMLV->iItem, false);
	*pResult = 0;
}
