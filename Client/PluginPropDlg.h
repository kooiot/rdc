#pragma once


// CPluginPropDlg �Ի���

class CPluginPropDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPluginPropDlg)

public:
	CPluginPropDlg(CString strConfig, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPluginPropDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLUGIN_PROP_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


public:
	CString GetConfig() const;

private:
	CString m_strConfig;
};
