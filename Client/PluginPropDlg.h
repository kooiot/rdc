#pragma once


// CPluginPropDlg 对话框

class CPluginPropDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPluginPropDlg)

public:
	CPluginPropDlg(CString strConfig, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPluginPropDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLUGIN_PROP_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
	CString GetConfig() const;

private:
	CString m_strConfig;
};
