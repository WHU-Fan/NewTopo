
// NewTopoView.h : CNewTopoView 类的接口
//

#include <stdlib.h>
#include <crtdbg.h>
#include<string>
#include<sstream>
#include<iostream>
#include"GeoLayer.h"
#include"GeoMap.h"
#include"SelectDialog.h"
#include"AttFindDialog.h"
#include"MainFrm.h"
using namespace std;

#pragma once


//#define MAPSCALE  1000//wmj

#define  ZOOMIN    10001
#define  ZOOMOUT   10002
#define  PAN       10003
#define  TRANSLATE 10004
#define  ONWHOLE   10005
#define  TOPOBUILD 10006
#define  SELECT 10007
#define  POLYGONSELECT 10008
#define  ADDLAYER   10009
#define  ADJOINPOLYGONQUERY 10010
//#define SIMPLIFY   10007


#include "myDCConfig.h"
struct RecordItem {
	BYTE name[11];
	BYTE fieldType;
	int Reserved3;
	BYTE fieldLength;
	BYTE decimalCount;
	short Reserved4;
	BYTE workID;
	short Reserved5[5];
	BYTE mDXFlag1;
};
class CNewTopoView : public CView
{
protected: // 仅从序列化创建
	CNewTopoView();


	DECLARE_DYNCREATE(CNewTopoView)

// 特性
public:
	CNewTopoDoc* GetDocument() const;
	CMyDCConfig dcCFG;
// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	int MAPSCALE;
	int currrentOperateStatus;
	BOOL isMouseDown;

	CDC* dcMem;
	CGeoMap *map;	
	CBitmap bmp;
	CRect rectWC;//屏幕范围内的逻辑Rect;top<bottom,left<right
	CPoint pt1,pt2 ;
	CGeoPolyline* pline11;
	CGeoPolyline* pline22;
	CRect oldRect;//不懂
	int selectID;
	vector<RecordItem> recordItems;
	bool isTopo;//是否完成拓扑
	int OnChangeByteOrder(int indata);
	void readData(CString& filename,int& layerIndex);//读数据
	void OnReadDbf(CString filename,int layerIndex);
	void DBFintoSHP(vector<CString>cc, vector<double>shapeArea,int layerIndex);//将读取到的dbf数据加到shp文件中对应的图层中
	virtual ~CNewTopoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnZoomin();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnUpdateZoomin(CCmdUI *pCmdUI);
	afx_msg void OnWhole();
	afx_msg void OnTranslation();
	afx_msg void OnUpdateTranslation(CCmdUI *pCmdUI);
	afx_msg void OnTopobuild();
	
	afx_msg void OnSimplify();
	afx_msg void OnPolygonsimplify();
	afx_msg void OnAddorimap();
	afx_msg void OnToposave();
	void WriteShp(CString& filename );
	// 将十进制转换成十六进制，并能被程序读取
	int OnChangeByteOrderTenToSixteen(int indate);
//	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	// 双缓冲绘图
	//bool OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	// 完成对dbf的输出
	void WriteDbf(CString filename);
	afx_msg void OnSelectpolygonid();
	afx_msg void OnFindattribute();
	afx_msg void OnMergegonsarea();
	afx_msg void OnMergepolygonsameatt();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	bool IsSizeChanged;
	afx_msg void OnPolygonSelect();
	// 通过鼠标点击选择某个面，同时高亮显示
	void MouseSelectPolygon(CPoint pt);
	afx_msg void OnAdddata();
	afx_msg void OnPartTopo();
//	afx_msg void OnBuildSimple();
	afx_msg void OnBuildsimple();
	afx_msg void OnRefineroad();
	afx_msg void OnAdjoinpolygonquery();
	afx_msg void OnFilter();
	afx_msg void OnFilterByArea();
	afx_msg void OnFindRectangle();
	afx_msg void OnMABR();
	afx_msg void OnOptimize();
	afx_msg void OnRemoveredundant();
	afx_msg void OnFinal();
	afx_msg void OnDeletetail();
};

#ifndef _DEBUG  // NewTopoView.cpp 中的调试版本
inline CNewTopoDoc* CNewTopoView::GetDocument() const
   { return reinterpret_cast<CNewTopoDoc*>(m_pDocument); }
#endif

