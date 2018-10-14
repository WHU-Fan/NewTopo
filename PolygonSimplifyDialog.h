#pragma once
#include "afxcmn.h"


// CPolygonSimplifyDialog 对话框

class CPolygonSimplifyDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CPolygonSimplifyDialog)

public:
	CPolygonSimplifyDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPolygonSimplifyDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double MinBuildingArea;
	double MinGardenArea;
	double m_MinParkArea;
	afx_msg void OnEnChangeEditgardenarea();
	afx_msg void OnBnClicked2();
	double m_MinQitayuandiArea;
	// 建筑滑块
	CSliderCtrl m_Slider_building;
	// 果园滑块
	CSliderCtrl m_Slider_Garden;
};
