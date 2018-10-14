#pragma once
#include "GeoPolygon.h"
//#include "geos.h"
class CGeoBuilding :
	public CGeoPolygon
{
public:
	CGeoBuilding();
	~CGeoBuilding();
	
	// 移除多边形的冗余点
	void buildPolygonSimple(double angle, double area, double length);
	// 得到建筑物多边形的最小外接矩形
	CGeoPolygon getMinMABR(void);
};

