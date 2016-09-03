// TestPortDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RemoteConnector.h"
#include "TestPortDlg.h"
#include "afxdialogex.h"

#define WM_MSG_RECV_DATA WM_USER + 0x105
#define WM_MSG_RECV_EVENT WM_USER + 0x105

// CTestPortDlg 对话框

IMPLEMENT_DYNAMIC(CTestPortDlg, CDialogEx)

CTestPortDlg::CTestPortDlg(RC_CHANNEL channel,
	IPortHandler* Handler,
	CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TP_DIALOG, pParent)
	, IPort(channel, Handler)
{

}

CTestPortDlg::~CTestPortDlg()
{
}

int CTestPortDlg::Write(void * buf, size_t len)
{
	char* temp = new char[len];
	memcpy(temp, buf, len);
	PostMessage(WM_MSG_RECV_DATA, (WPARAM)temp, (LPARAM)len);
	return 0;
}

void CTestPortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RECV, m_editRecv);
	DDX_Control(pDX, IDC_EDIT_SEND, m_editSend);
}


BEGIN_MESSAGE_MAP(CTestPortDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CTestPortDlg::OnBnClickedButtonSend)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_MSG_RECV_DATA, &CTestPortDlg::OnShowRecvData)
END_MESSAGE_MAP()


// CTestPortDlg 消息处理程序


LRESULT CTestPortDlg::OnShowRecvData(WPARAM wParam, LPARAM lParam)
{
	char* buf = (char*)wParam;
	int len = (int)lParam;

	CString str;
	m_editRecv.GetWindowText(str);
	str += "\r\n";
	str += buf;
	m_editRecv.SetWindowText(str);
	return 0L;
}

LRESULT CTestPortDlg::OnShowRecvEvent(WPARAM wParam, LPARAM lParam)
{
	return 0L;
}

void CTestPortDlg::OnBnClickedButtonSend()
{
	CString str;
	m_editSend.GetWindowText(str);
	m_pHandler->OnRecv(m_nChannel, str.GetBuffer(), str.GetLength());
}


void CTestPortDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	IPort::OnClose();
	CDialogEx::OnClose();
}
