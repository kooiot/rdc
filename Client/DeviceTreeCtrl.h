#pragma once
#include "afxcmn.h"
class CDeviceTreeCtrl :
	public CTreeCtrl
{
public:
	CDeviceTreeCtrl();
	virtual ~CDeviceTreeCtrl();

public:
	afx_msg void OnRBClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNewDeviceChannel(UINT id);
		
	DECLARE_MESSAGE_MAP()

};

