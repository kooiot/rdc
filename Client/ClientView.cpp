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

IMPLEMENT_DYNCREATE(CClientView, CTreeView)

BEGIN_MESSAGE_MAP(CClientView, CTreeView)
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

	return CTreeView::PreCreateWindow(cs);
}

void CClientView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	CTreeCtrl& ctrl = this->GetTreeCtrl();
	ctrl.ModifyStyle(0, TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS);

	HTREEITEM root = ctrl.InsertItem("Login", 0, 0);

	auto login = GetDocument()->GetLoginInfo();
	HTREEITEM item = ctrl.InsertItem(login.strSrvIP, 0, 0, root);
	ctrl.InsertItem(login.strSrvPort, root);
	ctrl.InsertItem(login.strUserName, root);
	ctrl.InsertItem(login.strPasswd, root);
	ctrl.Expand(root, TVE_EXPAND);
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
	CTreeView::AssertValid();
}

void CClientView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CClientDoc* CClientView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClientDoc)));
	return (CClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CClientView ��Ϣ�������
