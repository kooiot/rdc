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
	m_Info.baudrate = 9600;
	m_Info.bytesize = 8;
	m_Info.stopbits = 1;
	m_Info.flowcontrol = 0;
	m_Info.parity = 0;
}

CSerialDlg::~CSerialDlg()
{
}

void CSerialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSerialDlg, CDialogEx)
END_MESSAGE_MAP()


// CSerialDlg 消息处理程序
