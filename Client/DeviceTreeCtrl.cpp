#include "stdafx.h"
#include "DeviceTreeCtrl.h"
#include "resource.h"
#include "NewChannelDlg.h"

BEGIN_MESSAGE_MAP(CDeviceTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeCtrlEx)   
	ON_NOTIFY_REFLECT(NM_RCLICK, &CDeviceTreeCtrl::OnRBClick)
	ON_COMMAND_RANGE(ID_NEW_SERIAL_CHN, ID_NEW_TEST_CHN, &CDeviceTreeCtrl::OnNewDeviceChannel)
	//}}AFX_MSG_MAP   
END_MESSAGE_MAP()

CDeviceTreeCtrl::CDeviceTreeCtrl()
{
}


CDeviceTreeCtrl::~CDeviceTreeCtrl()
{
}

void CDeviceTreeCtrl::OnRBClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint point;
	GetCursorPos(&point);
	CPoint pointInTree = point;
	ScreenToClient(&pointInTree);
	HTREEITEM item;
	UINT flag = TVHT_ONITEM;
	item = HitTest(pointInTree, &flag);
	if (item != NULL)
	{
		SelectItem(item);

		/*if (GetParentItem(item) == NULL)
			return;*/

		/*CMenu menu;
		menu.LoadMenu(IDR_DEVICE_MENU);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this, NULL);
		*/

		//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_DEVICE_MENU, point.x, point.y, this, TRUE);
		
		CMenu menu;
		menu.LoadMenu(IDR_DEVICE_MENU);
		CMenu* pSumMenu = menu.GetSubMenu(0);

		if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CFrameWndEx)))
		{
			CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

			if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
				return;

			((CFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
			UpdateDialogControls(this, FALSE);

			// Tell parent window that we have handled this right-click message
			*pResult = 1L;
		}
	}
}

void CDeviceTreeCtrl::OnNewDeviceChannel(UINT id)
{
	CNewChannelDlg dlg;
	dlg.DoModal();
}