// MyDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "NewTopo.h"
#include "MyDialog.h"
#include "afxdialogex.h"


// CMyDialog 对话框

IMPLEMENT_DYNAMIC(CMyDialog, CDialogEx)

CMyDialog::CMyDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_lineTelorance(0)
{

}

CMyDialog::~CMyDialog()
{
}

void CMyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITLINE, m_lineTelorance);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
}


BEGIN_MESSAGE_MAP(CMyDialog, CDialogEx)
	ON_STN_CLICKED(IDC_STATIC1, &CMyDialog::OnStnClickedStatic1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CMyDialog::OnCustomdrawSlider1)
	ON_EN_CHANGE(IDC_EDITLINE, &CMyDialog::OnChangeEditline)
END_MESSAGE_MAP()


// CMyDialog 消息处理程序


void CMyDialog::OnStnClickedStatic1()
{
	// TODO: 在此添加控件通知处理程序代码
}


BOOL CMyDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_Slider.SetRange(0, 10);

	m_Slider.SetTicFreq(0.5);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMyDialog::OnCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_lineTelorance = m_Slider.GetPos();
	UpdateData(FALSE);
	*pResult = 0;
}


void CMyDialog::OnChangeEditline()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Slider.SetPos(m_lineTelorance);
}
