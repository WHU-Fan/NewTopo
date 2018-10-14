#pragma once
#include<vector>
#include"GeoObject.h"
#include"stdafx.h"
#include"GeoPolygon.h"
#include"GeoPolyline.h"
#include"GeoPoint.h"
#include"MyDCConfig.h"
#include "GeoBuilding.h"

struct LinePointPosition {//两条链比较相同时，返回第一条链的起终点，以及第二条链的起终点，均为顺时针方向
	int line1Ori;
	int line1End;
	int line2Ori;
	int line2End;
	int posInCommonLayer;
	
};
using namespace std;
class CGeoLayer
{
public:
	CGeoLayer();
	~CGeoLayer();
	bool isTopLayer;
private:
	CRect rectLWC;

public:

	vector<CMyPoint*> commonPoint;
	vector<CRect*> objectMABRs;
	vector<CGeoObject*> objects;
	vector<CGeoPolyline*> commonEdge;
	vector<CGeoObject*> objectsTemp;
	unsigned int i;
	bool isPolygonSimply;
	int IsLineSimply;
	void Draw(CDC *pdc, CMyDCConfig myCDcfg);//拓扑建立后通过公共链绘制

	CGeoObject* GetObjectAt(int);//获取第n个对象
	void LayerGeneral();//图层综合
	void TopoBuild();//拓扑建立
	void AddObject(CGeoObject *obj,vector<CGeoObject*>* objs);//添加要素
	int GetObjectNum();//获取要素个数
	void BuildMABR();//将对象MABR加入到数组
	void GetObjectIntersectMABR();//求每个对象相交的最小外接矩形，并将对应在数组objectMABRs中的位置加入到该object的数组中,同属同一个图层
	void setRect(CRect rect);
	CRect getRect();
	LinePointPosition findCommonPointInLine(int polygon1, int circle1ID, int pt1ID, int polygon2, int circle2ID, int pt2ID);//当两个面上的点相等时，进行处理
	void pointCompared(CGeoPolygon* polygonObject1, CGeoPolygon* polygonObject2);//寻找相同点，同时建立拓扑，入链
	void pointCompared(CGeoPolyline* circle1, CGeoPolyline* circle2);//寻找相同点，同时建立拓扑，入链
	void pointCompared();
	void pointCompared(CGeoObject* object);
	void AddAllBoundary();//对于边界上的点和链进行入链
	void AddBoundary(CGeoPolygon *polygon);//对某个面上的点和链加入公共数组
	void AddBoundary(CGeoPolyline *circle);//对某个环上的点和链加入公共数组
	void EndPointGetValue();//将每条链的最后一个点指向起点
	void EndPointGetValue(CGeoPolygon* polygon);
	void EndPointGetValue(CGeoPolyline* polyline);
	void simplify(double telorance);
	void PolylineDeleted(vector<double> areaParameter);
	void PolylineDeleted(CGeoPolyline* polyline,int num);//删除某条链
	void PolylineDelete(CGeoPolyline* line,bool isRemainEnd);//删除一个面时，对属于该面的链删除处理
	void MergeTwoPolygon(CGeoPolygon* leftPolygon, CGeoPolygon* rightPolygon,int num);//不同面属性合并
	void MergeTwoPolygon(CGeoPolygon* leftPolygon, CGeoPolygon* rightPolygon);//不同面属性合并
	void deleteCircle();
	bool PolygonCanBeDelete(CGeoPolygon* polygon, vector<double> areaParameter);
	void ReoganizeCommonCircle();
	// 对拓扑图层更新范围
	CRect updateTopoRect();
	// 得到objects中每一个object的面积
	void getObjectArea();
	// 对TOPO数据中的误差内部环进行删除，同时产生了面积，周长等属性	void deleteErrorCircle();
	void CGeoLayer::delErrorCircle();
	// 对拓扑数据进行更新，出现在线化简与面化简的情况下
	void updateTopoLayer();
	// 相同属性面合并
	void PolylineDeleted();
	// 得到在点在图层中所在面的ID值，若都不在，返回-1
	int getSelectPolygonID(CPoint pt);
	// 判断点是否在矩形内
	//bool isPtInRect(CPoint pt, CRect rect);
	
	void buildSimple(double angle, double area, double length);
	// 构建每个面上基于拓扑结构的公共环
	void buildCircle(void);
	// 得到道路中心线
	void getRoadCentreLine(void);
	// zf,0623,
	void ObjectFilter(void);
	// zf，按面积
	void ObjectFilterByArea(void);
	// 记录该图层内所有环的点数的数组
	vector<vector<int>> circlePointList;
	// 记录该图层内所有环的面积的数组
	vector<vector<double>> circleAreaList;
	// 点数一维数组,zf,0701
	vector<int> zf_point;
	//面积一维数组,zf,0701
	vector<double> zf_area;
	// //二维数组转化成一维数组
	void getNewArray_Point(void);
	// 二维数组转化为一维数组，面积
	void getNewArray_Area(void);
	// 寻找一般的外接矩形，zf，0704
	void FindRectangle(void);
	// zf,0708,最小获取外接矩形
	void FindMABR(void);
	// 最小外接矩形优化，0712，zf
	void optimize(void);
	// 去除冗余点，zf，0717
	void RemoveRedundant(void);
	// 1003 zf
	void DeleteTail(void);
};

