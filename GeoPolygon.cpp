#include "stdafx.h"
#include "GeoPolygon.h"
#include"GeoLayer.h"
#include "MyPoint.h"

#include <algorithm>
#define _USE_MATH_DEFINES   //　看math.h中的定义  
#include <math.h>

using namespace std;


CGeoPolygon::CGeoPolygon()
{
	times = 4;  //化简次数
	areaT = 100000;  //面积阈值
	isNULL = false;
	TopoArea = 0;
	isTopoed = false;
	circleNum = 0;
	shapeArea = 0;
	hasFindMABR = false;
	hasFindRectangle = false;
	hasGetBuildings = false;
}

CGeoPolygon::CGeoPolygon(CGeoPolygon & polygon)
{
	int size = polygon.circles.size();
	for (int i = 0; i < size; i++) {
		CGeoPolyline* polyline = new CGeoPolyline(*(polygon.circles[i]));
		circles.push_back(polyline);		
	}
}

CGeoPolygon::~CGeoPolygon()
{
	
	for (vector<CGeoPolyline*>::iterator itb = circles.begin();
		itb != circles.end(); ++itb) {
		CGeoPolyline* circle = *itb;
		if (circle) {
			delete circle;
			circle = NULL;
		}
	}
	circles.clear();

	for (vector<CGeoPolyline*>::iterator itc = commonEdge.begin();
		itc != commonEdge.end(); ++itc) {
		CGeoPolyline* commonedge = *itc;
		if (commonedge) {
			delete commonedge;
			commonedge = NULL;
		}
	}
	commonEdge.clear();

	int size = commoncircles.size();
	for (int i = 0; i < size; i++) {
		vector<CGeoPolyline*> circle = commoncircles[i];
		for (vector<CGeoPolyline*>::iterator itd = circle.begin();
			itd != circle.end(); ++itd) {
			CGeoPolyline* commonedge = *itd;
			if (commonedge) {
				delete commonedge;
				commonedge = NULL;
			}
		}
		circle.clear();
	}
	vector<vector<CGeoPolyline*>>().swap(commoncircles);

	vector<double>().swap(area);
	vector<double>().swap(circleLength);
	vector<int>().swap(circlePointNum);
	
}


void CGeoPolygon::Draw(CDC * pDC)
{
	int sizeofcircle = circles.size();
	if (sizeofcircle == 0) {
		int sizeofCommonEdge = commonEdge.size();
		for (int i = 0; i < sizeofCommonEdge; i++) {
			commonEdge[i]->Draw(pDC);
		}
	}
	CPoint pt;
	for (int i = 0; i < sizeofcircle; i++)//分子环绘制
	{
		CGeoPolyline* polyline=this->circles[i];
		int sizeofline = polyline->GetSize();
		
		for (int j = 0; j < sizeofline; j++) //子环绘制
		{
			circles[i]->Draw(pDC);
			//CMyPoint* point =polyline->GetPointAt(j);
			//pt =point->GetCPoint();
			//if (j == 0) {
			//	pDC->MoveTo(pt);
			//	//pDC->SetPixel(pt, RGB(255, 0, 0));
			//}
			//else
			//{
			//	pDC->LineTo(pt);
			//	pDC->MoveTo(pt);
			//	pDC->SetPixel(pt, RGB(255, 0, 0));
			//}
		}
		//delete polyline;
		//polyline = NULL;
	}
}

void CGeoPolygon::Draw(CDC * pDC, CMyDCConfig myDCcfg)
{
	
	if (myDCcfg.style == 2) {
		int num = commonEdge.size();
		if (myDCcfg.fillstyle==0)
		{
			for (int i = 0; i < num; i++) {
				if (!commonEdge[i]->isNULL) {
					commonEdge[i]->Draw(pDC, myDCcfg);
				}		    
			}
		}else if (myDCcfg.fillstyle==1)
		{
			//fillColor
			CBrush brush(RGB(247,238,214));
			CBrush *pOldBrush = pDC->SelectObject(&brush);  //笔刷
			pDC->BeginPath();
			CArray<CPoint,CPoint> pts;  //定义点数组
			commonEdge[0]->MyPointsToCPoints(circlePoints[0], pts);  //把外环的点存到pts中
			CPoint *pta = pts.GetData();   
			pDC->Polygon(pta,pts.GetSize());   //创建矩形
			pDC->EndPath();
			pDC->FillPath();
			pDC->SelectObject(pOldBrush);
			for (int i = 0; i < num; i++) {  //公共环数量
				if (!commonEdge[i]->isNULL) {
					commonEdge[i]->Draw(pDC, myDCcfg);
				}		    
			}
		}

	}
	else if (myDCcfg.style == 3) {
		for (int i = 0; i < circles.size(); i++) {
			//if(i ==0) myDCcfg.lineColor = RGB(255, 0,  0);
			circles[i]->Draw(pDC, myDCcfg);
			//if(circles[i]->areaProportion == 0)    //未找外接矩形之前
			//{
			//	circles[i]->Draw(pDC, myDCcfg);
			//}
			//if(circles[i]->areaProportion >= 0)   //找到外接矩形，算出面积比例之后
			//{
			//	circles[i]->Draw(pDC, myDCcfg);
			//	/*CString str;
			//	str.Format("%lf",circles[i]->areaProportion);
			//	pDC->TextOutA(circles[i]->center.x,circles[i]->center.y,str);*/
			//}
		}
		//zf,0705,画绑定外接矩形
		if(rectangleArray.size()!= 0&&hasFindRectangle == true)
		{
			int xunhuan = rectangleArray.size()/4;
			int count = 0;
			for(int n = 0;n<xunhuan;n++)  //一个多边形可能有多个环，0706
			{
				pDC->MoveTo(*rectangleArray[0+n*4]);
				pDC->LineTo(*rectangleArray[1+n*4]);
				pDC->LineTo(*rectangleArray[2+n*4]);
				pDC->LineTo(*rectangleArray[3+n*4]);
				pDC->LineTo(*rectangleArray[0+n*4]);
			}
		}
		//zf,0706,画最小外接矩形
		if(rectangleRotate.size()!= 0&&hasFindMABR==true)  //优化之前
		{
			int xunhuan = rectangleRotate.size()/4;
			int count = 0;
			for(int m = 0;m<xunhuan;m++)  //一个多边形可能有多个环，0706
			{
				pDC->MoveTo(*rectangleRotate[0+m*4]);
				pDC->LineTo(*rectangleRotate[1+m*4]);
				pDC->LineTo(*rectangleRotate[2+m*4]);
				pDC->LineTo(*rectangleRotate[3+m*4]);
				pDC->LineTo(*rectangleRotate[0+m*4]);
				/*CString str1;
				str1.Format("%d",count);
				pDC->TextOutA(rectangleRotate[0+m*4]->x+100,rectangleRotate[0+m*4]->y+100,str1);
				count++;
				CString str2;
				str2.Format("%d",count);
				pDC->TextOutA(rectangleRotate[1+m*4]->x+100,rectangleRotate[1+m*4]->y+100,str2);
				count++;
				CString str3;
				str3.Format("%d",count);
				pDC->TextOutA(rectangleRotate[2+m*4]->x+100,rectangleRotate[2+m*4]->y+100,str3);
				count++;
				CString str4;
				str4.Format("%d",count);
				pDC->TextOutA(rectangleRotate[3+m*4]->x+100,rectangleRotate[3+m*4]->y+100,str4);
				count++;*/
			}
		}
		//zf,0720,画拟合建筑物
		if(buildings.size()!= 0&&hasGetBuildings == true)
		{
			int count = 0;
			pDC->MoveTo(*buildings[0]);
			for(int m = 1;m<buildings.size();m++){
				pDC->LineTo(*buildings[m]);
				/*CString str;
				str.Format("%d",count);
				pDC->TextOutA(buildings[m]->x+100,buildings[m]->y+100,str);
				count++;*/

			}
		}
	}
	else if (myDCcfg.style == 5) {
		if (this->circlePoints.size() == 0) {
			AfxMessageBox("应该是没有建立拓扑，保存为空");
		}
		else {
			CPen pen(PS_SOLID, 1, myDCcfg.lineColor);
			CPen* pOldPen = pDC->SelectObject(&pen);
			int size = this->circlePoints.size();
			for (int i = 0; i < size; i++) {
				vector<CMyPoint*> pts = circlePoints[i];
				int sizeofPts = pts.size();
				for (int i = 0; i < sizeofPts; i++) {
					if (i == 0)
						pDC->MoveTo(pts[i]->GetCPoint());
					else
					{
						pDC->LineTo(pts[i]->GetCPoint());
						pDC->MoveTo(pts[i]->GetCPoint());
						//pDC->SetPixel(pt,myDCcfg.lineColor);
					}
				}
			}
			pDC->SelectObject(pOldPen);
			pen.DeleteObject();
		}
	}
}

int CGeoPolygon::GetCircleNum()
{
	return circles.size();
}


//CGeoPolyline* CGeoPolygon::GetCircleAt(int index)
//{
//	CGeoPolyline* circle = circles[index];
//	return circle;
//}

void CGeoPolygon::AddCircle(CGeoPolyline* line)
{
	circles.push_back(line);
}

CRect* CGeoPolygon::GetObjectMABR()
{
	CRect* rect;
	rect = new CRect();
	long left, right, top, bottom;
	for (int i = 0; i < circles.size(); i++) {
		int size = circles[i]->pts.size();
		for (int j = 0; j < circles[i]->pts.size(); j++) {
			CMyPoint* point = circles[i]->pts[j];
			if (i == 0&&j==0) {
				left = point->Getx();
				right = point->Getx();
				top = point->Gety();
				bottom = point->Gety();
			}
			else {
				if (left > point->Getx()) {
					left = point->Getx();
				}
				if (right < point->Getx()) {
					right = point->Getx();
				}
				if (top < point->Gety()) {
					top = point->Gety();
				}
				if (bottom > point->Gety()) {
					bottom = point->Gety();
				}
			}
		}

	}
	rect->left = left;
	rect->bottom = bottom;
	rect->right = right;
	rect->top = top;
	
	this->objectMABR = rect;
	return objectMABR;
}

void CGeoPolygon::deleteLine(int lineID)
{
}

void CGeoPolygon::rebuildCircle()//circleID还未赋值
{
	int size = commonEdge.size();
	for (int i = 0; i < size; i++) {
		commonEdge[i]->isInCommonCircle = 0;
	}
	if (commoncircles.size() != 0) {
		commoncircles.resize(0);
	}
	if (size == 1) {
		vector<CGeoPolyline*> line;
		CMyPoint* oriPt = commonEdge[0]->pts[0];
		CMyPoint* endPt = commonEdge[0]->pts[commonEdge[0]->pts.size()-1];
		if (oriPt->GetCPoint() == endPt->GetCPoint()) {
			line.push_back(commonEdge[0]);
			commonEdge[0]->isInCommonCircle = 1;
			commoncircles.push_back(line);
		}
	}
	else {
		for (int i = 0; i < size; i++) {
			if (commonEdge[i]->isInCommonCircle == 0) {
				vector<CGeoPolyline*> pline = rebuildCircle(commonEdge, i);
				commoncircles.push_back(pline);
			}		
		}
	}
	size = commoncircles.size();
	area.resize(size);
	circleLength.resize(size);
	circlePoints.resize(size);
	circlePointNum.resize(size);
	this->circleNum = size;
	this->getCirclePoint();
	this->getArea();
	this->getTopoPointNum();
	this->getCircleLength();
}

vector<CGeoPolyline*> CGeoPolygon::rebuildCircle(vector<CGeoPolyline*> lines, int i)//要注意修改line的circle信息  
{
	vector<CGeoPolyline*> circle;
	int temp = i;

	do {
		circle.push_back(lines[temp]);
		CPoint pt = findNextLine(lines, temp);
		temp = pt.x;
		int joinStyle = pt.y;
		lines[temp]->isInCommonCircle = joinStyle;		
	} while (temp != i);
	return circle;
}

// 在数组lines中寻找与i相连的下一条链，返回数组下标,以及上条链的连接方式
CPoint CGeoPolygon::findNextLine(vector<CGeoPolyline*> lines, int i)
{
	CPoint pt(-1, -1);
	int size = lines.size();
	CGeoPolyline* pline = lines[i];
	CMyPoint* joinPoint;
	int count = 0;
	int temp = -1;
	int flag = pline->isInCommonCircle;
	if (flag == 0) {
		if (lines[i]->rightAttribute == this->objectAttribute) {
			flag = 1;
			
		}
		else {
			flag = 2;
		}
		pline->isInCommonCircle = flag;
	}
	
	if ((*((lines[i]->pts).end() - 1))->GetCPoint() == lines[i]->pts[0]->GetCPoint()) {
		pt.SetPoint(i, 1);
		return pt;
	}
	if (flag == 1) {
		joinPoint = *((lines[i]->pts).end() - 1);
	}
	else {
		joinPoint = lines[i]->pts[0];
	}
	CPoint joinCPoint = joinPoint->GetCPoint();
	for (int j = 0; j < size; j++) {
		CGeoPolyline* pline1 = lines[j];
		if (!pline1->isNULL) {
			CMyPoint* endPoint = *((pline1->pts).end() - 1);
			CMyPoint* oriPoint = pline1->pts[0];
			if (endPoint->GetCPoint() == joinCPoint) {
				count++;
				if (j != i) {
					temp = j;
					pt.y = 2;
					pt.x = temp;
				}
			}
			if (oriPoint->GetCPoint() == joinCPoint) {
				count++;
				if (j != i) {
					temp = j;
					pt.y = 1;
					pt.x = temp;
				}
			}
		}
		
	}
	if (this->PosinCommonLayer == 250) {
		int sssss = 1;
	}
	int s = count;
	
	if (s != 2) {
		AfxMessageBox("polygon公共链成环出错:未找到两个点一致");
		if (this->PosinCommonLayer != 227) {
			int ssss = 1;
		}
		
	}
	if (s == 2 && pt.y == -1) {
		pt.y = 1;
	}
	if (s == 2 && temp == -1) {
		pt.x = i;
	}
	return pt;
}


void CGeoPolygon::mergeCommonEdge(CGeoPolyline * line1,CGeoPolyline* line2,int style)//合并两条链，line1和line2，后期要修改line的circleID
	//line1的起点=line2的起点返回1，line1的终点=line2的终点返回2，line1的终点=line2的起点返回3，line1的起点=line2的终点返回4，无法相连返回0
{
	if ((line1->isNULL&&!line2->isNULL)||(line2->isNULL&&!line1->isNULL))
		return;
	if (line1->isNULL&&!line2->isNULL){
		AfxMessageBox("两条线均为空，无法合并");
	}

	CMyPoint* line1OriPt = line1->pts[0];
	CMyPoint* line1EndPt = line1->pts[line1->pts.size()-1];
	CMyPoint* line2OriPt = line2->pts[0];
	CMyPoint* line2EndPt = line2->pts[line2->pts.size()-1];
	if (line1->PosinCommonLayer==line2->PosinCommonLayer)
	{
		AfxMessageBox("两条线重叠");
	}
	if (style==1)
	{
		line1OriPt->isJointPt = false;
		for(int i = 1;i<line2->pts.size();i++){
			line1->pts.insert(line1->pts.begin(),line2->pts[i]);
		}
		line2->isNULL = true;
	} 
	else if (style==2)
	{
		line1EndPt->isJointPt = false;
		for(int i = line2->pts.size()-2;i>=0;i--){
			line1->pts.push_back(line2->pts[i]);
		}
		line2->isNULL = true;
	} else if (style==3)
	{
		line1EndPt->isJointPt = false;
		for(int i = 1;i<line2->pts.size();i++){
			line1->pts.push_back(line2->pts[i]);
		}
		line2->isNULL = true;
	} else if (style==4)
	{
		line2EndPt->isJointPt = false;
		for(int i = 1;i<line1->pts.size();i++){
			line2->pts.push_back(line1->pts[i]);
		}
		line1->isNULL = true;
	} else if(style==0){
		AfxMessageBox("两条链无法合并");
		return;
	}
}

void CGeoPolygon::ReoganizeCommonCircle()
{
	
	this->eraseNullLine();//出现了一个面内部只有两条链，两条链左右面一致的情况
	this->rebuildCircle();
	//this->adjustCommonCircle();
}

void CGeoPolygon::eraseNullLine()
{
	for (int i = 0; i < commonEdge.size();) {
		if (commonEdge[i]->isNULL)
			commonEdge.erase(commonEdge.begin() + i);
		else
			i++;
	}
}

void CGeoPolygon::adjustCommonCircle()
{
	int size = commoncircles.size();
	for (int i = 0; i < size; i++) {
		this->adjustCommonCircle(commoncircles[i]);
	}	
}

void CGeoPolygon::adjustCommonCircle(vector<CGeoPolyline*> lines)
{
	int size = lines.size();
	int temp = -1;
	for (int i = 0; i < size; i++) {
		CMyPoint* EndPoint = *((lines[i]->pts).end() - 1);
		CPoint ptEnd = EndPoint->GetCPoint();
		int count = 0;
		for (int j = 0; j < size; j++) {
			CMyPoint* OriPoint = lines[j]->pts[0];
			CPoint ptOri = OriPoint->GetCPoint();
			if (ptEnd == ptOri) {
				count++;
				temp = j;
			}
		}//最后一条链的的尾点一定和首链的起点一致
		for (int j = 0; j < size&&j!=i; j++) {
			CMyPoint* OriPoint = *((lines[j]->pts).end() - 1);
			CPoint ptOri1 = OriPoint->GetCPoint();
			if (ptEnd == ptOri1) {
				count++;
				temp = j;
			}
		}//最后一条链的的尾点一定和首链的起点一致
		if ((temp != 0 && i == size - 1)||count!=1) {
			CString str;
			str.Format("环没有闭合");
			AfxMessageBox(str);
		}
		if (temp != i + 1) {
			if (i == size - 1 && temp == 0)
				break;
			else {
				swap(lines[temp], lines[i + 1]);
			}	
		}
	}
	
	for (int i = 0; i < lines.size(); i++) {
		int j = i + 1;
		if (j >= lines.size()) {
			j = 0;
		}
		if (lines[i]->leftAttribute == lines[j]->leftAttribute&&lines[i]->rightAttribute == lines[j]->rightAttribute) {
			int ptSize = lines[j]->pts.size();
			for (int m = 1; m < ptSize; m++) {
				lines[i]->pts.push_back(lines[j]->pts[m]);
			}
			lines.erase(lines.begin() + j);
		}
	}
}

//得到拓扑后的面积
void CGeoPolygon::getArea()
{
	int size = commoncircles.size();
	double areaTemp;
	
	for (int i = 0; i < size; i++) {
		areaTemp =getArea(i);//这个circle是不是要判断isNULL
		area[i]=areaTemp;
	}
	if (area.size() > 0) {
		double tempArea ;
		int ll = 0;
		for (int i = 0; i < size; i++) {
			if (i == 0) {
				tempArea = area[0];
			}
			else {
				if (tempArea < area[i]) {
					tempArea = area[i];
					ll = i;
				}
			}
		}
		if (ll != 0) {
			swap(area[0], area[ll]);
			swap(commoncircles[0], commoncircles[ll]);//此处需要验证是否交换成功
		}
	}
	
	double finalArea = area[0];
	for (int i = 1; i < area.size(); i++) {
		finalArea = finalArea - area[i];
	}
	if (finalArea <= 0) {
		AfxMessageBox("某个面的面积竟然小于0");
	}
	this->TopoArea = finalArea;
}

double CGeoPolygon::getArea(int index)
{
	double area = 0;
	vector<CMyPoint*> cpts = circlePoints[index];
	//同时填充circlePoint数组，shp文件中，每个面的最后一个点与第一个点相同
	//circlePoints[index] = cpts;
	area = getArea(cpts);
	
	return area;//这只是一个环面积
}
//double CGeoPolygon::getArea(int index)
//{
//	vector<CGeoPolyline*> lines = commoncircles[index];
//	vector<CPoint> cpts = this->getCirclePoint(lines);
//	//同时填充circlePoint数组，shp文件中，每个面的最后一个点与第一个点相同
//	circlePoints[index] = cpts;
//	vector<CPoint> a;
//	a = this->circlePointReorder(cpts);//现在知道了一条链上的两个端点分别在端点排序中的位置，例如一条链两端分别是19,26，那么还需要知道19到26之间的点的纵坐标
//	//y坐标值逐渐增大，在此处修改了纵坐标一致的情况，但还是存在
//
//	int sizeOfCpts = cpts.size();
//	vector<vector<Segment>> tixingLine;
//	tixingLine.resize(sizeOfCpts - 1);
//	for (int i = 0; i < sizeOfCpts; i++) {
//		this->tixingSplit(cpts, i, a, tixingLine);//存在相邻两点纵坐标一致
//	}
//	double s = 0;
//	double area = 0;
//	int sizeOfTixing = tixingLine.size();
//	for (int i = 0; i < sizeOfTixing; i++) {
//		s = getArea(tixingLine[i],i);//出现了面积小于0的情况
//		area = s + area;
//	}
//	CPoint pt1 = circlePoints[index][0];
//	circlePoints[index].push_back(pt1);
//	int ptNum = circlePoints[index].size();
//	circlePointNum[index] = ptNum;
//	return area;//这只是一个环面积
//}

// 删除数组circle里的内容
void CGeoPolygon::deleteCircle()
{
	int size = circles.size();
	for (int i = 0; i < size; i++) {
	circles[i]->pts.clear();
	circles[i]->pts.swap(vector<CMyPoint*>());
	}
	circles.clear();
	circles.swap(vector<CGeoPolyline*>());
}

// 对一个commonCircles的点顺序放到一个数组里，同时进行数据检查
vector<CMyPoint*> CGeoPolygon::getCirclePoint(vector<CGeoPolyline*> lines)
{
	int size = lines.size();
	for (int i = 0; i < size; i++) {
		lines[i]->isInCommonCircle = 0;
	}
	if (size == 1) {
		CMyPoint* pt1Ori = lines[0]->pts[0];
		CMyPoint* pt1End = *(lines[0]->pts.end() - 1);
		lines[0]->isInCommonCircle = 1;
		if (pt1End->GetCPoint() != pt1Ori->GetCPoint()) {
			AfxMessageBox("commonCircles中未闭合");
		}
	}
	else {
		for (int i = 0; i < size; i++) {
			if (i == 0) {
				if (lines[i]->rightAttribute == this->objectAttribute) {
					lines[i]->isInCommonCircle = 1;
				}
				else {
					lines[i]->isInCommonCircle = 2;
				}				
			}
				
			if (lines[i]->isNULL) {
				AfxMessageBox("commonCircles除去空链后竟然还有空链");
			}
			int j = i + 1;
			if (j == size)
				j = 0;
			CMyPoint* pt1;
			if (lines[i]->isInCommonCircle == 1) {
				pt1 = *(lines[i]->pts.end() - 1);
			}
			else {
				pt1 = lines[i]->pts[0];
			}

			CMyPoint* pt2Ori = lines[j]->pts[0];
			CMyPoint* pt2End = *(lines[j]->pts.end() - 1);
			if (pt1->GetCPoint() == pt2Ori->GetCPoint()) {
				lines[j]->isInCommonCircle = 1;
			}
			else if (pt1->GetCPoint() == pt2End->GetCPoint()) {
				lines[j]->isInCommonCircle = 2;
			}
			else {
				AfxMessageBox("commonCircles除去空链后竟然还有上一条链的尾点不等于下一条链的首点");//出现了第一条链的首点与下一条链相连
			}
			if (i == size - 1) {
				if (lines[0]->isInCommonCircle == 1) {
					if (pt1->GetCPoint() != lines[0]->pts[0]->GetCPoint()) {
						AfxMessageBox("commonCircles中未闭合");
					}
				}
				else {
					if (pt1->GetCPoint() != (*(lines[0]->pts.end()-1))->GetCPoint()) {
						AfxMessageBox("commonCircles中未闭合");
					}
				}
				
			}
		}
	}

	//开始
	vector<CMyPoint*> cpts;
	for (int i = 0; i < size; i++) {
		CGeoPolyline* line = lines[i];
		int sizePt = line->pts.size();
		if (line->isInCommonCircle == 1) {//line->rightAttribute == this->objectAttribute
			for (int j = 0; j < sizePt - 1; j++) {
				CMyPoint* myPoint = line->pts[j];
				if (!myPoint->isNULL) {
					
					cpts.push_back(myPoint);
				}
			}
		}
		else if (line->isInCommonCircle == 2) {
			for (int j = sizePt - 1; j >0; j--) {
				CMyPoint* myPoint = line->pts[j];
				if (!myPoint->isNULL) {
					
					cpts.push_back(myPoint);
				}
			}
		}
		else {
			AfxMessageBox("lines中有标记为0的存在");
		}

	}//pts中的点应该是不重复的点
	cpts.push_back(cpts[0]);
	//检测数据是否有重复点
	//for (int i = 1; i < cpts.size() - 1; i++) {
	//	if(cpts[i]== cpts[i+1])
	//		//AfxMessageBox("环数组中有点重复");
	//}
	return cpts;
}

// 对环内的数组进行排序，返回该点纵坐标在数组中排序的位置
vector<CPoint> CGeoPolygon::circlePointReorder(vector<CPoint>& pts)
{
	int size = pts.size();
	vector<squarePoint> aPoint;
	aPoint.resize(size);
	for (int i = 0; i < size; i++) {
		aPoint[i].pt = pts[i];
		aPoint[i].oldNum = i;
		aPoint[i].newNum = 0;
	}
	sort(aPoint.begin(), aPoint.end());
	//vector<int> repeatNum;
	int sizeAPoint = aPoint.size();
	
	//检查数据有没有水平线段
	for (int i = 0; i < sizeAPoint-1; i++) {
		int j = i + 1;
		if (aPoint[i].pt.y == aPoint[j].pt.y) {
			int m = this->findSamePoint(aPoint, i);
			this->modifySquarePoint(aPoint, i, m);
		}	
	}
	int ddd = 1;
	/*if (repeatNum.size() > 0) {
		for (int i = 0; i < repeatNum.size(); i++) {
			int d = repeatNum[i];
			pts[d].y= pts[d].y-1;
			aPoint[d].pt.y = aPoint[d].pt.y - 1;
		}	
	}*/
	for (int i = 0; i < size; i++) {
		aPoint[i].newNum = i;
	}
	for (int i = 0; i < size; i++) {
		pts[i] = aPoint[i].pt;
	}
	vector<CPoint> a;
	a.resize(size);
	int temp = -1;
	for (int i = 0; i < size; i++) {
		temp = aPoint[i].oldNum;
		a[temp].x = aPoint[i].newNum;
		a[i].y=aPoint[i].pt.y;
	}
	return a;
}
//cpts是环顺序点数组，i是执行剖分的第i个点，与下一个点组成线段，a的x表示第i个点纵坐标大小位置，假设与下一点纵坐标
//分别为16,29，那么调用a的第17到28个y直接进行分割，得到的每一段线加到梯形line的数组中
void CGeoPolygon::tixingSplit(vector<CPoint> cpts, int i, vector<CPoint> a, vector<vector<SegMent>>& tixingLine)
{
	
	int size = cpts.size();
	int j = i + 1;
	if (j > size - 1)
		j = 0;
	CPoint pt1 = cpts[i];
	CPoint pt2 = cpts[j];
	int pos1 = a[i].x;//第i个点纵坐标所在的位置，例如16
	int pos2 = a[j].x;//第j个点纵坐标所在的位置，例如20
	int temp;
	if (pos1 > pos2) {
		temp = pos1;
		pos1 = pos2;
		pos2 = temp;
	}
	vector<double> high;//放的是17到19的纵坐标值
	for (int m = pos1; m <= pos2; m++) {
		double  h = a[m].y;
		high.push_back(h);
	}
	int highSize = high.size();
	SegMent segment(cpts[i], cpts[j]);
	//segment.setSegment(cpts[i], cpts[j]);
	vector<double> horizonte;//横坐标
	for (int m = 0; m < highSize; m++) {
	
		double x = this->getSegementLeft(segment, high[m],i);
		horizonte.push_back(x);
	}
	for (int m = 0; m < highSize-1; m++) {
	
		CPoint pt1(horizonte[m], high[m]);
		CPoint pt2(horizonte[m+1], high[m+1]);
		SegMent asegment(pt1, pt2);
		//asegment.setSegment(pt1, pt2);
		tixingLine[pos1].push_back(asegment);
		
		pos1++;

	}
	int llll = 1;
}

// 线段line上，求纵坐标为y的点对应的横坐标
double CGeoPolygon::getSegementLeft(SegMent line, double y,int i)
{
	double x = 0;
	if (line.ptEnd.y == line.ptOri.y) {
		int ssss = 1;//纵坐标一致
	}
	
	double lgaocha = line.ptEnd.y - line.ptOri.y;
	double sgaocha = y - line.ptOri.y;
	double part = sgaocha / lgaocha;
	double xcha = line.ptEnd.x - line.ptOri.x;
	x = xcha*part + line.ptOri.x;
	return x;
}

// 计算一个条带内的梯形斜边组成的面积
double CGeoPolygon::getArea(vector<SegMent> segments,int m)
{
	int size = segments.size();
	
	double s = 0;
	double area = 0;
	sort(segments.begin(), segments.end());
	for (int i = 0; i < size;i=i+2) {
		CPoint ptOri1 = segments[i].ptOri;
		CPoint ptOri2 = segments[i + 1].ptOri;
		CPoint ptEnd1 = segments[i].ptEnd;
		CPoint ptEnd2 = segments[i + 1].ptEnd;
		double xTop = fabs((double)(ptOri1.x - ptOri2.x));
		double xBottom = fabs((double)(ptEnd1.x - ptEnd2.x));
		double h = fabs((double)(ptOri1.y - ptEnd1.y));
		s = (xTop + xBottom)*h / 2;
		if (s < 0) {
			int llll = 1;
		}
		area = area + s;
	}
	return area;
}


// 返回与m点纵坐标一致的点的最后一个点的下标
int CGeoPolygon::findSamePoint(vector<squarePoint> aPoint, int m)
{
	double y = aPoint[m].pt.y;
	int size = aPoint.size();
	while (aPoint[m].pt.y == y) {
		m++;
		if (m == size)
			break;
	}
	m = m - 1;
	return m;
}


// 对数组中从i到j的纵坐标进行修改，计算i-1坐标和j纵坐标之差，然后平分到每个点，要倒着平分
void CGeoPolygon::modifySquarePoint(vector<squarePoint>& aPoint, int i, int j)
{
	int size = aPoint.size();
	int m = i - 1;
	double y;
	if (i ==0) {
		y = 10;
	}
	else {
		y = aPoint[i].pt.y - aPoint[m].pt.y;
	}
	int num;
	if (i < j)
		num = j - i + 1;
	else
		num = size - i + j + 1;
	double increase = y / num;
	for (int x = i; x != j; x++) {
		if (x == size) {
			x = 0;
		}
		aPoint[x].pt.y = aPoint[x].pt.y - increase*(num - 1);
		num--;
	}
}


// 得到一个顶点为pt1,pt2,pt3的三角形面的面积
//double CGeoPolygon::getTriangleArea(CPoint pt1, CPoint pt2, CPoint pt3)
//{
//	double area = 0;
//	int l1 = getTwoPointDistance(pt1, pt2);
//	int l2 = getTwoPointDistance(pt1, pt3);
//	int l3 = getTwoPointDistance(pt3, pt2);
//	if (l1 == 0 || l2 == 0 || l3 == 0)
//		return 0;
//	int s = (l1 + l2 + l3) / 2;
//	area = sqrt((long double)(s*(s - l1)*(s - l2)*(s - l3)));
//	return area;
//}


// 得到两点之间的距离
//double CGeoPolygon::getTwoPointDistance(CPoint pt1, CPoint pt2)
//{
//	int distance = 0;
//	int x = pt1.x - pt2.x;
//	int y = pt1.y - pt2.y;
//	distance = sqrt((long double)(x*x + y*y));
//	return distance;
//}



// 得到拓扑建立后的对象范围
void CGeoPolygon::getObjectTopoRect()
{
	CRect newRcet;
	double nLeft, nRight, nTop, nBottom;
	CGeoPolyline* line0 = commoncircles[0][0];
	CMyPoint* myPoint0 = line0->pts[0];
	nLeft = myPoint0->Getx();
	nRight = myPoint0->Getx();
	nTop = myPoint0->Gety();
	nBottom = myPoint0->Gety();

	if (!isNULL) {
		int sizeLine = this->commonEdge.size();
		for (int j = 0; j < sizeLine; j++) {
			CGeoPolyline* line = this->commonEdge[j];
			if (!line->isNULL) {
				int sizePoint = line->pts.size();
				for (int z = 0; z < sizePoint; z++) {
					CMyPoint* myPoint = line->pts[z];
					if (!myPoint->isNULL) {
						if (nLeft > myPoint->Getx())
							nLeft = myPoint->Getx();
						if (nRight <myPoint->Getx())
							nRight = myPoint->Getx();
						if (nTop > myPoint->Gety())
							nTop = myPoint->Gety();
						if (nBottom < myPoint->Gety())
							nBottom = myPoint->Gety();
					}
				}
			}
		}
	}
	newRcet.left = nLeft;
	newRcet.right = nRight;
	newRcet.top = nTop;
	newRcet.bottom = nBottom;
	this->objectTopoRect = newRcet;
}


// 计算某个环的周长
double CGeoPolygon::getCircleLength(int i)
{
	double d = 0;
	vector<CGeoPolyline*> plines = commoncircles[i];
	int sizeofPlines = plines.size();
	for (int j = 0; j < sizeofPlines; j++) {
		if (!plines[j]->isNULL) {
			double l = plines[j]->getLineLength();
			d = d + l;
		}
	}
	return d;
}


// 对CPoint数组围成的环求面积
double CGeoPolygon::getArea(vector<CMyPoint*> pts)
{
	double minY = this->getMinY(pts);
	double tempArea = 0;
	double area = 0;
	int size = pts.size();
	for (int i = 0; i < size; i++) {
		int j = i + 1;
		if (i == size - 1) j = 0;
		double h = pts[j]->Getx() - pts[i]->Getx();
		double y1 = pts[i]->Gety() - minY;
		double y2 = pts[j]->Gety() - minY;
		tempArea = (y1 + y2)*h/2;
		area = area + tempArea;
	}
	if (area < 0)
		area = -area;
	return area;
}


// 对一个环上点，求x的最小坐标以及y值得最小坐标，都减100
double CGeoPolygon::getMinY(vector<CMyPoint*> pts)
{
	double y = pts[0]->Gety();
	int size = pts.size();
	for (int i = 0; i < size; i++) {
		
		if (y > pts[i]->Gety()) {
			y = pts[i]->Gety();
		}
	}
	return y;
}


// 得到拓扑建立后的点数量（circlePoints中获得）
void CGeoPolygon::getTopoPointNum()
{
	int temp = 0;
	if (circlePoints.size() < 1)
		AfxMessageBox("请先计算面积");
	for (int i = 0; i < circlePoints.size(); i++) {
		int ptNum = circlePoints[i].size();
		circlePointNum[i] = ptNum;
		temp = temp + circlePointNum[i];
	}
	TopoPointNum = temp;
}


// 得到每个环的长度
void CGeoPolygon::getCircleLength()
{
	int size = commoncircles.size();
	if (size == 0)
		AfxMessageBox("commonCircle大小竟然为0");
	double circlethresholdTemp;

	for (int i = 0; i < size; i++) {//isNULL均为false
		circlethresholdTemp = this->getCircleLength(i);
		circleLength[i]=circlethresholdTemp;
	}
	TopoLength = 0;
	for (int i = 0; i < size; i++) {//isNULL均为false
		TopoLength = TopoLength + circleLength[i];
	}
}


// 通过周长面积比来删除数据误差产生的内环
void CGeoPolygon::delErrorCircle()
{
	int size = area.size();
	vector<double> bili;
	for (int i = 0; i < size; i++)
	{
		double part = area[i] / circleLength[i];//通过面积周长比来删除数据误差产生的内环
		bili.push_back(part);
	}
	for (int i = 0; i < commoncircles.size(); ) {
		CGeoPolyline* line = commoncircles[i][0];
		if (!line->isNULL) {
			if (line->leftPolygonID == -1 && i != 0) {//如果这个面是一个内部有空隙的环
													//通过面积/周长进行筛选

				if (bili[i] < 5000) {
					int size1 = commoncircles[i].size();
					for (int j = 0; j < size1; j++) {
						commoncircles[i][j]->isNULL = true;
					}
					bili.erase(bili.begin() + 1);
					commoncircles.erase(commoncircles.begin() + i);
					TopoArea = TopoArea + area[i];
					area.erase(area.begin() + i);
					circleLength.erase(circleLength.begin() + i);
					circlePoints.erase(circlePoints.begin() + i);
					TopoPointNum = TopoPointNum - circlePointNum[i];
					circlePointNum.erase(circlePointNum.begin() + i);
					circleNum--;
				}
				else
					i++;
			}
			else {
				i++;
			}
		}
		else {//该环为空
			i++;
		}
	}
	bili.clear();
}


// 从commonCircle中获取点，按照一定顺序加入到circlePoints中
void CGeoPolygon::getCirclePoint()
{
	int size = commoncircles.size();
	circlePoints.resize(size);
	for(int i = 0; i < size; i++) {
		vector<CGeoPolyline*> lines = commoncircles[i];
		vector<CMyPoint*> cpts = this->getCirclePoint(lines);
		
		circlePoints[i] = cpts;
	}
	
}


// 判断一个点是否在这个面内
bool CGeoPolygon::isPtIn(CPoint pt)
{
	int threshold =commoncircles.size();
	double angleSum = 0.00;
	double angleTemp1 = 0.00;
	double angleTemp2 = 0.00;
	if(threshold>0){//说明经过rebuildcircle，则可以从CirclePoint中进行读取
		for(int i = 0;i<threshold;i++){
			vector<CGeoPolyline*> lines = commoncircles[i];
			int size1 = lines.size();
			angleTemp2 = 0.0;
			for(int j = 0;j<size1;j++){
				CGeoPolyline* pline = lines[j];
				angleTemp2+=pline->getAnglePtToLine(pt);
			}
			double cha1 = abs(1000*(getMod(angleTemp2,2*M_PI)));
			if(cha1>1){
				AfxMessageBox("一个commoncircle竟然不是360度");
			}
			angleTemp1+=angleTemp2;
		}
		double cha2 =abs(abs(angleTemp1)-2*M_PI)*1000;
		if(cha2>1)
			return false;
		else 
			return true;
	}else{//只能从Circle数组中读取
		int size2 = circles.size();
		
		for(int i = 0;i<size2;i++){
			angleTemp1 = 0;
			CGeoPolyline* line = circles[i];
			angleTemp1 = line->getAnglePtToLine(pt);
			double cha1 =getMod(angleTemp1,2*M_PI);
			cha1 = abs(1000*(cha1));
			if(cha1>1){
				AfxMessageBox("一个circle竟然不是360度");
			}
			angleSum+=angleTemp1;
		}
		double cha2 = abs(abs(angleSum)-2*M_PI)*1000;
		if(cha2>1)
			return false;
		else 
			return true;
	}
}


// 计算a与b的整倍数的最小差的绝对值
//double CGeoPolygon::getMod(double a, double b)
//{
//	double a1 = abs(a);
//	double b1 = abs(b);
//	int n = a1/b1;
//	double c = a1-b1*n;
//	c=abs(c);
//	double c1 = b-c;
//	if(c1<c)
//		c = c1;
//	return c;
//}


// 判断一个点是否在这个面内
bool CGeoPolygon::isPtInNew(CPoint pt)
{
	int threshold =commoncircles.size();
	if(threshold>0){//说明经过rebuildcircle，则可以从CirclePoint中进行读取
		int sum1 = 0;
		int temp = 0;
		for(int i = 0;i<threshold;i++){
			vector<CMyPoint*> pts = circlePoints[i];
			int ptNum = pts.size();
			for(int j = 0;j<ptNum-1;j++){
				CPoint pt1 = pts[j]->GetCPoint();
				CPoint pt2 = pts[j+1]->GetCPoint();
				temp+=getLinkofPtToSeg(pt1,pt2,pt);
			}
			sum1+=temp;
		}	
		if(sum1==0)
			return false;
		else if(sum1==2||sum1==-2)
			return true;
		else
			AfxMessageBox("计算出错");
	}else{//只能从Circle数组中读取
		int size2 = circles.size();
		int sum1 = 0;
		int temp = 0;
		for(int i = 0;i<size2;i++){			
			CGeoPolyline* line = circles[i];
			for(int j = 0;j<line->pts.size()-1;j++){
				CPoint pt1 = line->pts[j]->GetCPoint();
				CPoint pt2 = line->pts[j+1]->GetCPoint();
				temp = getLinkofPtToSeg(pt1,pt2,pt);
				sum1+=temp;
			}
		}
		if(sum1==0)
			return false;
		else if(sum1==2||sum1==-2)
			return true;
		else 
			AfxMessageBox("计算出错");
	}
	
}


// 判断点pt与起点为pt1终点为pt2的线段的关系——源自判定检测点是否在多边形内的新方法，无法判断点在面的边界上

int CGeoPolygon::getLinkofPtToSeg(CPoint pt1, CPoint pt2, CPoint pt)
{
	int i=0;
	CPoint pt12;//pt1到pt2的向量
	pt12.SetPoint((pt2.x-pt1.x)/100.0,(pt2.y-pt1.y)/100.0);
	double y = pt.y;
	double minY = min(pt1.y,pt2.y);
	double maxY = max(pt1.y,pt2.y);
	
	if (pt2.y>pt1.y)
	{
		i = 1;
	} 
	else if(pt2.y<pt1.y)
	{
		i = -1;
	}else
		i = 0;

	if(y<minY||y>maxY)
		return 0; 
	else if (y>minY&&y<maxY)
	{
		int j = ptPostoLine(pt1,pt2,pt);
		return 2*i*j;
	}else{
		int j = ptPostoLine(pt1,pt2,pt);
		return i*j;
	}
}


// pt1和pt2分别是一条线段的起点和终点，判断pt在线段的左右侧，1返回左侧，-1在线上，0返回右侧
//int CGeoPolygon::getPtLRtoLine(CPoint pt1, CPoint pt2, CPoint pt)
//{
//	int i=0;
//	//CPoint pt12;//pt1到pt2的向量
//	//pt12.SetPoint((pt2.x-pt1.x)/100.0,(pt2.y-pt1.y)/100.0);
//	//CPoint pt10;//p1到pt0的向量
//	//pt10.SetPoint((pt1.x-pt.x)/100.0,(pt1.y-pt.y)/100.0);
//
//	//double chaji =pt12.x*pt10.y - pt12.y*pt10.x;
//	int x = (pt.y-pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y)+pt1.x;
//	if(pt.x<x)
//		i=1;
//	else if(pt.x>x)
//		i=0;
//	else i = -1;
//	return i;
//}




// 判断一条线段与面的关系：0线段在面外；1外相接；2相交；3内相接；4在内部;5重叠
int CGeoPolygon::isSegmentInPolygon(SegMent line1)
{
	CPoint pt1Ori = line1.ptOri;
	CPoint pt1End = line1.ptEnd;
	CPoint pt(0,0);
	struct PointAtt{
		int pos;//-1点在面外；0点在面边界；1点在面内
		CPoint pt;
		PointAtt(int p,CPoint pt2){
			pos = p;
			pt = pt2;
		}
	};   
	PointAtt ptOri(0,pt1Ori),ptEnd(0,pt1End);
	if(isPtOnEdge(pt1Ori)){
		ptOri.pos = 0;
	}else if(isPtInNew(pt1Ori)){
		ptOri.pos = 1;
	}else{
		ptOri.pos = -1;
	}

	if(isPtOnEdge(pt1End)){
		ptEnd.pos = 0;
	}else if(isPtInNew(pt1End)){
		ptEnd.pos = 1;
	}else{
		ptEnd.pos = -1;
	}

	if (ptOri.pos*ptEnd.pos==-1)
	{
		return 2;
	}//判断线段首尾两点与面的关系
	//下面判断线段与面的交点，如果有跨越式交点，则返回2，如果没有相切点，则返回0,4，如果有多个相切点，判断是否重叠，根据中点判断线段在面内面外
	vector<CPoint> joinPoints;
	int threshold =commoncircles.size();
	if(threshold>0){//说明经过rebuildcircle，则可以从CirclePoint中进行读取
		int sum1 = 0;
		int temp = 0;
		for(int i = 0;i<threshold;i++){
			vector<CMyPoint*> pts = circlePoints[i];
			int ptNum = pts.size();
			for (int j=0;j<ptNum-1;j++)
			{
				SegMent line2(pts[j]->GetCPoint(),pts[j+1]->GetCPoint());
				if (intersect(line1,line2))
				{
					return 2;
				} 
				else
				{
					CPoint tempPt1 = getJointPt(line2,line1);
					if (tempPt1==CPoint(1,1))//说明待求线段完全都在面的边界重叠
					{ 
						return 5;
					}else if (tempPt1!=CPoint(0,0))
					{
						joinPoints.push_back(tempPt1);
					} 

					CPoint tempPt2 = getJointPt(line1,line2);
					if (tempPt2==CPoint(1,1))//说明面的某个边界完全在待求线段内部重叠
					{ 
						joinPoints.push_back(line2.ptOri);
						joinPoints.push_back(line2.ptEnd);
					}else if (tempPt2!=CPoint(0,0))
					{
						joinPoints.push_back(tempPt2);
					} 
				}
			}
			if(joinPoints.size()==0){//如果待求线段与面没有交点，只需判断线段某个端点在面内还是面外，从而返回0,4
				CPoint apt = line1.ptOri;
				if (isPtInNew(apt))
				{
					return 4;
				} 
				else
				{
					return 0;
				}
			}else{//待求线段与面有交点，将交点按照xy排序，求中点在面内还是面外，参考自计算方法
				vector<CPoint> paiXuPoints;
				paiXuPoints = pointsSort(joinPoints);
			}
		}	
		
	}else{//只能从Circle数组中读取
		int size2 = circles.size();
		int sum1 = 0;
		int temp = 0;
		for(int i = 0;i<size2-1;i++){			
			CGeoPolyline* line = circles[i];
			
		}
		
	}

	return 0;
}


// 判断点是否在面的边界上
bool CGeoPolygon::isPtOnEdge(CPoint pt)
{
	CMyPoint myPt(pt);
	if (!myPt.isInRect(objectMABR))
	{
		return false;
	}else{
		int threshold =commoncircles.size();
		if(threshold>0){//说明经过rebuildcircle，则可以从CirclePoint中进行读取
			int sum1 = 0;
			int temp = 0;
			for(int i = 0;i<threshold;i++){
				vector<CMyPoint*> pts = circlePoints[i];
				int ptNum = pts.size();
				for(int j = 0;j<ptNum-1;j++){
					CPoint pt1 = pts[j]->GetCPoint();
					CPoint pt2 = pts[j+1]->GetCPoint();
					SegMent line(pt1,pt2);
					if (isPtOnSegment(pt,line))
					{
						return true;
					}
				}
			}	
			return false;
		}else{//只能从Circle数组中读取
			int size2 = circles.size();
			int sum1 = 0;
			int temp = 0;
			for(int i = 0;i<size2;i++){			
				CGeoPolyline* line = circles[i];
				for(int j = 0;j<line->pts.size()-1;j++){
					CPoint pt1 = line->pts[j]->GetCPoint();
					CPoint pt2 = line->pts[j+1]->GetCPoint();
					SegMent line(pt1,pt2);
					if (isPtOnSegment(pt,line))
					{
						return true;
					}
				}
			}
			return false;
		}
	}	
}


// 判断点是否在线段上
bool CGeoPolygon::isPtOnSegment(CPoint pt, SegMent line1)
{
	if (pt==line1.ptEnd||pt==line1.ptOri)
	{
		return true;
	}
	CPoint ptOri = line1.ptOri;
	CPoint ptEnd = line1.ptEnd;
	CRect rect(0,0,0,0);
	rect.bottom = ptEnd.y>ptOri.y?ptEnd.y:ptOri.y;
	rect.top = ptEnd.y<ptOri.y?ptEnd.y:ptOri.y;
	rect.left = ptEnd.x<ptOri.x?ptEnd.x:ptOri.x;
	rect.right = ptEnd.x>ptOri.x?ptEnd.x:ptOri.x;
	CMyPoint myPt(pt);
	if (myPt.isInRect(rect))
	{
		double x1 = ptEnd.x-ptOri.x;
		double y1 = ptEnd.y - ptOri.y;
		double x2 = pt.x-ptOri.x;
		double y2 = pt.y - ptOri.y;
		if (y1/x1==y2/x2)
		{
			return true;
		} 
		else
		{
			return false;
		}
	}else{
		return false;
	}
	
}

// 得到line2落在line1上的端点，如果只有一个端点，返回该端点，如果没有端点返回（0,0），如果有两个端点，返回（1,1）则将line2的两个端点同时添加
CPoint CGeoPolygon::getJointPt(SegMent line1, SegMent line2)
{
	CPoint pt(0,0);
	CPoint pt2Ori = line2.ptOri;
	CPoint pt2End = line2.ptEnd;
	int count = 0;
	if (isPtOnSegment(pt2Ori,line1))
	{
		pt=pt2Ori;
		count++;
	}
	if (isPtOnSegment(pt2End,line1))
	{
		pt =pt2End;
		count++;
	}
	if (count==0||count==1)
	{
		return pt;
	}else{
		return CPoint(1,1);
	}
}
//判断线段line1和线段line2是否相交 相交返回true, 不相交返回false  
bool CGeoPolygon::intersect(SegMent line1,SegMent line2)  
{  
	CPoint aa=line1.ptOri;
	CPoint bb=line1.ptEnd;
	CPoint cc=line2.ptOri;
	CPoint dd=line2.ptEnd;
	double delta = determinant(bb.x-aa.x, cc.x-dd.x, bb.y-aa.y, cc.y-dd.y);  
	if ( delta<=(1e-6) && delta>=-(1e-6) )  // delta=0，表示两线段重合或平行  
	{  
		return false;  
	}  
	double namenda = determinant(cc.x-aa.x, cc.x-dd.x, cc.y-aa.y, cc.y-dd.y) / delta;  
	if ( namenda>1 || namenda<0 )  
	{  
		return false;  
	}  
	double miu = determinant(bb.x-aa.x, cc.x-aa.x, bb.y-aa.y, cc.y-aa.y) / delta;  
	if ( miu>1 || miu<0 )  
	{  
		return false;  
	}  
	return true;  
}  
///------------alg 3------------ 
///------------alg 3------------ 源自//blog.csdn.net/rickliuxiao/article/details/6259322
double CGeoPolygon::determinant(double v1, double v2, double v3, double v4)  // 行列式  
{  
	return (v1*v3-v2*v4);  
}  

// 对点数组oriPts进行去重排序，x小的在前，x相同y小在前
vector<CPoint> CGeoPolygon::pointsSort(vector<CPoint> oriPts)
{
	vector<CPoint> paiXuPoints;
	if (oriPts.size()==0)
	{
		return vector<CPoint>();
	}
	paiXuPoints.push_back(oriPts[0]);
	int oriPtNum = oriPts.size();
	for (int i = 1;i<oriPts.size();i++)
	{
		CPoint tempPt = oriPts[i];
		int paiXuptSize = paiXuPoints.size();
		if (tempPt.x>paiXuPoints[paiXuptSize-1].x||(tempPt.x==paiXuPoints[paiXuptSize-1].x&&tempPt.y>paiXuPoints[paiXuptSize-1].y))
		{
			paiXuPoints.push_back(tempPt);
			continue;
		}
		for (int j = 0;j<paiXuptSize;j++)//插入
		{
			CPoint paiXuPt = paiXuPoints[j];
			if (tempPt.x>paiXuPt.x)
			{
				continue;
			} 
			else if (tempPt.x<paiXuPt.x)
			{
				paiXuPoints.insert(paiXuPoints.begin()+j,tempPt);
			} 
			else
			{
				if (tempPt.y>paiXuPt.y)
				{
					continue;
				} 
				else if (tempPt.y==paiXuPt.y)
				{
					break;
					
				} 
				else
				{
					paiXuPoints.insert(paiXuPoints.begin()+j,tempPt);
				}
			}
			
		}
	}
	return paiXuPoints;
}


// 数组pts中是一条线段与面相交的顶点的排序，需判断数组中相邻点的中点是否在面内面外，如果均在外，返回-1，均在内，返回1，内外均有，返回0，中点全在边界上，返回2
int CGeoPolygon::getSegmentPos(vector<CPoint> pts)
{
	int inNum = 0;
	int outNum = 0;
	int onNum = 0;
	int size = pts.size();
	for (int i = 0;i<size -1;i++)
	{
		double x =(pts[i].x+pts[i+1].x)/2;
		double y =(pts[i].y+pts[i+1].y)/2;
		CPoint pt;
		pt.SetPoint(x,y);
		if (isPtOnEdge(pt))
		{
			onNum++;
		}else if (isPtInNew(pt))
		{
			inNum++;
		} 
		else
		{
			outNum++;
		}
	}
	if(outNum==0&&inNum==0&&onNum!=0){
		return 2;
	}else if (outNum==0&&inNum!=0)
	{
		return 1;
	}else if (outNum!=0&&inNum==0)
	{
		return -1;
	}else if (outNum!=0&&inNum!=0)
	{
		return 0;
	}
	return 0;
}




// 判断两条链是否可以合并，如果两条链存在同一端点，且左右面一致，则可以合并
bool CGeoPolygon::judgeLineMerge(CGeoPolyline* line1,CGeoPolyline* line2)
{
	bool isJointPt = false;
	if (line1->pts[0]->GetCPoint()==line2->pts[line2->pts.size()-1]->GetCPoint()||line1->pts[0]->GetCPoint()==line2->pts[0]->GetCPoint()
		||line1->pts[line1->pts.size()-1]->GetCPoint()==line2->pts[line2->pts.size()-1]->GetCPoint()||line1->pts[line1->pts.size()-1]->GetCPoint()==line2->pts[0]->GetCPoint())
	{
		isJointPt = true;
	}
	bool isAttSame = false;
	if ((line1->leftAttribute==line2->leftAttribute&&line1->rightAttribute==line2->rightAttribute)
		||(line1->leftAttribute==line2->rightAttribute&&line1->rightAttribute==line2->leftAttribute))
	{
		isAttSame = true;
	}
	if (isJointPt&&isAttSame)
	{
		return true;
	} 
	else
	{
		return false;
	}
	
}


// 判断两条链融合的类型，line1的起点=line2的起点返回1，line1的终点=line2的终点返回2，line1的终点=line2的起点返回3，line1的起点=line2的终点返回4，无法相连返回0
int CGeoPolygon::styleLineMerge(CGeoPolyline* line1, CGeoPolyline* line2)
{
	if (line1->pts[0]->GetCPoint()==line2->pts[0]->GetCPoint())
	{
		return 1;
	}else if (line1->pts[line1->pts.size()-1]->GetCPoint()==line2->pts[line2->pts.size()-1]->GetCPoint())
	{
		return 2;
	}else if (line1->pts[line1->pts.size()-1]->GetCPoint()==line2->pts[0]->GetCPoint())
	{
		return 3;
	}else if (line1->pts[0]->GetCPoint()==line2->pts[line2->pts.size()-1]->GetCPoint())
	{
		return 4;
	}else{
		return 0;
	}
}


// zf,0623，面的过滤,点
void CGeoPolygon::ObjectFilter(void)
{
	if(circlePointList.size() != 0)
		vector<int>().swap(circlePointList);
	//按点数过滤
	for(int i = 0;i<circles.size();i++)
	{
		int number = circles[i]->pts.size();
		circlePointList.push_back(number);
		if(number>159&&number<=1001)  //点数要自己确定阈值
		{
			//circles.pop_back();
			circles.erase(circles.begin() + i);
		}
	}
}

// zf,按面积过滤，0701
void CGeoPolygon::ObjectFilterByArea(void)
{
	//按面积
	vector<int> temp;  //定义数组，存噪点环
	if(circleAreaList.size()!=0) vector<double>().swap(circleAreaList);  //数组设定为空
	for(int i = 0;i<circles.size();i++)   //遍历环数组
	{
		double circlearea = circles[i]->getArea(circles[i]->pts);  //获取环的面积
		circleAreaList.push_back(circlearea);
		if(circlearea<=areaT) temp.push_back(i); //*****设定阈值*****，当面积小于阈值，噪点环数组添加该数值
	}
	vector<CGeoPolyline*> tempPolyline;     //定义新的无噪点环的环数组
	if(tempPolyline.size()!=0) vector<CGeoPolyline*>().swap(tempPolyline); //防止环数组不为空
	for(int j = 0;j<circles.size();j++)     //添加非噪点环到数组中
	{
		bool answer = isContained(temp,j);    //判断j是否存在temp中，若存在说明为冗余环，新circles中不应有此环
		if(answer == false)                   //若不存在说明不为冗余环，新circles应存此环
			tempPolyline.push_back(circles[j]);   
	}
	tempPolyline.swap(circles);         //交换，得到去除冗余环之后的环circles
}

// 获取多边形所有环的点数,0701
vector<int> CGeoPolygon::getPointNum(void)
{
	return circlePointList;
}

// 获取多边形所有环的面积,0701
vector<double> CGeoPolygon::getCircleArea(void)
{
	return circleAreaList;
}

// 寻找一般的外接矩形，zf，0704
void CGeoPolygon::FindRectangle(void)
{
	if(rectangleArray.size() != 0)
		vector<CPoint*>().swap(rectangleArray);
	for(int i = 0;i<circles.size();i++)
	{
		FindRectangle(circles[i]->pts);
	}
	hasFindRectangle = true;
	hasFindMABR = false;
	hasGetBuildings = false;
	//rotateAll();  //测试旋转
}

// 寻找某一个circle的绑定外接矩形,zf,0704
void CGeoPolygon::FindRectangle(vector<CMyPoint*> pts)
{
	//AfxMessageBox("一般的外接矩形!");
	int size = pts.size();
	if(size == 0)
		AfxMessageBox("该环为空");
	else
	{
		double Xmax = 0;
		double Ymax = 0;
		double Xmin = 60000000;  //最小值不能初始为0，
		double Ymin = 1000000000;  //最小值不能初始为0，
		for(int i = 0;i<size;i++)
		{
			double tempx = pts[i]->Getx();
			double tempy = pts[i]->Gety();
			if(tempx>=Xmax) Xmax = tempx;  //最大x，
			if(tempy>=Ymax) Ymax = tempy;  //最大y，
			if(tempx<=Xmin) Xmin = tempx;  //最小x
			if(tempy<=Ymin) Ymin = tempy;  //最小y
		}
		CPoint *pt1 = new CPoint(Xmax,Ymin);  //左上
		CPoint *pt2 = new CPoint(Xmax,Ymax);
		CPoint *pt3 = new CPoint(Xmin,Ymax);
		CPoint *pt4 = new CPoint(Xmin,Ymin);

		rectangleArray.push_back(pt1);
		rectangleArray.push_back(pt2);
		rectangleArray.push_back(pt3);
		rectangleArray.push_back(pt4);
	}
}

// 某一点pt绕center旋转theta角度，zf,0706
CPoint* CGeoPolygon::rotate(CPoint* pt, CPoint* center, double theta)
{
	double x1 = pt->x;
	double y1 = pt->y;
	double x0 = center->x;
	double y0 = center->y;

	double Q = theta / 180 * 3.1415926;  //角度
	double A = cos(Q);
	double B = sin(Q);
	double x2,y2;

	x2 = (x1-x0)*A-(y1-y0)*B+x0;   //旋转公式
	y2 = (x1-x0)*B+(y1-y0)*A+y0;

	CPoint* rotatePoint = new CPoint(x2,y2);
	//CPoint* rotatePoint = new CPoint((x1+10,y1+10));
	return rotatePoint;
}

// 旋转所有矩形，zf，0706，测试用
void CGeoPolygon::rotateAll(void)
{
	if(rectangleRotate.size() != 0)
		vector<CPoint*>().swap(rectangleRotate);
	if(rectangleArray.size()!= 0)
	{
		int xunhuan = rectangleArray.size()/4;
		for(int n = 0;n<xunhuan;n++)  //一个多边形可能有多个环，0706
		{
			double Cx = ((*(rectangleArray[1+n*4])).x + (*(rectangleArray[2+n*4])).x)/2;
			double Cy = ((*(rectangleArray[0+n*4])).y + (*(rectangleArray[1+n*4])).y)/2;
			CPoint* center = new CPoint(Cx,Cy);

			rectangleRotate.push_back(rotate(rectangleArray[0+n*4],center,45));
			rectangleRotate.push_back(rotate(rectangleArray[1+n*4],center,45));
			rectangleRotate.push_back(rotate(rectangleArray[2+n*4],center,45));
			rectangleRotate.push_back(rotate(rectangleArray[3+n*4],center,45));
		}
	}
}

// zf,0708，寻找最小外接矩形
void CGeoPolygon::FindMABR(void)
{
	if(rectangleRotate.size() != 0)    //*******
			vector<CPoint*>().swap(rectangleRotate);
	for(int i = 0;i<circles.size();i++)
	{
		circles[i]->MyPoint2Point(circles[i]->pts);       //pts转成odinPts
		CPoint* center = FindCenter(circles[i]->odinPts);  //寻找中心
		circles[i]->setCenter(center);

		if(circles[i]->MABR.size()!=0)     //*******
			vector<MABR_ZF*>().swap(circles[i]->MABR);

		for(int angle = 0;angle <= 90;angle++)   // -----1--------原始图形旋转0-90°
		{
			vector<CPoint*> tempArray;  //存储旋转后的环
			if(tempArray.size() != 0)  // //*******
				vector<CPoint*>().swap(tempArray);
			for(int j = 0;j<circles[i]->pts.size();j++)    //旋转某个环上每一点
				tempArray.push_back(rotate(circles[i]->odinPts[j],center,angle));  //旋转后的图形存入tempArray中
			vector<CPoint*> temp = FindRectangle(tempArray); //-----------2---------求旋转后的外接矩形
			if(temp.size() == 0)
				AfxMessageBox("简单外接矩形获取失败！");
			else
			{
				MABR_ZF *tempRect = new MABR_ZF();   //某个旋转角度时的外接矩形指针
				if(tempRect->vertices.size()!=0)     //*******
					vector<CPoint*>().swap(tempRect->vertices);
				for(int count = 0;count<temp.size();count++)    //将外接矩形顶点转移到circles的变量中
					tempRect->vertices.push_back(temp[count]);
				//求每个外接矩形的面积
				double deltaX = tempRect->vertices[2]->x - tempRect->vertices[0]->x;
				double deltaY = tempRect->vertices[2]->y - tempRect->vertices[0]->y;
				tempRect->area = deltaY * deltaX;
				tempRect->ID = angle;
				circles[i]->MABR.push_back(tempRect);  //MABR是用于存储旋转过程中每个外接矩形的数组，单组成元是每个对象的指针
			}
		}

		//------3-----------比较每个外接矩形，确定每个环面积最小的外接矩形
		int finalID;
		double compare = 60000000000;
		for(int num = 0;num<circles[i]->MABR.size();num++)
		{
			if(compare > circles[i]->MABR[num]->area)
			{
				finalID = circles[i]->MABR[num]->ID;
				compare = circles[i]->MABR[num]->area;
			}
		}
		for(int num = 0;num<circles[i]->MABR.size();num++)
		{
			if(circles[i]->MABR[num]->ID == finalID)
			{
				circles[i]->finalMABR.area = circles[i]->MABR[num]->area;
				circles[i]->finalMABR.ID = circles[i]->MABR[num]->ID;
				if(circles[i]->finalMABR.vertices.size()!=0)     //*******
					vector<CPoint*>().swap(circles[i]->finalMABR.vertices);
				for(int pointNum = 0; pointNum<circles[i]->MABR[num]->vertices.size();pointNum++)
					circles[i]->finalMABR.vertices.push_back(circles[i]->MABR[num]->vertices[pointNum]);
				break;
			}
		}

		//----------4-------将外接矩形朝相反的方向旋转相同度数
		int finalAngle = circles[i]->finalMABR.ID;
		for(int final = 0;final<circles[i]->finalMABR.vertices.size();final++)
			rectangleRotate.push_back(rotate(circles[i]->finalMABR.vertices[final],center,-finalAngle));  //！！！此处角度相反
	}
	hasFindMABR = true;
	hasFindRectangle = false;
	hasGetBuildings = false;
}

// 寻找环的中心（重心），zf，0708
CPoint* CGeoPolygon::FindCenter(vector<CPoint*> ptsArray)
{
	double tempX,tempY;
	double sumX = 0;
	double sumY = 0;
	int size = ptsArray.size();
	for(int i = 0;i<size;i++)
	{
		sumX = sumX + ptsArray[i]->x;
		sumY = sumY + ptsArray[i]->y;
	}
	tempX = sumX/size;
	tempY = sumY/size;
	CPoint* pt = new CPoint(tempX,tempY);
	return pt;
}

// 某多边形的一般外接矩形，zf，0708
vector<CPoint*> CGeoPolygon::FindRectangle(vector<CPoint*> pts)
{
	vector<CPoint*> temp;
	//int size = pts.size();
	if(pts.size() == 0)
		AfxMessageBox("该环为空");
	else
	{
		double Xmax = 0;
		double Ymax = 0;
		double Xmin = 6000000000;  //最小值不能初始为0，
		double Ymin = 1000000000;  //最小值不能初始为0，
		for(int i = 0;i<pts.size();i++)
		{
			double tempx = pts[i]->x;
			double tempy = pts[i]->y;
			if(tempx>=Xmax) Xmax = tempx;  //最大x，
			if(tempy>=Ymax) Ymax = tempy;  //最大y，
			if(tempx<=Xmin) Xmin = tempx;  //最小x
			if(tempy<=Ymin) Ymin = tempy;  //最小y
		}
		
		CPoint *pt0 = new CPoint(Xmin,Ymin);  //左上
		CPoint *pt1 = new CPoint(Xmax,Ymin);  //右上
		CPoint *pt2 = new CPoint(Xmax,Ymax);  //右下
		CPoint *pt3 = new CPoint(Xmin,Ymax);  //左下

		temp.push_back(pt0);
		temp.push_back(pt1);
		temp.push_back(pt2);
		temp.push_back(pt3);
		
	}
	return temp;
}

// 去除冗余点，zf，0717
void CGeoPolygon::RemoveRedundant(void)
{
	for(int count = 0; count<times;count++) GetHalfPoints();

	for(int i = 0;i<circles.size();i++)  //遍历每个circles
	{
		vector<int> temp;                //用于存放冗余点的下标
		bool odinary = true;             //设定状态，以确定三点单元的首点
		int count = 0;                   //记录删除的点数，一旦有正常情况恢复为0
		int ptsNum = circles[i]->pts.size();
		for(int n = 0;n<ptsNum-2;n++)
		{
			CMyPoint*first,*middle,*last;  //定义三点
			if(odinary)   //正常，没有冗余点的情况
			{
				first = circles[i]->pts[n];
				middle = circles[i]->pts[n+1];
				last = circles[i]->pts[n+2];
			}
			else         //不正常，存在冗余点的情况
			{
				first = circles[i]->pts[n-count];   //存在冗余点，删除后判断冗余点之后的点是否为冗余点还需要之前三点单元的首点
				middle = circles[i]->pts[n+1];
				last = circles[i]->pts[n+2];
			}
			double angle = calAngle(first,middle,last);   //计算角度
			if(angle >= 20&&angle<=160)         //角度阈值设为10°,当角度大于阈值，情况正常，不需要去除点
			{
				odinary = true;
				count = 0;                 //记录删除的点数，一旦有正常情况恢复为0
			}
			else   // 存在冗余点
			{
				temp.push_back(n+1);     //记录冗余点在circles[i]->pts中下标
				odinary = false;         //情况不正常
				count++;                 //连续不正常时都要记录
			}
		}
		vector<CMyPoint*> tempPoints;    //定义临时点集
		if(tempPoints.size()!=0) vector<CMyPoint*>().swap(tempPoints); //防止点集不为空
		for(int j = 0;j<ptsNum;j++)
		{
			bool answer = isContained(temp,j);    //判断j是否存在temp中，若存在说明为冗余点，新数组中不应有此点
			if(answer == false)                   //若不存在说明不为冗余点，新数组应存此点
				tempPoints.push_back(circles[i]->pts[j]);   
		}
		tempPoints.swap(circles[i]->pts);         //交换，得到去除冗余点之后的环circles[i]->pts
	}
}

// 0904,zf,保留一半的点
void CGeoPolygon::GetHalfPoints(void)
{
	for(int i = 0;i<circles.size();i++){
		vector<CMyPoint*> tempPoints;    //定义临时点集
		if(tempPoints.size()!=0) vector<CMyPoint*>().swap(tempPoints); //防止点集不为空
		for(int j = 0;j<circles[i]->pts.size();j++){
			if(j%2==0) tempPoints.push_back(circles[i]->pts[j]);
		}
		tempPoints.swap(circles[i]->pts);
	}
}


// 计算三点之间的角度，zf，0717
double CGeoPolygon::calAngle(CMyPoint* p1, CMyPoint* p2, CMyPoint* p3)
{
	double x1 = p1->Getx();
	double y1 = p1->Gety();
	double x2 = p2->Getx();
	double y2 = p2->Gety();
	double x3 = p3->Getx();
	double y3 = p3->Gety();

	double angle,p1p2,p2p3,p1p3;
	p1p2 = sqrt(((x1 - x2) * (x1 - x2) + (y1 -y2) * (y1 -y2)));
	p2p3 = sqrt(((x3 - x2) * (x3 - x2) + (y3 -y2) * (y3 -y2)));
	p1p3 = sqrt(((x1 - x3) * (x1 - x3) + (y1 -y3) * (y1 -y3)));
	angle = acos((p1p2 * p1p2 + p2p3 * p2p3 - p1p3 * p1p3) / (2 * p1p2 * p2p3)) * 180.0 / 3.1415926;
	if(angle<0)
		angle = angle*(-1);
    return angle;
}

// 判断某个数是不是在数组中，zf，0717
bool CGeoPolygon::isContained(vector<int> temp, int i)
{
	bool answer = false;
	for(int j = 0;j<temp.size();j++)
	{
		if(i == temp[j]) answer = true;
		if(answer) break;
	}
	return answer;
}

// 计算 建筑物占最小外接矩形的比例，建筑物/最小外接矩形 ，zf，0712
bool CGeoPolygon::areaProportion(void)
{
	bool hasgetMABR = true;
	for(int i = 0;i<circles.size();i++)
	{
		double circleArea = circles[i]->getArea(circles[i]->pts);
		if(circles[i]->finalMABR.vertices.size() == 0) hasgetMABR = false;  //最小外接矩形不存在时
		else
		{
			double deltaX = circles[i]->finalMABR.vertices[0]->x - circles[i]->finalMABR.vertices[2]->x;
			double deltaY = circles[i]->finalMABR.vertices[0]->y - circles[i]->finalMABR.vertices[2]->y;
			//double MABRArea = deltaX * deltaY;
			circles[i]->areaProportion = circleArea/(deltaX * deltaY);
		}
	}
	return hasgetMABR;
}

// 最小外接矩形优化，0712，zf
bool CGeoPolygon::optimize(void)
{
	hasFindMABR==false;
	//------1------- 求出所有多边形的面积占比
	bool hasgetMABR = areaProportion();
    //------2------  根据不同比例来实施不同操作
	if(hasgetMABR)
	{
		for(int i = 0 ;i<circles.size();i++)
		{
			if(circles[i]->areaProportion<0.9) fitting(circles[i]); //不超过90%，则最小外接矩形需拟合处理；超过90%，则最小外接矩形就是最后结果
		}
	}
	return hasgetMABR;
}

// MABR拟合建筑物多边形，zf，0719
void CGeoPolygon::fitting(CGeoPolyline* line)
{
	vector<CPoint*> tempArray;  //存储得到MABR时的环的坐标
	for(int i = 0;i<line->odinPts.size();i++)
		tempArray.push_back(rotate(line->odinPts[i],&(line->center),line->finalMABR.ID)); //旋转环上每一点
	tempArray.swap(line->odinPts);  //交换0919

	//------现有平行于xy轴的line->finalMABR，又有此情况下的建筑物多边形的点 line->odinPts
	//------先计算外切的四个交点，得出Xmax，Xmin，Ymax，Ymin,判断上下链还是左右链
	vector<CPoint*> temp = FindRectangle(line->odinPts); 
	double Ymin = temp[0]->y, Xmin = temp[0]->x, Xmax = temp[2]->x, Ymax = temp[2]->y;

	double y1,x2,y3,x4;
	int i1,i2,i3,i4;
	for(int i = 0;i<line->odinPts.size();i++){
		if(line->odinPts[i]->x == Xmin)  i1 = i;  //左
		if(line->odinPts[i]->y == Ymax)  i2 = i;  //上
		if(line->odinPts[i]->x == Xmax)  i3 = i;  //右
		if(line->odinPts[i]->y == Ymin)  i4 = i;  //下
	}
	y1 = line->odinPts[i1]->y;x2 = line->odinPts[i2]->x;y3 = line->odinPts[i3]->y;x4 = line->odinPts[i4]->x;
	vector<CPoint*> line1,line2,line3,line4;
	for(int j = 0;j<line->odinPts.size();j++){
		double compareX = line->odinPts[j]->x,compareY = line->odinPts[j]->y;  //划分很重要！！！
		if(i2<i3){     
			if( (compareX<=x2&&compareY>y1) && (j<=i2||j>=i1) ) line1.push_back(line->odinPts[j]);
			if( (compareX>x2&&compareY>=y3) && (j>=i2&&j<=i3) ) line2.push_back(line->odinPts[j]);
			if( (compareX>=x4&&compareY<y3) && (j>=i3&&j<=i4) ) line3.push_back(line->odinPts[j]);
			if( (compareX<x4&&compareY<=y1) && (j<=i1&&j>=i4) ) line4.push_back(line->odinPts[j]);
		}
		else{
			if( (compareX<=x2&&compareY>y1) && (j>=i1&&j<=i2) ) line1.push_back(line->odinPts[j]);
			if( (compareX>x2&&compareY>=y3) && (j<=i2||j>=i3) ) line2.push_back(line->odinPts[j]);
			if( (compareX>=x4&&compareY<y3) && (j>=i3&&j<=i4) ) line3.push_back(line->odinPts[j]);
			if( (compareX<x4&&compareY<=y1) && (j<=i1&&j>=i4) ) line4.push_back(line->odinPts[j]);
		}
	}
	int p1 = 0,p2 = 0 ,p3 = 0 ,p4 = 0 ;
	double deltaX,deltaY,tempArea;
	double compareArea1 = 0;
	for(int i = 0;i<line1.size();i++){
		deltaX = line1[i]->x - Xmin;
		deltaY = Ymax - line1[i]->y;
		tempArea = deltaY * deltaX;
		if(tempArea >= compareArea1){
			compareArea1 = tempArea;
			p1 = i;
		}
	}
	if(line1.size()==0) line1.push_back(&CPoint(Xmin,Ymax));
	double compareArea2 = 0;
	for(int i = 0;i<line2.size();i++){
		deltaX = Xmax - line2[i]->x;
		deltaY = Ymax - line2[i]->y;
		tempArea = deltaY * deltaX;
		if(tempArea >= compareArea2){
			compareArea2 = tempArea;
			p2 = i;
		}
	}
	if(line2.size()==0) line2.push_back(&CPoint(Xmin,Ymax));
	double compareArea3 = 0;
	for(int i = 0;i<line3.size();i++){
		double tempArea = 0;
		deltaX = Xmax - line3[i]->x;
		deltaY = line3[i]->y - Ymin;
		tempArea = deltaY * deltaX;
		if(tempArea >= compareArea3){
			compareArea3 = tempArea;
			p3 = i;
		}
	}
	if(line3.size()==0) line3.push_back(&CPoint(Xmin,Ymax));
	double compareArea4 = 0;
	for(int i = 0;i<line4.size();i++){
		double tempArea = 0;
		deltaX = line4[i]->x - Xmin;
		deltaY = line4[i]->y - Ymin;
		tempArea = deltaY * deltaX;
		if(tempArea >= compareArea4){
			compareArea4 = tempArea;
			p4 = i;
		}
	}
	if(line4.size()==0) line4.push_back(&CPoint(Xmin,Ymax));

	vector<CPoint*> outcome;
	double minArea = areaT;
	
	bool proper1 = !(line1[p1]->x>=line3[p3]->x||line1[p1]->y<=line3[p3]->y);   //出现异位 则为真
	bool proper2 = !(line2[p2]->x<=line4[p4]->x||line2[p2]->y<=line3[p3]->y);
	if(proper1&&proper2){
		if(line1.size()!=0){
			if(compareArea1>=minArea){
				CPoint *pt1 = new CPoint(Xmin,line1[p1]->y);
				outcome.push_back(pt1);
				outcome.push_back(line1[p1]);
				CPoint *pt2 = new CPoint(line1[p1]->x,Ymax);
				outcome.push_back(pt2);
			}
			else outcome.push_back(temp[3]);
		}
		if(line2.size()!=0){
			if(compareArea2>=minArea){
				CPoint *pt3 = new CPoint(line2[p2]->x,Ymax);
				outcome.push_back(pt3);
				outcome.push_back(line2[p2]);
				CPoint *pt4 = new CPoint(Xmax,line2[p2]->y);
				outcome.push_back(pt4);
			}
			else outcome.push_back(temp[2]);
		}
		if(line3.size()!=0){
			if(compareArea3>=minArea){
				CPoint *pt5 = new CPoint(Xmax,line3[p3]->y);
				outcome.push_back(pt5);
				outcome.push_back(line3[p3]);
				CPoint *pt6 = new CPoint(line3[p3]->x,Ymin);
				outcome.push_back(pt6);
			}
			else outcome.push_back(temp[1]);
		}
		if(line4.size()!=0){
			if(compareArea4>=minArea){
				CPoint *pt7 = new CPoint(line4[p4]->x,Ymin);
				outcome.push_back(pt7);
				outcome.push_back(line4[p4]);
				CPoint *pt8 = new CPoint(Xmin,line4[p4]->y);
				outcome.push_back(pt8);
			}
			else outcome.push_back(temp[0]);
		}

		CPoint* pt = outcome[0];
		outcome.push_back(pt);
	}
	if(!proper1){
		if(line1.size()!=0){
			if(compareArea1>=compareArea3){
				if(compareArea1>=minArea){
					CPoint *pt1 = new CPoint(Xmin,line1[p1]->y);
					outcome.push_back(pt1);
					outcome.push_back(line1[p1]);
					CPoint *pt2 = new CPoint(line1[p1]->x,Ymax);
					outcome.push_back(pt2);
				}
				else outcome.push_back(temp[3]);
			}
			else outcome.push_back(temp[3]);
		}

		if(line2.size()!=0){
			if(compareArea2>=minArea){
				CPoint *pt3 = new CPoint(line2[p2]->x,Ymax);
				outcome.push_back(pt3);
				outcome.push_back(line2[p2]);
				CPoint *pt4 = new CPoint(Xmax,line2[p2]->y);
				outcome.push_back(pt4);
			}
			else outcome.push_back(temp[2]);
		}
		if(line3.size()!=0){
			if(compareArea1<compareArea3){
				if(compareArea3>=minArea){
					CPoint *pt5 = new CPoint(Xmax,line3[p3]->y);
					outcome.push_back(pt5);
					outcome.push_back(line3[p3]);
					CPoint *pt6 = new CPoint(line3[p3]->x,Ymin);
					outcome.push_back(pt6);
				}
				else outcome.push_back(temp[1]);
			}
			else outcome.push_back(temp[1]);
		}
		if(line4.size()!=0){
			if(compareArea4>=minArea){
				CPoint *pt7 = new CPoint(line4[p4]->x,Ymin);
				outcome.push_back(pt7);
				outcome.push_back(line4[p4]);
				CPoint *pt8 = new CPoint(Xmin,line4[p4]->y);
				outcome.push_back(pt8);
			}
			else outcome.push_back(temp[0]);
		}

		CPoint* pt = outcome[0];
		outcome.push_back(pt);
	}

	if(!proper2){
		if(line1.size()!=0){
			if(compareArea1>=minArea){
				CPoint *pt1 = new CPoint(Xmin,line1[p1]->y);
				outcome.push_back(pt1);
				outcome.push_back(line1[p1]);
				CPoint *pt2 = new CPoint(line1[p1]->x,Ymax);
				outcome.push_back(pt2);
			}
			else outcome.push_back(temp[3]);
		}
		if(line2.size()!=0){
			if(compareArea2>compareArea4){
				if(compareArea2>=minArea){
					CPoint *pt3 = new CPoint(line2[p2]->x,Ymax);
					outcome.push_back(pt3);
					outcome.push_back(line2[p2]);
					CPoint *pt4 = new CPoint(Xmax,line2[p2]->y);
					outcome.push_back(pt4);
				}
				else outcome.push_back(temp[2]);
			}
			else outcome.push_back(temp[2]);
		}
		if(line3.size()!=0){
			if(compareArea3>=minArea){
				CPoint *pt5 = new CPoint(Xmax,line3[p3]->y);
				outcome.push_back(pt5);
				outcome.push_back(line3[p3]);
				CPoint *pt6 = new CPoint(line3[p3]->x,Ymin);
				outcome.push_back(pt6);
			}
			else outcome.push_back(temp[1]);
		}
		if(line4.size()!=0){
			if(compareArea2<=compareArea4){
				if(compareArea4>=minArea){
					CPoint *pt7 = new CPoint(line4[p4]->x,Ymin);
					outcome.push_back(pt7);
					outcome.push_back(line4[p4]);
					CPoint *pt8 = new CPoint(Xmin,line4[p4]->y);
					outcome.push_back(pt8);
				}
				else outcome.push_back(temp[0]);
			}
			else outcome.push_back(temp[0]);
		}

		CPoint* pt = outcome[0];
		outcome.push_back(pt);
	}
	
    int finalAngle = line->finalMABR.ID;
	if(buildings.size() != 0) vector<CPoint*>().swap(buildings);
	for(int final = 0;final<outcome.size();final++)
		buildings.push_back(rotate(outcome[final],&(line->center),-finalAngle));  //！！！此处角度相反

	hasFindRectangle = false;
	hasFindMABR = false;
	hasGetBuildings = true;
}

// 去除尾巴 1003 zf
void CGeoPolygon::DeleteTail(void)
{
	for(int i = 0 ; i < circles.size();i++){

		int firstNum = circles[i]->pts.size();
		int lastNum = 0;
		int t = circles[i]->pointDisThreshold; //下标差阈值
		double length = circles[i]->calAvgDis(circles[i]->pts);  //距离阈值
		int count = 0;
		while( count<1 && firstNum!=lastNum )
		{
			firstNum = circles[i]->pts.size();
			int num = circles[i]->pts.size()-1;  //环的点数(去除尾点)
			int finalJ = 0, finalK = 0, FinalDis = 0, finalType = 0;  //最后J,K,类型
			if(num > t) { //环的总点数大于点数差阈值时

				//找到所有环中下标差最大的两点
				for(int j = 0 ; j < (num - t); j++){ //遍历环的每个点，
					int tempDis = 0, tempk = 0, temptype = 0;
					for(int k = (j + t) ; k<num && (j+num-k)>=t ; k++) {  //针对每一个点，计算该点与符合要求的点的距离，找到下标差最大的另一个点,
						int dis = (j+num-k)>=(k-j)?(k-j):(j+num-k);   //取小的
						int type = (j+num-k)>=(k-j)?1:2;   // 1代表顺序，2代表非顺序
						if( (calDis(circles[i]->pts[j],circles[i]->pts[k])<(length*3)) && (tempDis<dis) ){  //寻找下标差最大
							tempDis = dis;
							tempk = k;
							temptype = type;
						}
					}
					if( FinalDis<=tempDis ){
						FinalDis = tempDis;
						finalJ = j;
						finalK = tempk;
						finalType = temptype;
					}
				}	
			}
			//现有 J K 两个下标，判断两个下标直接的点组成的环面积是否超过阈值,
			switch(finalType){
			case 1:
				{
					CGeoPolyline* line = new CGeoPolyline();
					for(int ii = finalJ; ii<finalK+1;ii++)   line->pts.push_back(circles[i]->pts[ii]);
					line->pts.push_back(circles[i]->pts[finalJ]);
					if(line->getArea(line->pts)<= (1*areaT)){
						CGeoPolyline *tmp = new CGeoPolyline();
						for(int iii = 0 ; iii<circles[i]->pts.size();iii++)
							if(iii<finalJ||iii>finalK) tmp->pts.push_back(circles[i]->pts[iii]);
						tmp->pts.swap(circles[i]->pts);
					}
					else{
						/*CGeoPolyline *tmp = new CGeoPolyline();
						CGeoPolyline *othertmp = new CGeoPolyline();
						for(int iii = 0 ; iii<circles[i]->pts.size();iii++)
						{
							if(iii<finalJ||iii>finalK) tmp->pts.push_back(circles[i]->pts[iii]);
							else othertmp->pts.push_back(circles[i]->pts[iii]);
						}
						tmp->pts.swap(circles[i]->pts);
						if(othertmp->pts.size()!=0){
							othertmp->pts.push_back(othertmp->pts[0]);
							circles.push_back(othertmp);
						}*/
					}
					break;
				}
			case 2:
				{
					CGeoPolyline* line = new CGeoPolyline();
					for(int ii = 0; ii<finalJ+1;ii++)   line->pts.push_back(circles[i]->pts[ii]);
					for(int jj = finalK; jj<circles[i]->pts.size();jj++)  line->pts.push_back(circles[i]->pts[jj]);
					line->pts.push_back(circles[i]->pts[0]);
					if(line->getArea(line->pts)<= (1*areaT)){   //判断面积
						CGeoPolyline *tmp = new CGeoPolyline();  //若小于，则tmp为新去掉尾巴点的
						for(int iii = 0 ; iii<circles[i]->pts.size();iii++)
							if(iii>finalJ&&iii<finalK) tmp->pts.push_back(circles[i]->pts[iii]);
						if(tmp->pts.size()!=0) tmp->pts.push_back(tmp->pts[0]);
						tmp->pts.swap(circles[i]->pts);   //交换
					}
					else{
						/*CGeoPolyline *tmp = new CGeoPolyline();
						CGeoPolyline *othertmp = new CGeoPolyline();
						for(int iii = 0 ; iii<circles[i]->pts.size();iii++)
						{
							if(iii>finalJ&&iii<finalK) tmp->pts.push_back(circles[i]->pts[iii]);
							else othertmp->pts.push_back(circles[i]->pts[iii]);
						}
						tmp->pts.swap(circles[i]->pts);
						if(othertmp->pts.size()!=0){
							othertmp->pts.push_back(othertmp->pts[0]);
							circles.push_back(othertmp);
						}*/
					}
					break;
				}
			case 0:
					break;
			}
			lastNum = circles[i]->pts.size();
			count++;
		}
		
	}
}


// 计算两点的距离  1003 zf
double CGeoPolygon::calDis(CMyPoint* pt1, CMyPoint* pt2)
{
	double deltaX = pt1->Getx() - pt2->Getx();
	double deltaY = pt1->Gety() - pt2->Gety();
	return sqrt(deltaX*deltaX+deltaY*deltaY);
}
