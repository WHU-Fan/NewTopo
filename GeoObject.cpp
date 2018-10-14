#include "stdafx.h"
#include "GeoObject.h"


CGeoObject::CGeoObject()
{
	isNULL = false;
	objectMABR=NULL;
}


CGeoObject::~CGeoObject()
{
	intersectMABRpos.clear();
	intersectMABRpos.swap(vector<long int>());
	intersectMABRposCopy.clear();
	intersectMABRposCopy.swap(vector<long int>());
}

void CGeoObject::BesidesObjectSearch()
{
}

void CGeoObject::TopoBuild()
{
}


void CGeoObject::ObjectMerge()
{
	
}

void CGeoObject::ObjectDelete()
{
	//GetCommomEdge();
}

void CGeoObject::ObjectSimplify()
{

}

void CGeoObject::ObjectShift()
{
	//GetCommomEdge();
}

BOOL CGeoObject::isObjectMABRintersect(CGeoObject* geoObject)
{
	int left = 0;
	int right = 0;
	int top = 0;
	int bottom = 0;
	left = this->objectMABR->left >= geoObject->objectMABR->left ? this->objectMABR->left : geoObject->objectMABR->left;
	right = this->objectMABR->right <= geoObject->objectMABR->right ? this->objectMABR->right : geoObject->objectMABR->right;
	top = this->objectMABR->top <= geoObject->objectMABR->top ? this->objectMABR->top : geoObject->objectMABR->top;
	bottom = this->objectMABR->bottom >= geoObject->objectMABR->bottom ? this->objectMABR->bottom : geoObject->objectMABR->bottom;
	if (left > right || top < bottom) {//不能有等于号
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

CRect * CGeoObject::GetObjectMABR()
{
	return nullptr;
}

int CGeoObject::GetCircleNum()
{
	return 0;
}

//void CGeoObject::getIntersectMABRposCopy()
//{
//	intersectMABRposCopy.assign(intersectMABRpos.begin(), intersectMABRpos.end());
//}




void CGeoObject::Draw(CDC * pDC, float width, int lineStyle, COLORREF lineColor, int fillstyle, COLORREF fillColor)
{
}

void CGeoObject::Draw(CDC * pdc, CMyDCConfig myDCcfg)
{
}


// // 得到一个顶点为pt1,pt2,pt3的三角形面的面积
double CGeoObject::getTriArea(CPoint pt1, CPoint pt2, CPoint pt3)
{
	
		double area = 0;
		int l1 = getTwoPointDis(pt1, pt2)/100.0;
		int l2 = getTwoPointDis(pt1, pt3)/100.0;
		int l3 = getTwoPointDis(pt3, pt2)/100.0;
		if (l1 == 0 || l2 == 0 || l3 == 0)
			return 0;
		int s = (l1 + l2 + l3) / 200.0;
		area = sqrt((long double)(s*(s - l1)*(s - l2)*(s - l3)))*10000;
		return area;
	
}


// 得到两点之间距离
double CGeoObject::getTwoPointDis(CPoint pt1, CPoint pt2)
{
	double distance = 0;
	double x = (pt1.x - pt2.x)/1000.0;
	double y = (pt1.y - pt2.y)/1000.0;
	distance = sqrt((long double)(x*x + y*y));
	return distance*1000;
}


// 计算a与b的整倍数的最小差的绝对值
double CGeoObject::getMod(double a, double b)
{
	double a1 = abs(a);
	double b1 = abs(b);
	int n = a1/b1;
	double c = a1-b1*n;
	c=abs(c);
	double c1 = b-c;
	if(c1<c)
		c = c1;
	return c;
}


// pt1表示线段起点，pt2表示线段终点，pt表示所求点,返回的是π制
double CGeoObject::getAngle(CPoint pt1, CPoint pt2, CPoint pt)
{
	int i=0;
	CPoint pt22;//pt到pt2的向量
	pt22.SetPoint((pt2.x-pt.x)/100.0,(pt2.y-pt.y)/100.0);
	CPoint pt11;//pt到pt1的向量
	pt11.SetPoint((pt1.x-pt.x)/100.0,(pt1.y-pt.y)/100.0);

	double chaji =pt22.x*pt11.y - pt22.y*pt11.x;
	if(chaji>0)
		i=1;
	else if(chaji<0)
		i=-1;
	else
		return 0;
	double a = ((pt2.x-pt.x)/100.0)*((pt2.x-pt.x)/100.0)+((pt2.y-pt.y)/100.0)*((pt2.y-pt.y)/100.0);//pt2-pt
	a=sqrt(a);
	double b = ((pt2.x-pt1.x)/100.0)*((pt2.x-pt1.x)/100.0)+((pt2.y-pt1.y)/100.0)*((pt2.y-pt1.y)/100.0);//pt1-pt2
	b=sqrt(b);
	double c = ((pt1.x-pt.x)/100.0)*((pt1.x-pt.x)/100.0)+((pt1.y-pt.y)/100.0)*((pt1.y-pt.y)/100);//pt1-pt
	c=sqrt(c);
	double d = (a*a+c*c-b*b)/(2*a*c);
	if(d>1){
		d=1;
		if(d-1>0.0001)
			AfxMessageBox("角度超限过大");
	}
	else if(d<-1)
		d=-1;
	double e = acos(d);
	if(_isnan(e)){
		AfxMessageBox("出现了NaN");
	}
	e =i* e;
	return e;

}


//  pt1和pt2分别是一条线段的起点和终点，判断pt在线段的左右侧，1返回左侧，-1在线上，0返回右侧
int CGeoObject::ptPostoLine(CPoint pt1, CPoint pt2, CPoint pt)
{
	int i=0;
	
	if (pt2.y == pt1.y)
	{
		if (pt.y>pt1.y)
		{ 
			return 1;
		}else if (pt.y==pt1.y)
		{
			return -1;
		}else {
			return 0;
		}
	}

	int x = (pt.y-pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y)+pt1.x;
	if(pt.x<x)
		i=1;
	else if(pt.x>x)
		i=0;
	else i = -1;
	return i;
}


// 点到直线的最短距离
double CGeoObject::MinsPointToLine(CPoint pt, CPoint ptOri, CPoint ptEnd)
{
	double areaTemp = getTriArea(pt,ptEnd,ptOri);
	double s = getTwoPointDis(ptOri,ptEnd);
	double d = 2*areaTemp/s;
	return d;
}


// 得到不平行的两条直线的交点
CPoint CGeoObject::getLineCrossPt(SegMent segMent1, SegMent segMent2)
{//要考虑直线竖直的问题
	CPoint CrossP;
	//y = a * x + b;
	if ((segMent1.ptOri.x - segMent1.ptEnd.x)<0.001)
	{
		if ((segMent2.ptOri.x - segMent2.ptEnd.x)<=0.001)
		{
			AfxMessageBox("两条直线均竖直");
			return (0,0);
		}else{
			int a = (segMent2.ptOri.y - segMent2.ptEnd.y) / (segMent2.ptOri.x - segMent2.ptEnd.x);
			CrossP.y = a*(segMent1.ptOri.x-segMent2.ptOri.x)+segMent2.ptOri.y;
			CrossP.x = segMent1.ptOri.x;
			return CrossP;
		}
	}else if ((segMent2.ptOri.x - segMent2.ptEnd.x)<=0.001)
	{
		int a = (segMent1.ptOri.y - segMent1.ptEnd.y) / (segMent1.ptOri.x - segMent1.ptEnd.x);
		CrossP.y = a*(segMent2.ptOri.x-segMent1.ptOri.x)+segMent1.ptOri.y;
		CrossP.x = segMent2.ptOri.x;
		return CrossP;
	}
	int a1 = (segMent1.ptOri.y - segMent1.ptEnd.y) / (segMent1.ptOri.x - segMent1.ptEnd.x);
	int b1 = segMent1.ptOri.y - a1 * (segMent1.ptOri.x);

	int a2 = (segMent2.ptOri.y - segMent2.ptEnd.y) / (segMent2.ptOri.x - segMent2.ptEnd.x);
	int b2 = segMent2.ptOri.y - a1 * (segMent2.ptOri.x);
	

	CrossP.x = (b1 - b2) / (a2 - a1);
	CrossP.y = a1 * CrossP.x + b1;
	return CrossP;
}


// 判断pt1和pt4是否在链pt2-pt3的同侧，若同侧返回1，异侧返回-1，有一点或者两点均在链上，返回0
int CGeoObject::getStyleFourptsUnit(CPoint pt1, CPoint pt2, CPoint pt3, CPoint pt4)
{
	int style1 = ptPostoLine(pt2,pt3,pt1);
	int style4 = ptPostoLine(pt2,pt3,pt4);
	if (style4==-1||style1==-1)
	{
		AfxMessageBox("竟然还有冗余点");
		return 0;
	}
	if ((style1+style4)==1)
	{
		return -1;
	}
	if ((style1+style4)==0||(style1+style4)==2)
	{
		return 1;
	}
}


// zf,0623
void CGeoObject::ObjectFilter(void)
{
}


// zf，0701
void CGeoObject::ObjectFilterByArea(void)
{
}


// 返回点数数组，zf，0701
vector<int> CGeoObject::getPointNum(void)
{
	return vector<int>();
}


// 返回面积数组，zf，0701
vector<double> CGeoObject::getCircleArea(void)
{
	return vector<double>();
}


// 寻找一般的外接矩形，zf，0704
void CGeoObject::FindRectangle(void)
{
}


// zf,0708,最小获取外接矩形
void CGeoObject::FindMABR(void)
{
}


// 最小外接矩形优化，0712，zf
bool CGeoObject::optimize(void)
{
	return false;
}


// 去除冗余点，zf，0717
void CGeoObject::RemoveRedundant(void)
{
}


// zf 1003
void CGeoObject::DeleteTail(void)
{
}
