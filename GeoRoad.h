#pragma once
#include "geopolygon.h"
class CGeoRoad :
	public CGeoPolygon
{
public:
	vector<CGeoPolyline*> roadCentreLine;
	CGeoRoad(void);
	~CGeoRoad(void);

	// 生成道路中心线
	void generateRoadCentreLine(void);
	// 得到道路中心线
	vector<CGeoPolyline*> getRoadCentreLine(void);
	// 得到化简后的道路面
	CGeoRoad* refineRoad(void);
	// 得到由道路中心线向两边扩展width后的道路面
	CGeoRoad* refineRoad(double width);
	// 得到由某条polyline向两边扩展width后的道路面
	CGeoRoad* refineRoad(double width,CGeoPolyline* line);
};

