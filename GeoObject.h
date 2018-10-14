#pragma once
#include<vector>
#include"MyDCConfig.h"
#include "atltypes.h"
using namespace std;

struct SegMent {

	CPoint ptOri;
	CPoint ptEnd;
	SegMent(CPoint pt1, CPoint pt2){
		ptOri = pt1;
		ptEnd = pt2;
	}
	void setSegMent(CPoint pt1, CPoint pt2) {
		ptOri = pt1;
		ptEnd = pt2;
	}
	bool operator < (const SegMent &m)const {
		return ptOri.x < m.ptOri.x;
	}
};
class CGeoObject
{
public:
	CGeoObject();
	~CGeoObject();
	unsigned long int MABRpos;
	bool isNULL;
	unsigned long int PosinCommonLayer;//对象在Map中数组objects中的位置
	CString objectAttribute;//属性
	
	CRect* objectMABR;//最小外接矩形
	vector<long int> intersectMABRpos;//与之相交最小外接矩形在Map中objectMABR中的位置
	vector<long int> intersectMABRposCopy;

	BOOL isObjectMABRintersect(CGeoObject* geoObject);//两个对象的最小外接矩形是否相交
	void BesidesObjectSearch();//周围要素搜索
	void TopoBuild();//拓扑建立
	void ObjectMerge();//要素合并
	void ObjectDelete();//要素删除
	void ObjectSimplify();//要素化简
	void ObjectShift();//要素移位
	
	virtual CRect* GetObjectMABR();
	virtual int GetCircleNum();//得到子环数目	
	//void getIntersectMABRposCopy();//副本赋值

	virtual void Draw(CDC * pDC){};//要素绘制
	virtual void Draw(CDC* pDC, float width, int lineStyle, COLORREF lineColor, int fillstyle, COLORREF fillColor);//
	virtual void Draw(CDC* pdc, CMyDCConfig myDCcfg);//
													 // 一个面建立拓扑后的范围
	CRect objectTopoRect;
	// // 得到一个顶点为pt1,pt2,pt3的三角形面的面积
	double getTriArea(CPoint pt1, CPoint pt2, CPoint pt3);
	// 得到两点之间距离
	double getTwoPointDis(CPoint pt1, CPoint pt2);
	// 计算a与b的整倍数的最小差的绝对值
	double getMod(double a, double b);
	// pt1表示线段起点，pt2表示线段终点，pt表示所求点,返回的是π制
	double getAngle(CPoint pt1, CPoint pt2, CPoint pt);
	//  pt1和pt2分别是一条线段的起点和终点，判断pt在线段的左右侧，1返回左侧，-1在线上，0返回右侧
	int ptPostoLine(CPoint pt1, CPoint pt2, CPoint pt);
	// 点到直线的最短距离
	double MinsPointToLine(CPoint pt, CPoint ptOri, CPoint ptEnd);
	// 得到不平行的两条直线的交点
	CPoint getLineCrossPt(SegMent segMent1, SegMent segMent2);
	// 判断pt1和pt4是否在链pt2-pt3的同侧，若同侧返回1，异侧返回-1，有一点或者两点均在链上，返回0
	int getStyleFourptsUnit(CPoint pt1, CPoint pt2, CPoint pt3, CPoint pt4);

	// zf,0623,按点数过滤
	virtual void ObjectFilter(void);
	// zf，0701，按面积过滤
	virtual void ObjectFilterByArea(void);
	// 返回点数数组，zf，0701
	virtual vector<int> getPointNum(void);
	// 返回面积数组，zf，0701
	virtual vector<double> getCircleArea(void);
	// 寻找一般的外接矩形，zf，0704
	virtual void FindRectangle(void);
	// zf,0708,最小获取外接矩形
	virtual void FindMABR(void);
	// 最小外接矩形优化，0712，zf
	virtual bool optimize(void);
	// 去除冗余点，zf，0717
	virtual void RemoveRedundant(void);
	// zf 1003
	virtual void DeleteTail(void);
};

