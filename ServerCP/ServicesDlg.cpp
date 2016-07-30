// ServicesDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerCP.h"
#include "ServicesDlg.h"
#include "afxdialogex.h"
#include <ServicesApi.h>

// CServicesDlg 对话框

IMPLEMENT_DYNAMIC(CServicesDlg, CDialogEx)

CServicesDlg::CServicesDlg(void* pContext, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVICES_DIALOG, pParent)
	, m_CurSel(-1)
{
	m_pApi = new CServicesApi(pContext);
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
	ON_BN_CLICKED(IDC_BUTTON_ACC, &CServicesDlg::OnBnClickedButtonAcc)
	ON_BN_CLICKED(IDC_BUTTON_STREAM, &CServicesDlg::OnBnClickedButtonStream)
END_MESSAGE_MAP()


// CServicesDlg 消息处理程序


BOOL CServicesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_cbMode.AddString("Disable");
	m_cbMode.AddString("Once");
	m_cbMode.AddString("Auto");

	m_listServices.InsertColumn(0, "Name", LVCFMT_LEFT, 120);
	m_listServices.InsertColumn(1, "Desc", LVCFMT_LEFT, 240);
	m_listServices.SetExtendedStyle(m_listServices.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	if (!m_pApi->Connect())
		MessageBox("Failed to connect to ServiceStater");

	ServiceNode Nodes[64];
	int rc = m_pApi->List(Nodes, 64);
	for (int i = 0; i < rc; ++i) {
		memcpy(&m_Nodes[i], &Nodes[i], sizeof(ServiceNode));
		m_Nodes[i].New = false;
		int n = m_listServices.InsertItem(i, m_Nodes[i].Name);
		m_listServices.SetItemText(n, 1, m_Nodes[i].Desc);
	}
	
	m_CurSel = -2;
	BindService(-1, false);

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
	if (nCur < 0 || nCur > 64)
		return;

	ServiceNodeEx& info = m_Nodes[nCur];
	m_editName.SetWindowText(info.Name);
	m_editDesc.SetWindowText(info.Desc);
	m_editExec.SetWindowText(info.Exec);
	m_editWorkDir.SetWindowText(info.WorkDir);
	m_editArgs.SetWindowText(info.Args);
	m_cbMode.SetCurSel(info.Mode);
}

void CServicesDlg::DumpService(int nCur)
{
	ServiceNodeEx& info = m_Nodes[nCur];

	m_editName.GetWindowText(info.Name, RC_MAX_NAME_LEN);
	m_editDesc.GetWindowText(info.Desc, RC_MAX_DESC_LEN);
	m_editExec.GetWindowText(info.Exec, RC_MAX_PATH);
	m_editWorkDir.GetWindowText(info.WorkDir, RC_MAX_PATH);
	m_editArgs.GetWindowText(info.Args, RC_MAX_PATH);

	info.Mode = (ServiceMode)m_cbMode.GetCurSel();
}

void CServicesDlg::OnBnClickedButtonAdd()
{
	int nCur = m_listServices.GetItemCount();
	char name[RC_MAX_NAME_LEN];
	sprintf(name, "Services %d", nCur);
	int n = m_listServices.InsertItem(nCur, name);
	strcpy(m_Nodes[n].Name, name);
	strcpy(m_Nodes[n].Desc, "");
	strcpy(m_Nodes[n].Exec, "");
	strcpy(m_Nodes[n].WorkDir, "");
	strcpy(m_Nodes[n].Args, "");
	m_Nodes[n].New = true;
	m_listServices.SetItemState(nCur, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
	BindService(nCur, true);
}


void CServicesDlg::OnBnClickedButtonDel()
{
	if (m_CurSel == -1) {
		return;
	}

	if (LVIS_SELECTED != m_listServices.GetItemState(m_CurSel, LVIS_SELECTED)) {
		MessageBox("EEEEE");
		return;
	}

	ServiceNode& info = m_Nodes[m_CurSel];
	if (strlen(info.Name) > 0) {
		int rc = m_pApi->Delete(info.Name);
		if (rc != 0) {
			MessageBox("Delete Service Failed");
			return;
		}
	}
	m_listServices.DeleteItem(m_CurSel);
	for (int i = m_CurSel + 1; i < 64; ++i) {
		m_Nodes[i - 1] = m_Nodes[i];
	}
}


void CServicesDlg::OnBnClickedButtonSave()
{
	if (m_CurSel == -1) {
		return;
	}

	if (LVIS_SELECTED != m_listServices.GetItemState(m_CurSel, LVIS_SELECTED)) {
		MessageBox("EEEEE");
		return;
	}
	DumpService(m_CurSel);

	ServiceNode& info = m_Nodes[m_CurSel];
	if (m_Nodes[m_CurSel].New) {
		int rc = m_pApi->Add(&info);
		if (rc < 0) {
			MessageBox("Add Device Failed");
		}
		else {
			MessageBox("Add Device Done!");
			m_Nodes[m_CurSel].New = false;
		}
	}
	else {
		int rc = m_pApi->Modify(&info);
		if (rc != 0) {
			MessageBox("Modify Device Failed");
		}
		else {
			MessageBox("Modify Device Done!");
		}
	}
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


void CServicesDlg::OnBnClickedButtonAcc()
{
	if (m_CurSel == -1) {
		return;
	}

	if (LVIS_SELECTED != m_listServices.GetItemState(m_CurSel, LVIS_SELECTED)) {
		return;
	}

	ServiceNode& info = m_Nodes[m_CurSel];
	sprintf(info.Name, "Acc Server %d", m_CurSel);
	sprintf(info.Desc, "Default Acc Server %d", m_CurSel);
	sprintf(info.Exec, "AccServer.exe");
	sprintf(info.WorkDir, "");
	std::string bip = "127.0.0.1";
	int port_rep = 6600;
	int port_pub = 6601;
	int port_stream = 6602;
	sprintf(info.Args, "%s %d %d %d", bip.c_str(), port_rep, port_pub, port_stream);

	BindService(m_CurSel, true);
}


void CServicesDlg::OnBnClickedButtonStream()
{
	if (m_CurSel == -1) {
		return;
	}

	if (LVIS_SELECTED != m_listServices.GetItemState(m_CurSel, LVIS_SELECTED)) {
		return;
	}

	ServiceNode& info = m_Nodes[m_CurSel];
	sprintf(info.Name, "Stream Server %d", m_CurSel);
	sprintf(info.Desc, "Default Stream Server %d", m_CurSel);
	sprintf(info.Exec, "StreamServer.exe");
	sprintf(info.WorkDir, "");
	int id = RC_STREAM_SERVER_ID_BASE + m_CurSel;
	std::string sip = "127.0.0.1";
	int sport = 6602;
	std::string bip = "127.0.0.1";
	int bport = 6800;
	sprintf(info.Args, "%d %s %d %s %d", id, sip.c_str(), sport, bip.c_str(), bport);

	BindService(m_CurSel, true);
}
