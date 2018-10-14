// AttFindDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "NewTopo.h"
#include "AttFindDialog.h"
#include "afxdialogex.h"


// CAttFindDialog 对话框

IMPLEMENT_DYNAMIC(CAttFindDialog, CDialogEx)

CAttFindDialog::CAttFindDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOGFINDATTRIBUTE, pParent)
{

}

CAttFindDialog::~CAttFindDialog()
{
}

void CAttFindDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_comboBox);
	DDX_Control(pDX, IDC_EDIT1, m_showColor);
}


BEGIN_MESSAGE_MAP(CAttFindDialog, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CAttFindDialog::OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTONCOLOR, &CAttFindDialog::OnBnClickedButtoncolor)
END_MESSAGE_MAP()


// CAttFindDialog 消息处理程序


BOOL CAttFindDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	lineColor = RGB(32, 190, 190);
	m_comboBox.AddString(_T("0110"));

	m_comboBox.AddString(_T("0210"));

	m_comboBox.AddString(_T("0212"));
	m_comboBox.AddString(_T("0213"));

	m_comboBox.AddString(_T("0230"));

	m_comboBox.AddString(_T("0290"));
	m_comboBox.AddString(_T("0300"));

	m_comboBox.AddString(_T("0310"));

	m_comboBox.AddString(_T("0322"));
	m_comboBox.AddString(_T("0323"));

	m_comboBox.AddString(_T("0330"));

	m_comboBox.AddString(_T("0400"));
	m_comboBox.AddString(_T("0411"));

	m_comboBox.AddString(_T("0412"));

	m_comboBox.AddString(_T("0500"));
	m_comboBox.AddString(_T("0520"));
	m_comboBox.AddString(_T("0550"));
	m_comboBox.AddString(_T("0600"));
	m_comboBox.AddString(_T("0640"));
	m_comboBox.AddString(_T("0700"));
	m_comboBox.AddString(_T("0710"));
	m_comboBox.AddString(_T("0712"));
	m_comboBox.AddString(_T("0713"));
	m_comboBox.AddString(_T("0719"));
	m_comboBox.AddString(_T("0720"));
	m_comboBox.AddString(_T("0820"));
	m_comboBox.AddString(_T("0950"));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CAttFindDialog::OnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_comboBox.GetCurSel();

	CString strCBText;

	m_comboBox.GetLBText(nIndex, strCBText);
	attribute = strCBText;
}


void CAttFindDialog::OnBnClickedButtoncolor()
{
	// TODO: 在此添加控件通知处理程序代码
	lineColor = RGB(32, 190, 190);      // 颜色对话框的初始颜色为红色  
	CColorDialog colorDlg(lineColor);         // 构造颜色对话框，传入初始颜色值   

	if (IDOK == colorDlg.DoModal())       // 显示颜色对话框，并判断是否点击了“确定”   
	{
		lineColor = colorDlg.GetColor();      // 获取颜色对话框中选择的颜色值   
		
	}
	BYTE Red = GetRValue(lineColor); ///得到红颜色
	BYTE Green = GetGValue(lineColor); ///得到绿颜色
	BYTE Blue = GetBValue(lineColor); ///得到兰颜色
	char chR[4];
	itoa(Red, chR, 10);

	char chG[4];
	itoa(Green, chG, 10);

	char chB[4];
	itoa(Blue, chB, 10);

	CString strR, strG, strB;
	strR = chR;
	strG = chG;
	strB = chB;

	CString strRGBText = strR + "," + strG + "," + strB;
	m_showColor.SetWindowText(strRGBText);
	
}
