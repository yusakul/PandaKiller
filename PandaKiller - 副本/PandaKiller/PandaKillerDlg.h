
// PandaKillerDlg.h: 头文件
//

#pragma once


// CPandaKillerDlg 对话框
class CPandaKillerDlg : public CDialogEx
{
// 构造
public:
	CPandaKillerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PANDAKILLER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editCtrl;

	//1.在内存中查找病毒是否存在
	bool FindTargetProcess(char * pszProcessName, DWORD * dwpid);

	//2.提升权限,访问一些受限制的系统资源
	bool EnableDebugPrivilege(char * pszPrivilege);
	afx_msg void OnBnClickedButton1();
	CString m_edit;
	
};
