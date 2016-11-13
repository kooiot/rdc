#pragma once

#include "afxpropertygridctrl.h"

class ChannelGrid
{
public:
	ChannelGrid();
	~ChannelGrid();
	static CMFCPropertyGridProperty* CreateSerial();
	static CMFCPropertyGridProperty* CreateTcp();
	static CMFCPropertyGridProperty* CreateUdp();
	static CMFCPropertyGridProperty* CreatePlugin();
};


class CMFCPropertyGridPluginProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridPluginProperty)

	// Construction
public:

	CMFCPropertyGridPluginProperty(const CString& strName,
		const CMFCPropertyGridProperty* pPluginProp,
		const CString& strContent,
		LPCTSTR lpszDescr = NULL,
		DWORD_PTR dwData = 0);
	virtual ~CMFCPropertyGridPluginProperty();

	// Overrides
public:
	virtual void OnClickButton(CPoint point);

	// Attributes
protected:
	const CMFCPropertyGridProperty* m_pPluginProp;
};