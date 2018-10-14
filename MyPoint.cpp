#include "stdafx.h"
#include "MyPoint.h"


CMyPoint::CMyPoint()
{
	this->isCommon = false;
	this->isNULL = false;
	this->isJointPt = false;
	this->isUsed = false;
}

CMyPoint::CMyPoint(CPoint pt1)
{
	pt = pt1;
	this->isCommon = false;
	this->isNULL = false;
	this->isJointPt = false;
	this->isUsed = false;
}

CMyPoint::CMyPoint(CMyPoint & point)
{
	circleID = point.circleID;
	//PointGonTopuPos = point.PointGonTopuPos;
	polygonID = point.polygonID;
	ptID = point.ptID;
	pt = point.pt;
	PosInCommonPoint = point.PosInCommonPoint;
	isJointPt = point.isJointPt;
	isUsed = point.isUsed;
	isNULL = false;
}

//CMyPoint::CMyPoint(CMyPoint ** point)
//{
//	circleID = (*point)->circleID;
//	PointGonTopuPos = (*point)->PointGonTopuPos;
//	polygonID = (*point)->polygonID;
//	ptID = (*point)->ptID;
//	pt = (*point)->pt;
//	PosInCommonPoint = (*point)->PosInCommonPoint;
//}

CPoint CMyPoint::GetCPoint()
{
	return pt;
}

double CMyPoint::Getx()
{
	return pt.x;
}

double CMyPoint::Gety()
{
	return pt.y;
}


CMyPoint::~CMyPoint()
{
}

void CMyPoint::setPoint(CPoint pt1)
{
	this->pt = pt1;
}


// 判断一个点是否在rect内
bool CMyPoint::isInRect(CRect rect)
{
	if(pt.x<rect.right&&pt.x>rect.left&&pt.y<rect.top&&pt.y>rect.bottom){
		return true;
	}else
		return false;
	return false;
}


// CMyPoint转化为CPoint类，zf，0708
CPoint* CMyPoint::MyPoint2Point(CMyPoint* pt)
{
	//double xx = pt->Getx();
	//double yy = pt->Gety();
	CPoint* newPoint = new CPoint(pt->Getx(),pt->Gety());
	return newPoint;
}
