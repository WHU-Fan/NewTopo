#pragma once


// CSelectDialog 对话框

class CSelectDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectDialog)

public:
	CSelectDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelectDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECTDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 查询某个面的id，然后高亮显示
	int m_SelectID;
};
