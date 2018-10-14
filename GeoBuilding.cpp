#include "stdafx.h"
#include "GeoBuilding.h"


CGeoBuilding::CGeoBuilding()
{
}


CGeoBuilding::~CGeoBuilding()
{
}


// 移除多边形的冗余点
void CGeoBuilding::buildPolygonSimple(double angle, double area, double length)
{
	int circleNum1 = circleNum;//有问题，当合并面之后，circle要重新组织
	for (int i = 0;i<circleNum1;i++)
	{
		vector<CGeoPolyline*> simpleCircle = commoncircles[i];//应该为空，合并面的时候，要更新circleNum，拓扑建立后要rebuildCircle，合并要rebuild，rebuild前要判断
		//rebuild前要判断数组是否是空，如果是空就填入，不空就清除 
		int lineSize = simpleCircle.size();
		for (int j = 0;j<lineSize;j++)
		{
			CGeoPolyline* lineTemp = simpleCircle[j];
			lineTemp->buildLineSimple(angle,area,length);
		}
	}
}





// 得到建筑物多边形的最小外接矩形
CGeoPolygon CGeoBuilding::getMinMABR(void)
{
	return CGeoPolygon();
}
