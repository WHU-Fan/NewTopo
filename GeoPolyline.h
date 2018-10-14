#pragma once
#include "GeoObject.h"
#include"GeoPoint.h"
#include<vector>
#include"MyPoint.h"
#include"MyDCConfig.h"
#include"MABR_ZF.h"
#include "atltypes.h"
using namespace std;
typedef struct tagCOLINFO
{
	USHORT from;
	USHORT to;
	BYTE   nItem;//用于栅格处理时为赋属性值
}COLINFO;
typedef CArray<COLINFO, COLINFO> ColInfoArray;
typedef CArray<CPoint, CPoint> CPointArray;
class CGeoPolyline :
	public CGeoObject
{
public:
	CGeoPolyline();
	CGeoPolyline(CGeoPolyline & polyline);
	//CGeoPolyline(CGeoPolyline ** polyline);
	~CGeoPolyline();

	int leftPolygonID;//左面ID
	int rightPolygonID;//右面ID
	CString leftAttribute;//左面属性
	CString rightAttribute;//右面属性
	int polygonID;//未拓扑前，该链所在面的ID
	int circleID;//未拓扑前，该环在circle数组中的下标
	vector<CMyPoint*> pts;
	//vector<int>endPointPos;//端点下标
	bool canDeleted;
	

	void AddPoint(CMyPoint*);
	void setPoint(int index, CMyPoint* mypoint);
	void Draw(CDC* pDC);
	void Draw(CDC* pDC, CMyDCConfig myDCcfg);
	int GetSize();
	CMyPoint* GetPointAt(int index);
	void removePoints(int index1, int index2, int  posInCommonLayer);
	//点到线段的最短距离
	double MinsPointToSeg(CMyPoint* pt, CMyPoint* ptOri,CMyPoint* ptEnd);
	//double distanceBetweenTwoPoints(CMyPoint* pt1, CMyPoint* pt2);
	int getMaxArcLength(vector<CMyPoint*> pts,int start,int end, double& maxLength);
	int getFarestPointNum(vector<CMyPoint*> pts);
	void DouglasPeuckerDataCompress(double tolerance);
	void DouglasPeuckerDataCompress(vector<CMyPoint*> pts,CUIntArray& aPtNum, double tolerance);
	bool JudgeCanDeleted();
	// 是否进入公共环
	int isInCommonCircle;//0代表未入环，1代表是首点与上一环相连，2代表是尾点与上一环相连
	// 得到某条链的长度
	double getLineLength();
	// 得到一个点与一条链的夹角
	double getAnglePtToLine(CPoint pt);
	// pt1表示线段起点，pt2表示线段终点，pt表示所求点
	//double getAngle(CPoint pt1, CPoint pt2, CPoint pt);
	// pt1表示线段起点，pt2表示线段终点，pt表示所求点，判断pt1在pt和pt2连线的左右侧，左侧返回-1，右侧返回1，在上返回0
	int getPtLRtoLine(CPoint pt1, CPoint pt2, CPoint pt);

	// // 移除多边形的冗余点，当顶角小于angle同时与两边夹三角形小于area，可以删除该顶点，同时提示备份数据，要注意端点不可删
	void buildLineSimple(double angle, double area,double length);
	// 计算a与b的整数倍的最小差的绝对值
	//double getMod(double a, double b);
	//int getStyleFourptsUnit(CPoint pt1, CPoint pt2, CPoint pt3, CPoint pt4);
	// 建筑物邻近四点去短边处理
	void buildFourptDelshortline(CMyPoint* pt1, CMyPoint* pt2, CMyPoint* pt3, CMyPoint* pt4);
	// 建筑物邻近四点Z型去短边处理
	void buildFourptDelShortlineZ(CMyPoint* pt1, CMyPoint* pt2, CMyPoint* pt3, CMyPoint* pt4);
	// 建筑物邻近四点U型去短边处理
	void buildFourptDelShortlineU(CMyPoint* pt1, CMyPoint* pt2, CMyPoint* pt3, CMyPoint* pt4);
	// 判断四点是否是瓶口型
	bool isBottleNeck(CPoint pt1, CPoint pt2, CPoint pt3, CPoint pt4);
	// 得到链中第i点前离i最近的有效点下标
	int getPrevalidPtNum(int x);
	// 得到链中第x点后第一个有效点的下标
	int getNextvalidPtNum(int x);
	// 将MyPoint类型数组转为CPoint数组
	bool MyPointsToCPoints(vector<CMyPoint*> pts, CPointArray &points);
	// zf,0623,过滤线
	void ObjectFilter(void);
	// //zf,计算某个环的面积
	double getArea(vector<CMyPoint*> pts);
	// zf.获取最小Y
	double getMinY(vector<CMyPoint*> pts);
	// zf，过滤
	void ObjectFilterByArea(void);
	// 寻找一般的外接矩形，zf，0704
	void FindRectangle(void);

	//环的外接多边形数组，zf，0708
	vector<MABR_ZF*> MABR;
	//环的最小外接矩形，zf，0708,未旋转回来
	MABR_ZF finalMABR;
	// ordinary Points 正常的点集，zf，0708
	vector<CPoint*> odinPts;
	// 将CMyPoint数组转化为CPoint类
	void MyPoint2Point(vector<CMyPoint*> pts);

	// 环的面积占最小外接矩形的面积比例，zf，0712
	double areaProportion;
	// 环的中心（重心），zf，0712
	CPoint center;
	// 设定环的中心，zf，0712
	void setCenter(CPoint* pt);

	// 环的两点间平均距离，zf，1003
	//double avgDis;
	// 计算两点间平均距离，zf,1003
	double calAvgDis(vector<CMyPoint*> pts);
	// 计算两点间距离，zf，1003
	double calDis(CMyPoint* pt1, CMyPoint* pt2);
	// 下标差阈值
	int pointDisThreshold;
};

