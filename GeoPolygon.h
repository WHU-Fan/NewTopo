#pragma once
#include "GeoObject.h"
#include"GeoPoint.h"
#include"GeoPolyline.h"
#include"MyDCConfig.h"
#include"MyPoint.h"
#include"MABR_ZF.h"
#include<vector>
using namespace std;

struct squarePoint {
	CPoint pt;
	int oldNum;
	int newNum;
	bool operator < (const squarePoint &m)const {
		return pt.y < m.pt.y;
	}
};

class CGeoPolygon :
	public CGeoObject
{
public:
	CGeoPolygon();
	CGeoPolygon(CGeoPolygon &polygon);
	~CGeoPolygon();
	
	void Draw(CDC *pDC);//要素绘制
	void Draw(CDC *pDC, CMyDCConfig myDCcfg);//通过公共链数组进行绘制
	int GetCircleNum();//得到子环数目

    //CGeoPolyline* GetCircleAt(int index);//得到面中的第index子环
	void AddCircle(CGeoPolyline*);//添加子环
	CRect* GetObjectMABR();
	void deleteLine(int lineID);//目前没用到
	void rebuildCircle();//从commonEdge中整理链成数个环，并添加到commonCircle中
	vector<CGeoPolyline*> rebuildCircle(vector<CGeoPolyline*> lines, int i);//从lines[i]形成一个环，返回这个环
	void mergeCommonEdge(CGeoPolyline* line1,CGeoPolyline* line2,int style);//合并两条链，line1和line2，后期要修改line的circleID
	//line1的起点=line2的起点返回1，line1的终点=line2的终点返回2，line1的终点=line2的起点返回3，line1的起点=line2的终点返回4，无法相连返回0
	void ReoganizeCommonCircle();//得到完整的commonCircles,同时circlePoints、circlePointNum开辟存储空间、TopoCircleNum，赋值

	void eraseNullLine();//重建commonCircle时删除isNULL=true的链
	void adjustCommonCircle();//调整公共环内部链的顺序，无用函数
	void adjustCommonCircle(vector<CGeoPolyline*> lines);//无用函数
	// 拓扑面积
	double TopoArea; 
	double shapeArea;//最开始读入的面积
	int circleNum;//环数
	int TopoPointNum;//点的总数
	double TopoLength;//所有环的长度
	bool isTopoed;

	vector<CGeoPolyline*>circles;//子环
	vector<CGeoPolyline*>commonEdge;//试验用公共边数组
	vector<vector<CGeoPolyline*>> commoncircles;//用于存放公共边的环，用于计算面积，以及输出文件
	vector<vector<CMyPoint*>> circlePoints;   //每个环的点，zf
	vector<int> circlePointNum;//每个子环上点的数目
	vector<vector<int>> nodePos;//每个子环上交点的位置

	void getArea();//得到拓扑后的面积
	double getArea(int index);//对链数组lines围成的环计算面积

	 // 得到一个顶点为pt1,pt2,pt3的三角形面的面积
	//double getTriangleArea(CPoint pt1, CPoint pt2, CPoint pt3);
	// 得到两点之间的距离
	//double getTwoPointDistance(CPoint pt1, CPoint pt2);
	// 删除数组circle里的内容
	void deleteCircle();
	// 在数组lines中寻找与i相连的下一条链，返回数组下标
	CPoint findNextLine(vector<CGeoPolyline*> lines, int i);
	// 对一个commonCircles的点顺序放到一个数组里，同时进行数据检查
	vector<CMyPoint*> getCirclePoint(vector<CGeoPolyline*> lines);
	// 对环内的数组进行排序，返回该点纵坐标在数组中排序的位置.pt.x代表了该点的纵坐标的排序位置，pt.y代表
	//了排序后第i个大小的纵坐标值
	vector<CPoint> circlePointReorder(vector<CPoint>& pts);
	//梯形剖分
	void tixingSplit(vector<CPoint> cpts, int i, vector<CPoint> a, vector<vector<SegMent>>& tixingLine);
	// 线段line上，求纵坐标为y的点对应的横坐标
	double getSegementLeft(SegMent line, double y,int i);
	// 计算一个条带内的梯形斜边组成的面积
	double getArea(vector<SegMent> segments,int i);
	// 返回与m点纵坐标一致的点的最后一个点的下标
	int findSamePoint(vector<squarePoint> aPoint, int m);
	// 对数组中从i到j的纵坐标进行修改，计算i-1坐标和j纵坐标之差，然后平分到每个点，要倒着平分
	void modifySquarePoint(vector<squarePoint>& aPoint, int i, int j);
	// 得到拓扑建立后的对象范围
	void getObjectTopoRect();
	// 计算某个环的周长
	double getCircleLength(int i);
	// 对CPoint数组围成的环求面积
	double getArea(vector<CMyPoint*> pts);
	// 对一个环上点，求x的最小坐标以及y值得最小坐标，都减100
	double getMinY(vector<CMyPoint*> pts);
	// 得到拓扑建立后的点数量（circlePoints中获得）
	void getTopoPointNum();
	// 每个环的周长
	vector<double> circleLength;
	// 得到每个环的长度
	void getCircleLength();
	// 每个环的面积
	vector<double> area;
	// 通过周长面积比来删除数据误差产生的内环
	void delErrorCircle();
	// 从commonCircle中获取点，按照一定顺序加入到circlePoints中
	void getCirclePoint();
	// 判断一个点是否在这个面内
	bool isPtIn(CPoint pt);
	// 计算a与b的整倍数的最小差的绝对值
	//double getMod(double a, double b);
	// 判断一个点是否在这个面内
	bool isPtInNew(CPoint pt);
	// 判断点pt与起点为pt1终点为pt2的线段的关系
	int getLinkofPtToSeg(CPoint pt1, CPoint pt2, CPoint pt);
	// pt1和pt2分别是一条线段的起点和终点，判断pt在线段的左右,侧1返回左侧，-1在线上，0返回右侧
	//int getPtLRtoLine(CPoint pt1, CPoint pt2, CPoint pt);
	// 判断一条线段与面的关系：0线段在面外；1外相接；2相交；3内相接；4在内部
	int isSegmentInPolygon(SegMent line);
	// 判断点是否在面的边界上
	bool isPtOnEdge(CPoint pt);
	// 判断点是否在链上
	bool isPtOnSegment(CPoint pt, SegMent line1);
	// 得到line2落在line1上的端点，如果只有一个端点，返回该端点，如果没有端点返回（0,0），如果有两个端点，返回（1,1）则将line2的两个端点同时添加
	CPoint getJointPt(SegMent line1, SegMent line2);
	//aa, bb为一条线段两端点 cc, dd为另一条线段的两端点 相交返回true, 不相交返回false  
	bool intersect(SegMent line1,SegMent line2);  
	double determinant(double v1, double v2, double v3, double v4);
	// 对点数组oriPts进行去重排序，x小的在前，x相同y小在前
	vector<CPoint> pointsSort(vector<CPoint> oriPts);
	// 数组pts中是一条线段与面相交的顶点的排序，需判断数组中相邻点的中点是否在面内面外，如果均在外，返回-1，均在内，返回1，内外均有，返回0
	int getSegmentPos(vector<CPoint> pts);
	
	// 判断两条链是否可以合并，如果两条链存在同一端点，且左右面一致，则可以合并
	bool judgeLineMerge(CGeoPolyline* line1,CGeoPolyline* line2);
	// 判断两条链融合的类型，line1的起点=line2的起点返回1，line1的终点=line2的终点返回2，line1的终点=line2的起点返回3，line1的起点=line2的终点返回4，无法相连返回0
	int styleLineMerge(CGeoPolyline* line1, CGeoPolyline* line2);

	// zf,0623，面的过滤
	void ObjectFilter(void);
	// zf,按面积过滤
	void ObjectFilterByArea(void);
	//zf，0701,环的点数组成的数组

	vector<int> circlePointList;
	//zf，0701,环的面积组成的数组
	vector<double> circleAreaList;
	// 获取多边形所有环的点数

	vector<int> getPointNum(void);
	// 获取多边形所有环的面积
	vector<double> getCircleArea(void);
	// 寻找一般的外接矩形，zf，0704
	void FindRectangle(void);

	// 存储一般外接矩形的一维数组，zf，0704
	vector<CPoint*> rectangleArray;

	// 寻找某一个circle的外接矩形，zf，0704
	void FindRectangle(vector<CMyPoint*> pts);

	// 存储旋转后外接矩形的数组，zf，0706
	vector<CPoint*> rectangleRotate;

	// 某一点pt绕center旋转theta弧度，zf,0706
	CPoint* rotate(CPoint* pt, CPoint* center, double theta);
	// 旋转所有矩形，zf，0706
	void rotateAll(void);
	// zf,0708，寻找最小外接矩形
	void FindMABR(void);
	// 寻找环的中心（重心），zf，0708
	CPoint* FindCenter(vector<CPoint*> ptsArray);
	// 某多边形的一般外接矩形，zf，0708
	vector<CPoint*> FindRectangle(vector<CPoint*> pts);
	// 计算 建筑物占最小外接矩形的比例，建筑物/最小外接矩形 ，zf，0712
	bool areaProportion(void);
	// 最小外接矩形优化，0712，zf
	bool optimize(void);
	// 是否找到外接矩形，zf，0714
	bool hasFindMABR;
	// 是否找到一般外接矩形（绑定外接矩形）
	bool hasFindRectangle;
	// 去除冗余点，zf，0717
	void RemoveRedundant(void);
	// 计算三点之间的角度，zf，0717
	double calAngle(CMyPoint* p1, CMyPoint* p2, CMyPoint* p3);
	// 判断某个数是不是在数组中，zf，0717
	bool isContained(vector<int> temp, int i);
	// MABR拟合建筑物多边形，zf，0719
	void fitting(CGeoPolyline* line);
	// 拟合后成果，zf,0720
	vector<CPoint*> buildings;
	// 20180720,zf
	bool hasGetBuildings;
	// 0904,zf,保留一半的点
	void GetHalfPoints(void);
	
	// 去除尾巴 1003 zf
	void DeleteTail(void);
	// 计算两点的距离  1003 zf
	double calDis(CMyPoint* pt1, CMyPoint* pt2);
	// 面积阈值，zf，1003
	int areaT;
	// 化简次数  zf  1003
	int times;
};
