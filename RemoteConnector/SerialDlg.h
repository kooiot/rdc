#pragma once


// CSerialDlg �Ի���

class CSerialDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSerialDlg)

public:
	CSerialDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSerialDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERIAL_DIALOG };
#endif

	SerialInfo m_Info;
	bool m_Editable;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
