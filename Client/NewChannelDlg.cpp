// NewChannelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "NewChannelDlg.h"
#include "afxdialogex.h"


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
	
	return TRUE;  
}


void CNewChannelDlg::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("外观"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("三维外观"), (_variant_t)false, _T("指定窗口的字体不使用粗体，并且控件将使用三维边框")));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("边框"), _T("对话框外框"), _T("其中之一: “无”、“细”、“可调整大小”或“对话框外框”"));
	pProp->AddOption(_T("无"));
	pProp->AddOption(_T("细"));
	pProp->AddOption(_T("可调整大小"));
	pProp->AddOption(_T("对话框外框"));
	pProp->AllowEdit(FALSE);

	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("标题"), (_variant_t)_T("关于"), _T("指定窗口标题栏中显示的文本")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("窗口大小"), 0, TRUE);

	pProp = new CMFCPropertyGridProperty(_T("高度"), (_variant_t)250l, _T("指定窗口的高度"));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("宽度"), (_variant_t)150l, _T("指定窗口的宽度"));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);

	m_wndPropList.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("字体"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	_tcscpy_s(lf.lfFaceName, _T("宋体, Arial"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("字体"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("指定窗口的默认字体")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("使用系统字体"), (_variant_t)true, _T("指定窗口使用“MS Shell Dlg”字体")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("杂项"));
	pProp = new CMFCPropertyGridProperty(_T("(名称)"), _T("应用程序"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("窗口颜色"), RGB(210, 192, 254), NULL, _T("指定默认的窗口颜色"));
	pColorProp->EnableOtherButton(_T("其他..."));
	pColorProp->EnableAutomaticButton(_T("默认"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static const TCHAR szFilter[] = _T("图标文件(*.ico)|*.ico|所有文件(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("图标"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("指定窗口图标")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("文件夹"), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("层次结构"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("第一个子级"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("第二个子级"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 1"), (_variant_t)_T("值 1"), _T("此为说明")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 2"), (_variant_t)_T("值 2"), _T("此为说明")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 3"), (_variant_t)_T("值 3"), _T("此为说明")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);
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

	// TODO:  在此添加您专用的创建代码
	// InitPropList();

	return 0;
}
