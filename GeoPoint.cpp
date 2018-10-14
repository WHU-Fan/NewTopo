#include "stdafx.h"
#include "GeoPoint.h"


CGeoPoint::CGeoPoint()
{
}

void CGeoPoint::Draw(CDC * pDC)
{
	pDC->SetPixel(pt, RGB(0, 0, 0));
}

CGeoPoint::CGeoPoint(CPoint pt1)
{
	pt = pt1;
}


CGeoPoint::~CGeoPoint()
{
}

void CGeoPoint::setPoint(CPoint pt1) {
	pt = pt1;
}

double CGeoPoint::getx() {
	return pt.x;
}

double CGeoPoint::gety() {
	return pt.y;
}
