#pragma once
#include "GeoObject.h"
class CGeoPoint :
	public CGeoObject
{//无用
public:
	CGeoPoint();
	void Draw(CDC *pDC);
	CGeoPoint(CPoint pt1);
	~CGeoPoint();

	void setPoint(CPoint pt1);
	double getx();
	double gety();
	int pointStyle;//0代表不属于公共边，1代表属于公共边但不属于端点，2代表公共边的端点
private:
	CPoint pt;
};

