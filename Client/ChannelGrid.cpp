#include "stdafx.h"
#include "ChannelGrid.h"
#include "PluginPropDlg.h"

ChannelGrid::ChannelGrid()
{
}


ChannelGrid::~ChannelGrid()
{
}

CMFCPropertyGridProperty * ChannelGrid::CreateSerial()
{

	auto pGroup = new CMFCPropertyGridProperty(_T("串口"));
	auto pLocalGroup = new CMFCPropertyGridProperty(_T("本机设置"), 0, TRUE);
	pLocalGroup->AddSubItem(new CMFCPropertyGridProperty(_T("串口"), (_variant_t)_T("COM4"), _T("本机虚拟串口名称,如COM4.")));
	pLocalGroup->Expand(FALSE);

	auto pDeviceGroup = new CMFCPropertyGridProperty(_T("远端设置"), 0, TRUE);
	pDeviceGroup->AddSubItem(new CMFCPropertyGridProperty(_T("串口"), (_variant_t)_T("ttyS1"), _T("设备中串口的文件名,如ttyS1 ttyUSB0 ttyM0等")));

	auto pProp = new CMFCPropertyGridProperty(_T("波特率"), (_variant_t)9600u, _T("串口波特率"));
	pProp->AddOption(_T("4800"));
	pProp->AddOption(_T("9600"));
	pProp->AddOption(_T("115200"));
	pProp->AllowEdit(TRUE);
	pDeviceGroup->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("数据位"), (_variant_t)8u, _T("串口数据位长度"));
	pProp->AddOption(_T("5"));
	pProp->AddOption(_T("6"));
	pProp->AddOption(_T("7"));
	pProp->AddOption(_T("8"));
	pProp->AllowEdit(FALSE);
	pDeviceGroup->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("停止位"), (_variant_t)_T("1"), _T("串口停止位长度"));
	pProp->AddOption(_T("1"));
	pProp->AddOption(_T("2"));
	pProp->AddOption(_T("1.5"));
	pProp->AllowEdit(FALSE);
	pDeviceGroup->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("校验"), (_variant_t)_T("无"), _T("串口校验方式"));
	pProp->AddOption(_T("无"));
	pProp->AddOption(_T("偶校验"));
	pProp->AddOption(_T("奇校验"));
	pProp->AddOption(_T("置1"));
	pProp->AddOption(_T("置0"));
	pProp->AllowEdit(FALSE);
	pDeviceGroup->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("流控制"), (_variant_t)_T("无"), _T("串口数据流控制"));
	pProp->AddOption(_T("无"));
	pProp->AddOption(_T("软件"));
	pProp->AddOption(_T("硬件"));
	pProp->AllowEdit(FALSE);
	pDeviceGroup->AddSubItem(pProp);
	pDeviceGroup->Expand(FALSE);

	pGroup->AddSubItem(pDeviceGroup);
	pGroup->AddSubItem(pLocalGroup);
	return pGroup;
}

CMFCPropertyGridProperty * ChannelGrid::CreateTcp()
{
	auto pGroup = new CMFCPropertyGridProperty(_T("TCP连接"));
	auto pLocalGroup = new CMFCPropertyGridProperty(_T("本机设置"), 0, TRUE);
	pLocalGroup->AddSubItem(new CMFCPropertyGridProperty(_T("绑定地址"), (_variant_t)_T("0.0.0.0"), _T("本机网络绑定地址")));
	pLocalGroup->AddSubItem(new CMFCPropertyGridProperty(_T("绑定端口"), (_variant_t)4000u, _T("本机网络绑定端口")));
	pLocalGroup->Expand(FALSE);

	auto pDeviceGroup = new CMFCPropertyGridProperty(_T("远端设置"), 0, TRUE);
	pDeviceGroup->AddSubItem(new CMFCPropertyGridProperty(_T("远程地址"), (_variant_t)_T("192.168.1.9"), _T("远程服务地址")));
	pDeviceGroup->AddSubItem(new CMFCPropertyGridProperty(_T("远程端口"), (_variant_t)4000u, _T("远程服务端口")));

	pDeviceGroup->AddSubItem(new CMFCPropertyGridProperty(_T("绑定地址"), (_variant_t)_T("192.168.1.200"), _T("远端设备本机网络绑定地址")));
	pDeviceGroup->AddSubItem(new CMFCPropertyGridProperty(_T("绑定端口"), (_variant_t)4000u, _T("远端设备本机网络绑定端口")));

	pDeviceGroup->Expand(FALSE);

	pGroup->AddSubItem(pDeviceGroup);
	pGroup->AddSubItem(pLocalGroup);
	return pGroup;
}

CMFCPropertyGridProperty * ChannelGrid::CreateUdp()
{
	auto pGroup = new CMFCPropertyGridProperty(_T("UDP"));
	auto pLocalGroup = new CMFCPropertyGridProperty(_T("本机设置"), 0, TRUE);
	pLocalGroup->AddSubItem(new CMFCPropertyGridProperty(_T("绑定地址"), (_variant_t)_T("0.0.0.0"), _T("本机网络绑定地址")));
	pLocalGroup->AddSubItem(new CMFCPropertyGridProperty(_T("绑定端口"), (_variant_t)4000u, _T("本机网络绑定端口")));
	pLocalGroup->Expand(FALSE);

	auto pDeviceGroup = new CMFCPropertyGridProperty(_T("远端设置"), 0, TRUE);
	pDeviceGroup->AddSubItem(new CMFCPropertyGridProperty(_T("远程地址"), (_variant_t)_T("192.168.1.9"), _T("远程服务地址")));
	pDeviceGroup->AddSubItem(new CMFCPropertyGridProperty(_T("远程端口"), (_variant_t)4000u, _T("远程服务端口")));

	pDeviceGroup->AddSubItem(new CMFCPropertyGridProperty(_T("绑定地址"), (_variant_t)_T("192.168.1.200"), _T("远端设备本机网络绑定地址")));
	pDeviceGroup->AddSubItem(new CMFCPropertyGridProperty(_T("绑定端口"), (_variant_t)4000u, _T("远端设备本机网络绑定端口")));
	pDeviceGroup->Expand(FALSE);


	pGroup->AddSubItem(pDeviceGroup);
	pGroup->AddSubItem(pLocalGroup);
	return pGroup;
}

CMFCPropertyGridProperty * ChannelGrid::CreatePlugin()
{
	auto pGroup = new CMFCPropertyGridProperty(_T("插件"));
	auto pProp = new CMFCPropertyGridProperty(_T("插件"), (_variant_t)_T("Example"), _T("插件选择"));
	pProp->AddOption(_T("MultiNet"));
	pProp->AddOption(_T("Example"));
	pProp->AllowEdit(FALSE);
	pGroup->AddSubItem(pProp);

	pGroup->AddSubItem(new CMFCPropertyGridPluginProperty(_T("本地插件配置"), pProp, (_variant_t)_T(""), _T("远端插件配置")));
	pGroup->AddSubItem(new CMFCPropertyGridPluginProperty(_T("本地插件配置"), pProp, (_variant_t)_T(""), _T("远端插件配置")));


	return pGroup;
}

#define AFX_PROP_HAS_BUTTON 0x0002

IMPLEMENT_DYNAMIC(CMFCPropertyGridPluginProperty, CMFCPropertyGridProperty)

CMFCPropertyGridPluginProperty::CMFCPropertyGridPluginProperty(
	const CString& strName, 
	const CMFCPropertyGridProperty* pPluginProp,
	const CString& strContent,
	LPCTSTR lpszDescr,
	DWORD_PTR dwData)
	: CMFCPropertyGridProperty(strName, COleVariant((LPCTSTR)strContent), lpszDescr, dwData)
	, m_pPluginProp(pPluginProp)
{
	m_dwFlags = AFX_PROP_HAS_BUTTON;
}

CMFCPropertyGridPluginProperty::~CMFCPropertyGridPluginProperty()
{
}

void CMFCPropertyGridPluginProperty::OnClickButton(CPoint /*point*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();

	CString strConfig(m_varValue.bstrVal);
	BOOL bUpdate = FALSE;

	CString strPluginName = m_pPluginProp->GetValue();
	// TODO: Plugin Name
	CPluginPropDlg dlg(strConfig);
	if (dlg.DoModal() == IDOK)
	{
		bUpdate = TRUE;
		strConfig = dlg.GetConfig();
	}

	if (bUpdate)
	{
		if (m_pWndInPlace != NULL)
		{
			m_pWndInPlace->SetWindowText(strConfig);
		}

		m_varValue = (LPCTSTR)strConfig;
	}

	m_bButtonIsDown = FALSE;
	Redraw();

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus();
	}
	else
	{
		m_pWndList->SetFocus();
	}
}