// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���  
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�  
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ������� 
// http://go.microsoft.com/fwlink/?LinkId=238214��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// ClientView.cpp : CClientView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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

// CClientView ����/����

CClientView::CClientView()
{
	// TODO: �ڴ˴���ӹ������

}

CClientView::~CClientView()
{
}

BOOL CClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

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
	
	list.InsertColumn(0, "ICON", LVCFMT_LEFT, 40);//������
	list.InsertColumn(1, "ID", LVCFMT_LEFT, 120);//������
	list.InsertColumn(2, "NAME", LVCFMT_LEFT, 200);
	int nRow = list.InsertItem(0, "0");//������
	list.SetItem(nRow, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0);
	list.SetItemText(nRow, 1, "jacky");//��������
	nRow = list.InsertItem(1, "222");//������
	list.SetItemText(nRow, 1, "Dirk");//��������
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


// CClientView ���

#ifdef _DEBUG
void CClientView::AssertValid() const
{
	CListView::AssertValid();
}

void CClientView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CClientDoc* CClientView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClientDoc)));
	return (CClientDoc*)m_pDocument;
}
#endif //_DEBUG

