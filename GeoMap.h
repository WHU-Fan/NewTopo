#pragma once
#include "GeoLayer.h"
#include"MyDCConfig.h"
#include<vector>
#include "atltypes.h"
using std::vector;
class CGeoMap
{
public:
	CGeoMap();
	~CGeoMap();
private:
	CGeoLayer* TopoLayer;//用于存放所有对象的layer
	CRect rectMWC;//逻辑范围
	unsigned int i;
	vector<CGeoLayer*> Layer;//数据库读入
public:	
	//void Draw(CDC *pDC, vector<CGeoLayer*> layer);
	void Draw(CDC *pDC,CMyDCConfig& myDCcfg);
	void MapTopo();//地图综合
	void TopoLayerAddObject();//将构建拓扑的所有对象加入到layer的objects数组中
	void AddLayer(CGeoLayer* layer1);///////这个地方问一下老师
	CRect GetMapRect();
	CGeoLayer* getTopoLayer();//得到私有变量layer，用于存放所有拓扑对象
	void setMapRect(CRect rect);
	CRect getMapRect();
	vector<CGeoLayer*> getLayer();//得到原始图层数组
	CGeoLayer* getLayerAt(int index);
	int getLayerSize();
	int mapScale;
	void simplify(double telorance);
	
	// 建立拓扑后输出数据时的范围
	CRect TopoRect;
	bool isTopoed;
	// 得到拓扑建立后数据范围，赋值给TopoRect
	void getTopoRect();
	// map中TopoLayer中每个面的面积
	void getTopoLayerObjectArea();
	// 得到在地图中pt点所在面的ID值
	int getSelectPolygonID(CPoint pt);
	// 在map层面进行局部拓扑，对Layer数组中最后一个元素进行判断，如果最后一个元素layer中还有面，就进行局部更新，同时删除数据，如果没有数据的话，进行提示：无可添加数据
	void mapPart_Topo(void);
	// 对一个layer进行局部拓扑，拓扑对向为TopoLayer，首先应该建立每个object的最小外接矩形，其次找到相交面，然后两个面进行建立裁剪，以及拓扑更新
	void layerPart_Topo(CGeoLayer* layer,CGeoLayer*& topoTayer);
	// 两个layer之间相互建立最小外接矩形的相交列表，如果index为0，则两个layer中每个面均需与两个layer中每一个面进行相交比对，如果index为1，则主需要layer1中每个面均需与两个layer中每一个面进行相交比对，意味着layer2中内部相交已经列表建成
	int GetTwoLayerIntersectMABR(CGeoLayer*& layer1, CGeoLayer*& layer2, int index);
	// 将layer2中的所有objects和objectstemp添加到layer1中
	int Layer1AddLayer2(CGeoLayer*& layer1, CGeoLayer* layer2);
	// 在TopoLayer中添加一个面，并进行拓扑更新
	void TopoLayerAddObject(CGeoPolygon* polygon);
	// 判断两个面是否相交
	int PosTwoPolygon(CGeoPolygon* polygon1, CGeoPolygon* polygon2);
	// 判断两个rect是否相交
	bool isIntersectTwoRect(CRect rect1, CRect rect2);
	// 两个面相交，如果两个面中没有经过拓扑的面，则return，如果有一个经过拓扑的面，则另一个与经过拓扑的面进行裁剪，如果两个面都经过拓扑，则重新考虑
	void twoPolygonIntersect(CGeoPolygon*& polygon1, CGeoPolygon*& polygon2);
	// zf,0623,filter
	void ObjectFilter(void);
	// zf,按面积
	void ObjectFilterByArea(void);
	// 寻找一般的外接矩形
	void FindRectangle(void);
	// zf,0708,获取最小外接矩形
	void FindMABR(void);
	// 建筑物优化 ，zf 0712
	void optimize(void);
	// 去除冗余点，zf，0717
	void RemoveRedundant(void);
	// 去除尾巴 1003 zf
	void DeleteTail(void);
};

