// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问 
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Client.h"

#include "MainFrm.h"
#include "ClientDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_COMMAND(ID_CONNECT, &CMainFrame::OnCmdConnect)
	ON_COMMAND(ID_DISCONNECT, &CMainFrame::OnCmdDisconnect)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_DISCONNECT, &CMainFrame::OnUpdateCmdDisconnect)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CONNECT, ID_EDIT_PASSWD, &CMainFrame::OnUpdateCmdConnect)
	ON_UPDATE_COMMAND_UI_RANGE(ID_NEW_SERIAL_CHN, ID_NEW_TEST_CHN, &CMainFrame::OnUpdateCmdDisconnect)
	ON_COMMAND_RANGE(ID_NEW_SERIAL_CHN, ID_NEW_TEST_CHN, &CMainFrame::OnNewDeviceChannel)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_USER_DOC_OPENED, &CMainFrame::OnDocOpened)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	
	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 导航窗格将创建在左侧，因此将暂时禁用左侧的停靠: 
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	// 创建并设置“Outlook”导航栏: 
	//if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, m_wndTree, m_wndCalendar, 250))
	if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, 250))
	{
		TRACE0("未能创建导航窗格\n");
		return -1;      // 未能创建
	}

	// 创建标题栏: 
	if (!CreateCaptionBar())
	{
		TRACE0("未能创建标题栏\n");
		return -1;      // 未能创建
	}

	// 已创建 Outlook 栏，应允许在左侧停靠。
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	// TODO:
	m_wndCaptionBar.ShowWindow(SW_HIDE);
	m_wndStatusBar.ShowWindow(SW_HIDE);
	m_bConnected = FALSE;
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}

	// 创建属性窗口
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, int nInitialWidth)
{
	bar.SetMode2003();

	BOOL bNameValid;
	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
	ASSERT(bNameValid);
	if (!bar.Create(strTemp, this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_LEFT))
	{
		return FALSE; // 未能创建
	}

	CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();

	if (pOutlookBar == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pOutlookBar->EnableInPlaceEdit(TRUE);

	static UINT uiPageID = 1;

	// 可浮动，可自动隐藏，可调整大小，但不能关闭
	DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;

	CRect rectDummy(0, 0, 0, 0);
	const DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	m_wndOnlineTree.Create(dwTreeStyle, rectDummy, &bar, 1200);
	m_wndOfflineTree.Create(dwTreeStyle, rectDummy, &bar, 1200);
	m_wndOutdateTree.Create(dwTreeStyle, rectDummy, &bar, 1200);

	CImageList* pImageList = new CImageList();
	
	pImageList->Create(24, 24, ILC_COLOR32 | ILC_MASK, 0, 3);
	CBitmap bmp;
	bmp.LoadBitmap(IDB_DEVICES);
	pImageList->Add(&bmp, RGB(255, 0, 255));
	m_wndOnlineTree.SetImageList(pImageList, TVSIL_NORMAL);
	m_wndOfflineTree.SetImageList(pImageList, TVSIL_NORMAL);
	m_wndOutdateTree.SetImageList(pImageList, TVSIL_NORMAL);

	bNameValid = strTemp.LoadString(IDS_ONLINE_DEVICES);
	ASSERT(bNameValid);
	m_treeOnlineRoot = m_wndOnlineTree.InsertItem(strTemp, 0, 0);
	//m_wndOnlineTree.InsertItem(strTemp, 0, 0, m_treeOnlineRoot);
	bNameValid = strTemp.LoadString(IDS_OFFLINE_DEVICES);
	ASSERT(bNameValid);
	m_treeOfflineRoot = m_wndOfflineTree.InsertItem(strTemp, 0, 0);
	bNameValid = strTemp.LoadString(IDS_OUTDATE_DEVICES);
	ASSERT(bNameValid);
	m_treeOutdateRoot = m_wndOutdateTree.InsertItem(strTemp, 0, 0);

	bNameValid = strTemp.LoadString(IDS_ONLINE_DEVICES);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&m_wndOnlineTree, strTemp, 2, TRUE, dwStyle);
	bNameValid = strTemp.LoadString(IDS_OFFLINE_DEVICES);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&m_wndOfflineTree, strTemp, 2, TRUE, dwStyle);
	bNameValid = strTemp.LoadString(IDS_OUTDATE_DEVICES);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&m_wndOutdateTree, strTemp, 2, TRUE, dwStyle);
	

	bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	pOutlookBar->SetImageList(theApp.m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
	pOutlookBar->SetToolbarImageList(theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
	pOutlookBar->RecalcLayout();

	BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
	CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);

	bar.SetButtonsFont(&afxGlobalData.fontBold);

	return TRUE;
}
//
//BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMFCShellTreeCtrl& tree, CCalendarBar& calendar, int nInitialWidth)
//{
//	bar.SetMode2003();
//
//	BOOL bNameValid;
//	CString strTemp;
//	bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
//	ASSERT(bNameValid);
//	if (!bar.Create(strTemp, this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_LEFT))
//	{
//		return FALSE; // 未能创建
//	}
//
//	CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();
//
//	if (pOutlookBar == NULL)
//	{
//		ASSERT(FALSE);
//		return FALSE;
//	}
//
//	pOutlookBar->EnableInPlaceEdit(TRUE);
//
//	static UINT uiPageID = 1;
//
//	// 可浮动，可自动隐藏，可调整大小，但不能关闭
//	DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;
//
//	CRect rectDummy(0, 0, 0, 0);
//	const DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
//
//	tree.Create(dwTreeStyle, rectDummy, &bar, 1200);
//	bNameValid = strTemp.LoadString(IDS_FOLDERS);
//	ASSERT(bNameValid);
//	pOutlookBar->AddControl(&tree, strTemp, 2, TRUE, dwStyle);
//
//	calendar.Create(rectDummy, &bar, 1201);
//	bNameValid = strTemp.LoadString(IDS_CALENDAR);
//	ASSERT(bNameValid);
//	pOutlookBar->AddControl(&calendar, strTemp, 3, TRUE, dwStyle);
//
//	bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
//
//	pOutlookBar->SetImageList(theApp.m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
//	pOutlookBar->SetToolbarImageList(theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
//	pOutlookBar->RecalcLayout();
//
//	BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
//	CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);
//
//	bar.SetButtonsFont(&afxGlobalData.fontBold);
//
//	return TRUE;
//}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("未能创建标题栏\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}
void CMainFrame::OnUpdateCmdConnect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bConnected);
}
void CMainFrame::OnUpdateCmdDisconnect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bConnected);
}

void CMainFrame::OnCmdConnect()
{
	m_bConnected = TRUE;
}

void CMainFrame::OnCmdDisconnect()
{
	m_bConnected = FALSE;
}

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}

void CMainFrame::OnNewDeviceChannel(UINT id)
{
	auto doc = GetActiveDocument();
	auto view = GetActiveView();
	view->GetDocument();
}

LRESULT CMainFrame::OnDocOpened(WPARAM wParam, LPARAM lParam)
{
	auto doc = dynamic_cast<CClientDoc*>(GetActiveDocument());
	if (doc) {
		auto info = doc->GetLoginInfo();
		CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_SRV_IP));
		if (pEdit) pEdit->SetEditText(info.strSrvIP);
		pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_SRV_PORT));
		if (pEdit) pEdit->SetEditText(info.strSrvPort);
		pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_USERNAME));
		if (pEdit) pEdit->SetEditText(info.strUserName);
		pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_PASSWD));
		if (pEdit) pEdit->SetEditText(info.strPasswd);
	}
/*
	CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_SRV_IP));
	if (pEdit)
		pEdit->SetEditText(AfxGetApp()->GetProfileString("Login", "ServerIP", "kooiot.com"));
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_SRV_PORT));
	if (pEdit)
		pEdit->SetEditText(AfxGetApp()->GetProfileString("Login", "ServerPort", "6600"));
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_USERNAME));
	if (pEdit)
		pEdit->SetEditText(AfxGetApp()->GetProfileString("Login", "UserName", "User1"));
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_PASSWD));
	if (pEdit) {
		pEdit->SetEditText(AfxGetApp()->GetProfileString("Login", "Password", "Password"));
	}*/

	return 1L;
}
