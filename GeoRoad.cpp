#include "StdAfx.h"
#include "GeoRoad.h"


CGeoRoad::CGeoRoad(void)
{
}


CGeoRoad::~CGeoRoad(void)
{
}

// 生成道路中心线
void CGeoRoad::generateRoadCentreLine(void)
{
	
}

// 得到道路中心线
vector<CGeoPolyline*> CGeoRoad::getRoadCentreLine(void)
{
	return roadCentreLine;
}



// 得到化简后的道路面(是不是要考虑中心线向外扩展多宽的问题）
CGeoRoad* CGeoRoad::refineRoad(void)
{
	return NULL;
}


// 得到由道路中心线向两边扩展width后的道路面
CGeoRoad* CGeoRoad::refineRoad(double width)
{
	return NULL;
}
// 得到由某条polyline向两边扩展width后的道路面
CGeoRoad* CGeoRoad::refineRoad(double width,CGeoPolyline* line)
{
	return NULL;
}
