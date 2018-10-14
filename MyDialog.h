#pragma once
#include "afxcmn.h"


// CMyDialog 对话框

class CMyDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMyDialog)

public:
	CMyDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_lineTelorance;
	afx_msg void OnStnClickedStatic1();
	CSliderCtrl m_Slider;
	virtual BOOL OnInitDialog();
	afx_msg void OnCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnChangeEditline();
};
