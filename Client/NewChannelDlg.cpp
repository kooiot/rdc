// NewChannelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "NewChannelDlg.h"
#include "afxdialogex.h"
#include "ChannelGrid.h"

// CNewChannelDlg 对话框

IMPLEMENT_DYNAMIC(CNewChannelDlg, CDialogEx)

CNewChannelDlg::CNewChannelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_NEW_CHANEL_DLG, pParent)
{

}

CNewChannelDlg::~CNewChannelDlg()
{
}

void CNewChannelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHANNEL_PROPS, m_wndPropList);
}


BEGIN_MESSAGE_MAP(CNewChannelDlg, CDialogEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CNewChannelDlg 消息处理程序


void CNewChannelDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnOK();
}


BOOL CNewChannelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	InitPropList();

	// For column size of PropListGrip
	CRect rect;
	m_wndPropList.GetWindowRect(&rect);
	ScreenToClient(rect);
	m_wndPropList.SetWindowPos(NULL, rect.left - 1, rect.top - 1, rect.Width() + 2, rect.Height() + 2, SWP_NOACTIVATE | SWP_NOZORDER);
	return TRUE;
}


void CNewChannelDlg::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("信道类型"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("类型"), _T("串口"), _T("信道类型选择"));
	pProp->AddOption(_T("串口"));
	pProp->AddOption(_T("TCP连接"));
	pProp->AddOption(_T("UDP"));
	pProp->AddOption(_T("插件"));
	pProp->AddOption(_T("测试"));
	pProp->AllowEdit(FALSE);

	pGroup1->AddSubItem(pProp);
	m_wndPropList.AddProperty(pGroup1);

	m_wndPropList.AddProperty(ChannelGrid::CreateSerial());
	m_wndPropList.AddProperty(ChannelGrid::CreateTcp());
	m_wndPropList.AddProperty(ChannelGrid::CreateUdp());
	m_wndPropList.AddProperty(ChannelGrid::CreatePlugin());
}

void CNewChannelDlg::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}


int CNewChannelDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
