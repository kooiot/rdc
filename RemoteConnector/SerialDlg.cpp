// SerialDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RemoteConnector.h"
#include "SerialDlg.h"
#include "afxdialogex.h"


// CSerialDlg 对话框

IMPLEMENT_DYNAMIC(CSerialDlg, CDialogEx)

CSerialDlg::CSerialDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERIAL_DIALOG, pParent),
	m_Editable(true)
{
	sprintf(m_Info.dev, "%s", "COM8");//
	sprintf(m_LocalInfo.dev, "%s", "COM4");//
	m_Info.baudrate = 9600;
	m_Info.bytesize = eightbits;
	m_Info.stopbits = stopbits_one;
	m_Info.flowcontrol = flowcontrol_none;
	m_Info.parity = parity_none;
}

CSerialDlg::~CSerialDlg()
{
}

void CSerialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DCOM, m_editDeviceCOM);
	DDX_Control(pDX, IDC_EDIT_LCOM, m_editLocalCOM);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_cbBaudrate);
	DDX_Control(pDX, IDC_COMBO_DATABITS, m_cbDatabits);
	DDX_Control(pDX, IDC_COMBO_DATABITS2, m_cbStopbits);
	DDX_Control(pDX, IDC_COMBO_PARITY, m_cbParity);
	DDX_Control(pDX, IDC_COMBO_FLOWCTRL, m_cbFlowcontrol);
}


BEGIN_MESSAGE_MAP(CSerialDlg, CDialogEx)
END_MESSAGE_MAP()


// CSerialDlg 消息处理程序


BOOL CSerialDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_cbBaudrate.AddString("1200");
	m_cbBaudrate.AddString("2400");
	m_cbBaudrate.AddString("4800");
	m_cbBaudrate.AddString("9600");
	m_cbBaudrate.AddString("19200");

	CString str;
	str.Format("%d", m_Info.baudrate);
	m_cbBaudrate.SetWindowText(str);

	m_editLocalCOM.SetWindowText(m_LocalInfo.dev);
	m_editDeviceCOM.SetWindowText(m_Info.dev);

	m_cbDatabits.AddString("5");
	m_cbDatabits.AddString("6");
	m_cbDatabits.AddString("7");
	m_cbDatabits.AddString("8");
	m_cbDatabits.SetCurSel(m_Info.bytesize - fivebits);

	m_cbStopbits.AddString("1");
	m_cbStopbits.AddString("2");
	m_cbStopbits.AddString("1.5");
	m_cbStopbits.SetCurSel(m_Info.stopbits - stopbits_one);

	m_cbParity.AddString("无");
	m_cbParity.AddString("偶校验");
	m_cbParity.AddString("奇校验");
	m_cbParity.AddString("置1");
	m_cbParity.AddString("置0");
	m_cbParity.SetCurSel(m_Info.parity - parity_none);

	m_cbFlowcontrol.AddString("无");
	m_cbFlowcontrol.AddString("软件");
	m_cbFlowcontrol.AddString("硬件");
	m_cbFlowcontrol.SetCurSel(m_Info.flowcontrol - flowcontrol_none);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSerialDlg::OnOK()
{
	CString str;
	
	m_editDeviceCOM.GetWindowText(str);
	sprintf(m_Info.dev, "%s", str);
	
	m_editLocalCOM.GetWindowText(str);
	sprintf(m_LocalInfo.dev, "%s", str);

	m_cbBaudrate.GetWindowText(str);
	m_Info.baudrate = atoi(str);

	m_Info.bytesize = (bytesize)(fivebits + m_cbDatabits.GetCurSel());
	m_Info.stopbits = (stopbits)(stopbits_one + m_cbStopbits.GetCurSel());
	m_Info.flowcontrol = (flowcontrol)(flowcontrol_none + m_cbFlowcontrol.GetCurSel());
	m_Info.parity = (parity)(parity_none + m_cbParity.GetCurSel());

	CDialogEx::OnOK();
}
