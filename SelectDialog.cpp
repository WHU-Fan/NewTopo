// SelectDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "NewTopo.h"
#include "SelectDialog.h"
#include "afxdialogex.h"


// CSelectDialog 对话框

IMPLEMENT_DYNAMIC(CSelectDialog, CDialogEx)

CSelectDialog::CSelectDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SELECTDIALOG, pParent)
	, m_SelectID(0)
{

}

CSelectDialog::~CSelectDialog()
{
}

void CSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_SelectID);
}


BEGIN_MESSAGE_MAP(CSelectDialog, CDialogEx)
END_MESSAGE_MAP()


// CSelectDialog 消息处理程序
