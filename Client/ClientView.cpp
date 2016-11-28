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

// ClientView.cpp : CClientView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Client.h"
#endif

#include "ClientDoc.h"
#include "ClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientView

IMPLEMENT_DYNCREATE(CClientView, CListView)

BEGIN_MESSAGE_MAP(CClientView, CListView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CClientView 构造/析构

CClientView::CClientView()
{
	// TODO: 在此处添加构造代码

}

CClientView::~CClientView()
{
}

BOOL CClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CListView::PreCreateWindow(cs);
}

void CClientView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	CListCtrl& list = GetListCtrl();

	CImageList* pImageList = new CImageList();

	pImageList->Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 4);
	CBitmap bmp;
	bmp.LoadBitmap(IDB_PAGES_SMALL_HC);
	pImageList->Add(&bmp, RGB(255, 0, 255));
	list.SetImageList(pImageList, LVSIL_SMALL);

	list.ModifyStyle(0, LVS_REPORT | LVS_ALIGNLEFT | WS_TABSTOP);
	//list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES | LVS_EX_ONECLICKACTIVATE);
	list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
	
	list.InsertColumn(0, "ICON", LVCFMT_LEFT, 40);//插入列
	list.InsertColumn(1, "ID", LVCFMT_LEFT, 120);//插入列
	list.InsertColumn(2, "NAME", LVCFMT_LEFT, 200);
	int nRow = list.InsertItem(0, "0");//插入行
	list.SetItem(nRow, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0);
	list.SetItemText(nRow, 1, "jacky");//设置数据
	nRow = list.InsertItem(1, "222");//插入行
	list.SetItemText(nRow, 1, "Dirk");//设置数据
}

void CClientView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CClientView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CClientView 诊断

#ifdef _DEBUG
void CClientView::AssertValid() const
{
	CListView::AssertValid();
}

void CClientView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CClientDoc* CClientView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClientDoc)));
	return (CClientDoc*)m_pDocument;
}
#endif //_DEBUG

