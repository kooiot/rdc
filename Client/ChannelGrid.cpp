#include "stdafx.h"
#include "ChannelGrid.h"
#include "PluginPropDlg.h"

ChannelGrid::ChannelGrid()
{
}


ChannelGrid::~ChannelGrid()
{
}

CMFCPropertyGridProperty * ChannelGrid::CreateLocal()
{

	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(_T("���ض˿�"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("����"), _T("����"), _T("�ŵ�����ѡ��"));
	pProp->AddOption(_T("����"));
	pProp->AddOption(_T("TCP�ͻ���"));
	pProp->AddOption(_T("TCP������"));
	pProp->AddOption(_T("UDP"));
	pProp->AddOption(_T("���"));
	pProp->AddOption(_T("����"));
	pProp->AllowEdit(FALSE);

	pGroup->AddSubItem(pProp);
	return pGroup;
}

CMFCPropertyGridProperty * ChannelGrid::CreateRemote()
{

	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(_T("�豸�˿�"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("����"), _T("����"), _T("�ŵ�����ѡ��"));
	pProp->AddOption(_T("����"));
	pProp->AddOption(_T("TCP�ͻ���"));
	pProp->AddOption(_T("TCP������"));
	pProp->AddOption(_T("UDP"));
	pProp->AddOption(_T("���"));
	pProp->AddOption(_T("����"));
	pProp->AllowEdit(FALSE);

	pGroup->AddSubItem(pProp);
	return pGroup;
}

CMFCPropertyGridProperty * ChannelGrid::CreateSerial()
{
	auto pGroup = new CMFCPropertyGridProperty(_T("���ش���"), 0, TRUE);

	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("�����豸"),
		(_variant_t)_T("ttyS1"), _T("�豸�д��ڵ��ļ���,��ttyS1 ttyUSB0 ttyM0��")));

	auto pProp = new CMFCPropertyGridProperty(_T("������"), 
		(_variant_t)9600u, _T("���ڲ�����"));
	pProp->AddOption(_T("4800"));
	pProp->AddOption(_T("9600"));
	pProp->AddOption(_T("115200"));
	pProp->AllowEdit(TRUE);
	pGroup->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("����λ"), 
		(_variant_t)8u, _T("��������λ����"));
	pProp->AddOption(_T("5"));
	pProp->AddOption(_T("6"));
	pProp->AddOption(_T("7"));
	pProp->AddOption(_T("8"));
	pProp->AllowEdit(FALSE);
	pGroup->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("ֹͣλ"), 
		(_variant_t)_T("1"), _T("����ֹͣλ����"));
	pProp->AddOption(_T("1"));
	pProp->AddOption(_T("2"));
	pProp->AddOption(_T("1.5"));
	pProp->AllowEdit(FALSE);
	pGroup->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("У��"), 
		(_variant_t)_T("��"), _T("����У�鷽ʽ"));
	pProp->AddOption(_T("��"));
	pProp->AddOption(_T("żУ��"));
	pProp->AddOption(_T("��У��"));
	pProp->AddOption(_T("��1"));
	pProp->AddOption(_T("��0"));
	pProp->AllowEdit(FALSE);
	pGroup->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("������"),
		(_variant_t)_T("��"), _T("��������������"));
	pProp->AddOption(_T("��"));
	pProp->AddOption(_T("����"));
	pProp->AddOption(_T("Ӳ��"));
	pProp->AllowEdit(FALSE);
	pGroup->AddSubItem(pProp);
	pGroup->Expand(FALSE);

	return pGroup;
}

CMFCPropertyGridProperty * ChannelGrid::CreateSerialLocal()
{
	auto pGroup = new CMFCPropertyGridProperty(_T("���ش���"), 0, TRUE);
	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("�����豸"),
		(_variant_t)_T("COM4"), _T("�������⴮������,��COM4.")));
	pGroup->Expand(FALSE);

	return pGroup;
}

CMFCPropertyGridProperty * ChannelGrid::CreateTcpClient()
{
	auto pGroup = new CMFCPropertyGridProperty(_T("TCP�ͻ���"), 0, TRUE);

	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("Զ�̵�ַ"),
		(_variant_t)_T("192.168.1.9"), _T("Զ�̷����ַ")));
	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("Զ�̶˿�"),
		(_variant_t)4000u, _T("Զ�̷���˿�")));

	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("�󶨵�ַ"),
		(_variant_t)_T("192.168.1.200"), _T("Զ���豸��������󶨵�ַ")));
	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("�󶨶˿�"),
		(_variant_t)4000u, _T("Զ���豸��������󶨶˿�")));

	pGroup->Expand(FALSE);

	return pGroup;
}
CMFCPropertyGridProperty * ChannelGrid::CreateTcpServer()
{
	auto pGroup = new CMFCPropertyGridProperty(_T("TCP������"), 0, TRUE);
	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("�󶨵�ַ"),
		(_variant_t)_T("0.0.0.0"), _T("��������󶨵�ַ")));
	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("�󶨶˿�"),
		(_variant_t)4000u, _T("��������󶨶˿�")));
	pGroup->Expand(FALSE);

	return pGroup;
}

CMFCPropertyGridProperty * ChannelGrid::CreateUdp()
{
	auto pGroup = new CMFCPropertyGridProperty(_T("UDP�˿�"), 0, TRUE);
	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("Զ�̵�ַ"),
		(_variant_t)_T("192.168.1.9"), _T("Զ�̷����ַ")));
	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("Զ�̶˿�"),
		(_variant_t)4000u, _T("Զ�̷���˿�")));

	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("�󶨵�ַ"),
		(_variant_t)_T("192.168.1.200"), _T("Զ���豸��������󶨵�ַ")));
	pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("�󶨶˿�"),
		(_variant_t)4000u, _T("Զ���豸��������󶨶˿�")));
	pGroup->Expand(FALSE);

	return pGroup;
}

CMFCPropertyGridProperty * ChannelGrid::CreatePlugin()
{
	auto pGroup = new CMFCPropertyGridProperty(_T("���"));
	auto pProp = new CMFCPropertyGridProperty(_T("���"),
		(_variant_t)_T("Example"), _T("���ѡ��"));
	pProp->AddOption(_T("MultiNet"));
	pProp->AddOption(_T("Example"));
	pProp->AllowEdit(FALSE);
	pGroup->AddSubItem(pProp);

	pGroup->AddSubItem(new CMFCPropertyGridPluginProperty(_T("���ز������"),
		pProp, (_variant_t)_T(""), _T("Զ�˲������")));
	pGroup->AddSubItem(new CMFCPropertyGridPluginProperty(_T("���ز������"), 
		pProp, (_variant_t)_T(""), _T("Զ�˲������")));


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