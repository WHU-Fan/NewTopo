// PolygonSimplifyDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "NewTopo.h"
#include "PolygonSimplifyDialog.h"
#include "afxdialogex.h"


// CPolygonSimplifyDialog 对话框

IMPLEMENT_DYNAMIC(CPolygonSimplifyDialog, CDialogEx)

CPolygonSimplifyDialog::CPolygonSimplifyDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	, MinBuildingArea(0)
	, MinGardenArea(0)
	, m_MinParkArea(0)
	, m_MinQitayuandiArea(0)
{

}

CPolygonSimplifyDialog::~CPolygonSimplifyDialog()
{
}

void CPolygonSimplifyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITBUILDINGAREA, MinBuildingArea);
	DDX_Text(pDX, IDC_EDITGARDENAREA, MinGardenArea);
	DDX_Text(pDX, IDC_EDIT2, m_MinParkArea);
	DDX_Text(pDX, IDC_QITAYUANDI, m_MinQitayuandiArea);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider_building);
	DDX_Control(pDX, IDC_SLIDER2, m_Slider_Garden);
}


BEGIN_MESSAGE_MAP(CPolygonSimplifyDialog, CDialogEx)
	ON_EN_CHANGE(IDC_EDITGARDENAREA, &CPolygonSimplifyDialog::OnEnChangeEditgardenarea)
	ON_BN_CLICKED(2, &CPolygonSimplifyDialog::OnBnClicked2)
END_MESSAGE_MAP()


// CPolygonSimplifyDialog 消息处理程序


void CPolygonSimplifyDialog::OnEnChangeEditgardenarea()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CPolygonSimplifyDialog::OnBnClicked2()
{
	// TODO: 在此添加控件通知处理程序代码
}
