#pragma once
#include<vector>
using namespace std;
struct PointTopo//用于存储一个点所对应的的面、环、链中的位置信息
{
	int x;//点所在面的下标
	int y;//点所在的环在面中的下标
	int z;//点所在环中的下标
	void setTopo(int x1, int y1, int z1) {
		x = x1;
		y = y1;
		z = z1;
	}
};
class CMyPoint
{
public:
	CMyPoint();
	CMyPoint(CPoint pt1);
	CMyPoint(CMyPoint &point);
	//CMyPoint(CMyPoint** point);
	CPoint GetCPoint();
	double Getx();
	double Gety();
	BOOL isCommon;//是否加入到公共点数组
	unsigned long int PosInCommonPoint;
	~CMyPoint();
	//vector<PointTopo> PointGonTopuPos;//一个点所属的面在Map数组中的位置，例如点pt1同属于polygon1与polygon2，两面在数组中
	                                  //位置分别是35、1086,则将该点所属面情况通过CPoint形式记录，端点会有多个CPoint，所以采用数组
	bool isNULL;
	void setPoint(CPoint pt1);
	int polygonID;//所属面在layer图层中的下标
	int circleID;//所属环在circles数组中的下标
	int ptID;//点在circle中pts数组中的下标
	bool isJointPt;//是否是端点
	bool isUsed;

	CPoint pt;
public:
	// 判断一个点是否在rect内
	bool isInRect(CRect rect);
	// CMyPoint转化为CPoint类，zf，0708
	CPoint* MyPoint2Point(CMyPoint* pt);
};

