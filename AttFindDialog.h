#pragma once
#include "afxwin.h"


// CAttFindDialog 对话框

class CAttFindDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAttFindDialog)

public:
	CAttFindDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAttFindDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOGFINDATTRIBUTE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_comboBox;
	CString attribute;

	afx_msg void OnSelchangeCombo1();
	afx_msg void OnBnClickedButtoncolor();
	COLORREF lineColor;
	CEdit m_showColor;
};
