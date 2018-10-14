#include "stdafx.h"
#include"GeoLayer.h"
#include"GeoPolygon.h"
#include"GeoPolyline.h"
#include"GeoPoint.h"
#include"GeoRoad.h"
#include"MainFrm.h"
#include<string.h>
using namespace std;


CGeoLayer::CGeoLayer()
{
	isTopLayer = false;
	isPolygonSimply = false;
	IsLineSimply = 0;
}


CGeoLayer::~CGeoLayer()
{	
	for (vector<CGeoObject*>::iterator itb = objects.begin();
		itb != objects.end(); ++itb) {
		CGeoObject* object = *itb;
		if (object) {
			delete object;
			object = NULL;
		}
	}
	objects.clear();
	objects.swap(vector<CGeoObject*>());

	for (vector<CMyPoint*>::iterator itb = commonPoint.begin();
		itb != commonPoint.end(); ++itb) {
		CMyPoint* mypoint = *itb;
		if (mypoint) {
			delete mypoint;
			mypoint = NULL;
		}
	}
	commonPoint.clear();
	commonPoint.swap(vector<CMyPoint*>());

	for (vector<CGeoPolyline*>::iterator itc = commonEdge.begin();
		itc != commonEdge.end(); ++itc) {
		CGeoPolyline* line = *itc;
		if (line) {
			delete line;
			line = NULL;
		}
	}
	commonEdge.clear();
	commonEdge.swap(vector<CGeoPolyline*>());

	for (vector<CGeoObject*>::iterator itd = objectsTemp.begin();
		itd != objectsTemp.end(); ++itd) {
		CGeoObject* object = *itd;
		if (object) {
			delete object;
			object = NULL;
		}
	}
	objectsTemp.clear();
	objectsTemp.swap(vector<CGeoObject*>());

	for (vector<CRect*>::iterator ite = objectMABRs.begin();
		ite != objectMABRs.end(); ++ite) {
		CRect* rect = *ite;
		if (rect) {
			delete rect;
			rect = NULL;
		}
	}
	objectMABRs.clear();
	objectMABRs.swap(vector<CRect*>());
}

void CGeoLayer::Draw(CDC * pdc, CMyDCConfig myCDcfg)
{
	int style = myCDcfg.style;
	if (style == 1) {//通过polygon的公共边数组绘制
		for (i = 0; i < objects.size(); i++) {
			objects[i]->Draw(pdc, myCDcfg);
		}
	}
	else if(style == 2) {//通过commonedge绘制
		if (commonEdge.size() == 0) {
			CString str;
			str = "未构建拓扑，请先构建拓扑";
			AfxMessageBox(str);
			myCDcfg.style = 3;
			this->Draw(pdc, myCDcfg);
		}
		else {
			COLORREF lineColor = RGB(255, 0, 0);
			myCDcfg.lineColor = lineColor;
			for (i = 0; i < objects.size(); i++) {
				
				objects[i]->Draw(pdc, myCDcfg);
			}
		}
	}
	else if (style == 3) {//通过circle绘制
		COLORREF lineColor = RGB(0, 0, 0);
		myCDcfg.lineColor = lineColor;
		int objectNum = 0;
		for (i = 0; i < objects.size(); i++) {
			//if (i < objects.size() - 1) continue;
			objects[i]->Draw(pdc, myCDcfg);
			objectNum = objectNum + 1;
		}
	}
	
	else if (style == 4) {//加载原图
		myCDcfg.style = 3;
		this->Draw(pdc, myCDcfg);
		myCDcfg.style = 2;
		this->Draw(pdc, myCDcfg);
	}
	else if (style == 5) {//保存使用
		COLORREF lineColor = RGB(0, 0, 200);
		myCDcfg.lineColor = lineColor;
		for (i = 0; i < objects.size(); i++) {
			//if (i < objects.size() - 1) continue;
			objects[i]->Draw(pdc, myCDcfg);
		}
	}
	
}


CGeoObject* CGeoLayer::GetObjectAt(int index)
{
	return objects[index];
}

void CGeoLayer::LayerGeneral()
{
	for (i = 0; i < objects.size(); i++) {
		objects[i]->BesidesObjectSearch();
		objects[i]->ObjectMerge();
		objects[i]->ObjectDelete();
		objects[i]->ObjectMerge();
		objects[i]->ObjectSimplify();
		objects[i]->ObjectShift();
	}
}


void CGeoLayer::TopoBuild()
{
	this->BuildMABR();//将所有对象的MABR加入到数组中
	this->GetObjectIntersectMABR();//求每个对象相交的最小外接矩形，并将对应在数组objectMABRs中的位置加入到该object的数组中
	this->EndPointGetValue();
	
	this->pointCompared();
	this->AddAllBoundary();
	this->buildCircle();
	//this->deleteCircle();//释放circle的内存，同时构建commonCircle
}

void CGeoLayer::AddObject(CGeoObject * obj, vector<CGeoObject*>* objs)
{
	(*objs).push_back(obj);
	obj->PosinCommonLayer = (*objs).size() - 1;//同时得到每个object在数组中的下标
}

int CGeoLayer::GetObjectNum()
{
	return objects.size();
}

void CGeoLayer::pointCompared(CGeoPolygon * polygonObject1, CGeoPolygon * polygonObject2)
{
	int circleNum1 = polygonObject1->GetCircleNum();
	int circleNum2 = polygonObject2->GetCircleNum();
	int lll=1;
	for (int i = 0; i < circleNum1; i++) {
		CGeoPolyline* pline1 = polygonObject1->circles[i];//第i个环
		pline1->circleID = i;
		pline1->polygonID = polygonObject1->PosinCommonLayer;
		for (int j = 0; j < circleNum2; j++) {
			CGeoPolyline* pline2 = polygonObject2->circles[j];//第j个环
			pline2->circleID = j;
			pline2->polygonID = polygonObject2->PosinCommonLayer;//j=6
			
			this->pointCompared(pline1, pline2);
			if(i==1)
				lll=0;
		}
	}
	
}

void CGeoLayer::pointCompared(CGeoPolyline * circle1, CGeoPolyline * circle2)
{//加端点数组
	for (int i = 0; i < circle1->GetSize()-1; i++) {//，circle动态变化，同时控制i的变化
		CMyPoint* pt1 = circle1->pts[i];
		CMyPoint* pt3 = circle1->pts[i+1];
		if (pt1->Getx() != -1&&pt3->Getx() != -1) {
			for (int j = 0; j <circle2->GetSize(); j++) {
				pt1=circle1->pts[i];
				CMyPoint* pt2 = circle2->pts[j];
				if (pt1->GetCPoint() == pt2->GetCPoint()) {//如果pt1不是（-1，-1）！！！pt1竟然变了
					LinePointPosition linePointPos;
					int polygon1 = circle1->polygonID;
					int polygon2 = circle2->polygonID;
					int circle1ID = circle1->circleID;
					int circle2ID = circle2->circleID;
					linePointPos = this->findCommonPointInLine(polygon1, circle1ID, i, polygon2, circle2ID, j);
					if (linePointPos.line1Ori == -1) continue;
					circle1->removePoints(linePointPos.line1Ori, linePointPos.line1End,linePointPos.posInCommonLayer);
					circle2->removePoints(linePointPos.line2Ori, linePointPos.line2End,linePointPos.posInCommonLayer);
					int ssss=1;
				}
			}
		}
	}
}

void CGeoLayer::pointCompared()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	long t1 = GetTickCount();//程序段开始前取得系统运行时间(ms)  
	int size = objectsTemp.size();
	for (int i = 0; i < size; i++) {
		int sss=0;
		if(i==118)
			sss=1;
		if(i==200)
			sss=1;
		if(i==300)
			sss=1;
		if(i==400)
			sss=1;
		this->pointCompared(this->objectsTemp[i]);//寻找所有公共点，同时记录了每个公共点的点面拓扑关系
		((CGeoPolygon*)objects[i])->isTopoed = true;
	
		
		CString str;
		double part = ((double)i) / ((double)size);
		str.Format("进度：%5.3f", part*100*pow((100-part),2/5));
		str = str + "%    ";
		long t2 = GetTickCount();//程序段结束后取得系统运行时间(ms)    
		CString str1;
		str1.Format("已用时:%dms", t2 - t1);//前后之差即程序运行时间      
		str += str1;
		pFrame->m_wndStatusBar.SetPaneText(0, str);
	}	
}

void CGeoLayer::pointCompared(CGeoObject * object)
{
	CGeoPolygon* object1 = (CGeoPolygon*)(object);
	if (object->intersectMABRpos.size() != 0){
		int nums = object->intersectMABRpos.size();
		for (int i = 0; i <nums; i++) {//对object中的相交最小外接矩形数组进行遍历
			int m = object->intersectMABRpos[i];//得到最小外接矩形数组中第i个位置存放的数值，
			CGeoPolygon* polygon = (CGeoPolygon*)(this->objectsTemp[m]);//得到该数值对应的objects数组中对应的object
			this->pointCompared(object1, polygon);//二者求公共边端点 i=5
			int n = object->PosinCommonLayer;//object在map数组中的位置
			for (vector<long int>::iterator it = objectsTemp[m]->intersectMABRpos.begin(); it != objectsTemp[m]->intersectMABRpos.end();) {//比完一次就将object[m]中对应的object编号除去，防止重复比较
				if (*it == n) {
					it = objectsTemp[m]->intersectMABRpos.erase(it);
					break;
				}
				else {
					++it;
				}
			}
		}
		object1->isTopoed = true;
	}	
}

void CGeoLayer::AddAllBoundary()//原则之一就是若存在相邻两个坐标均不为（-1，-1），则可成链
{
	int size = objectsTemp.size();
	double sss = 0;
	
	for (int i = 0; i < size; i++) {
		
		if (i ==0)
		{
			sss = 1;
		}
		if (i ==100)
		{
			sss = 1;
		}if (i ==251)
		{
			sss = 1;
		}if (i == 400)
		{
			sss = 1;
		}
		CGeoPolygon* polygon = (CGeoPolygon*)(objectsTemp[i]);
		this->AddBoundary(polygon);
	}
}

void CGeoLayer::AddBoundary(CGeoPolygon * polygon)
{
	int size = polygon->circles.size();
	for (int i = 0; i < size; i++) {
		this->AddBoundary(polygon->circles[i]);
	}
}

void CGeoLayer::AddBoundary(CGeoPolyline * circle)//首尾未考虑
{
	int circleID = circle->circleID;
	int polygonID = circle->polygonID;
	CGeoPolygon* polygon = (CGeoPolygon*)(this->objects[polygonID]);

	if (circle->pts.size() < 3) return;
	int ori = 0;
	int end = 0;
	if (circle->pts[0]->Getx() != -1 &&circle->pts[1]->Getx() != -1 &&circle->pts[circle->pts.size() - 2]->Getx() != -1) {//说明存在起点坐标大于终点坐标的存在
		CGeoPolyline *polyline = new CGeoPolyline();
		int flag = 0;
		for (int i = circle->pts.size() - 2; (circle->pts[i]->Getx() != -1)&&i>=0; i--) {
			ori = i;
			int pos = circle->pts[i]->ptID;
			CMyPoint* myPoint = polygon->circles[circleID]->pts[pos];
			if (!myPoint->isCommon) {//这个得在object里面判断或者换一种
				this->commonPoint.push_back(myPoint);
				myPoint->PosInCommonPoint = commonPoint.size() - 1;
				myPoint->isCommon = true;
			}
			polyline->pts.push_back(myPoint);
			if (i == 0) {
				flag = 1;
				break;
			}
		}
		if(polyline->pts.size()>1)
			reverse(polyline->pts.begin(),polyline->pts.end());
		if (flag == 1) {
			polyline->pts.push_back(polygon->circles[circleID]->pts[0]);
			end = polyline->pts.size() - 1;
			goto loop;
		}

		for (int i = 0; (circle->pts[i]->Getx() != -1)&&i<circle->pts.size(); i++) {
			end = i;
			int pos = circle->pts[i]->ptID;
			CMyPoint* myPoint = polygon->circles[circleID]->pts[pos];
			if (!myPoint->isCommon) {//这个得在object里面判断或者换一种
				this->commonPoint.push_back(myPoint);
				myPoint->PosInCommonPoint = commonPoint.size() - 1;
				myPoint->isCommon = true;
			}
			polyline->pts.push_back(myPoint);
		}
		loop:
		this->commonEdge.push_back(polyline);
		polyline->PosinCommonLayer = this->commonEdge.size() - 1;
		polyline->rightAttribute = polygon->objectAttribute;
		polygon->commonEdge.push_back(polyline);
		polyline->rightPolygonID = polygonID;
		polyline->leftPolygonID = -1;
		//polygon->commoncircles[circleID].push_back(polyline);
			
		circle->removePoints(ori, end, polyline->PosinCommonLayer);
	}
	
	for (int i = 0; i <  circle->pts.size()-1; i++) {	
		if (circle->pts[i]->Getx() != -1 &&circle->pts[i+1]->Getx() != -1) {
			CGeoPolyline *polyline = new CGeoPolyline();
			ori = i;
			int j = i;
			while (circle->pts[j]->Getx() != -1 &&j<circle->pts.size()-1) {
				end = j;				
				int pos = circle->pts[j]->ptID;
				CMyPoint* myPoint = polygon->circles[circleID]->pts[pos];
				if (!myPoint->isCommon) {//这个得在object里面判断或者换一种
					this->commonPoint.push_back(myPoint);
					myPoint->PosInCommonPoint = commonPoint.size() - 1;
					myPoint->isCommon = true;
				}
				
				polyline->pts.push_back(myPoint);
				if (j == circle->pts.size() - 2) {
					polyline->pts.push_back(polygon->circles[circleID]->pts[0]);
					end++;
				}
				j++;
			}
			this->commonEdge.push_back(polyline);
			polyline->PosinCommonLayer = this->commonEdge.size() - 1;
			polygon->commonEdge.push_back(polyline);
			polyline->rightPolygonID = polygonID;
			polyline->leftPolygonID = -1;
			polyline->rightAttribute = polygon->objectAttribute;
			//polygon->commoncircles[circleID].push_back(polyline);
			circle->removePoints(ori, end, polyline->PosinCommonLayer);//可以在remove中进行修改，不用insert（-1，-1，可以把链的信息加进去）
		}
	}

}

void CGeoLayer::EndPointGetValue()
{
	int size = this->objects.size();
	for (int i = 0; i < size; i++) {
		CGeoPolygon* polygon = (CGeoPolygon*)(objects[i]);
		EndPointGetValue(polygon);
	}
}

void CGeoLayer::EndPointGetValue(CGeoPolygon * polygon)
{
	int size = polygon->circles.size();
	for (int i = 0; i < size; i++) {
		EndPointGetValue(polygon->circles[i]);
	}
}

void CGeoLayer::EndPointGetValue(CGeoPolyline * polyline)
{
	int size = polyline->pts.size();
	polyline->pts[size - 1] = polyline->pts[0];
}

void CGeoLayer::simplify(double telorance)//对面积有影响
{
	int size = commonEdge.size();
	for (int i = 0; i < size; i++) {
		if(!commonEdge[i]->isNULL)
			if (i == 26) {
				int aaa = 1;
			}
			commonEdge[i]->DouglasPeuckerDataCompress(telorance);
			int size1 = commonEdge[i]->pts.size();
			
			for (int m = 0; m < size1 - 1; m++) {
				CPoint pt1 = commonEdge[i]->pts[m]->GetCPoint();
				CPoint pt2 = commonEdge[i]->pts[m+1]->GetCPoint();

				if (pt1 == pt2) {
					int ssss = 1;
				}
				
			}
	}
}

void CGeoLayer::PolylineDeleted(vector<double> areaParameter)
{
	CString str;
	int count = 0;
	int size1 = objects.size();
	for (int i = 0; i < size1; i++) {
		if (!objects[i]->isNULL) {
			CGeoPolygon* polygon = (CGeoPolygon*)objects[i];
			if (PolygonCanBeDelete(polygon,areaParameter)) {
				count++;
				int size2 = polygon->commonEdge.size();
				for (int i = 0; i < size2; i++) {
					CGeoPolyline* line = polygon->commonEdge[i];
					if (line->isNULL) {
						continue;
					}
					else {
						line->canDeleted = true;
						PolylineDeleted(line,polygon->PosinCommonLayer);
						break;
					}
				}
			}
		}	
	}
	str.Format("%d条面要删除", count);//前后之差即程序运行时间     
	AfxMessageBox(str);
}

// 相同属性面合并
void CGeoLayer::PolylineDeleted()
{
	CString str;
	int count = 0;
	int size = commonEdge.size();
	for (int i = 0; i < size; i++) {
		if (!commonEdge[i]->isNULL) {
			
			bool a = commonEdge[i]->JudgeCanDeleted();
			if (a) {
				count++;
				PolylineDeleted(commonEdge[i], -1);
			}
		}
	}
	str.Format("%d组相邻面属性一致", count);//前后之差即程序运行时间     
	AfxMessageBox(str);
}

void CGeoLayer::PolylineDeleted(CGeoPolyline * polyline, int num)//是对环的内部有影响，改变了环的组成
{
	bool a = polyline->canDeleted;
	
	if (a) {//如果该链可以被删除的话
		
		int rightPolygonID = polyline->rightPolygonID;
		int leftPolygonID = polyline->leftPolygonID;
		if (rightPolygonID == -1 || leftPolygonID == -1) {
			polyline->canDeleted = false;
			return;
		}
		CGeoPolygon* rightPolygon = (CGeoPolygon*)(objects[rightPolygonID]);
		CGeoPolygon* leftPolygon = (CGeoPolygon*)(objects[leftPolygonID]);
		//int size = polyline->pts.size();
		//CMyPoint* pt1 = polyline->pts[0];
		//CMyPoint* pt2 = polyline->pts[size-1];
		//if (pt1->GetCPoint()==pt2->GetCPoint()) {//如果该链首尾闭合，即该链组成了一个面，也就是一个岛
		//	PolylineDelete(polyline, false);	
		//}
		//else {//说明是两个面的公共边，删除该链，同时合并两面到ID号较小的面上
		//	PolylineDelete(polyline, true);		
		//}
		polyline->isNULL = true;
		if (num == -1) {
			MergeTwoPolygon(leftPolygon, rightPolygon);
		}
		else {
			MergeTwoPolygon(leftPolygon, rightPolygon, num);//出现了左右面一致的情况
		}	
	}
	int s = 1;
}

void CGeoLayer::PolylineDelete(CGeoPolyline * line, bool isRemainEnd)//还没写链数组中删除操作
{
	int end = line->pts.size() - 1;
	if (!isRemainEnd) {//首末两点删除
		
		line->pts[0]->isNULL = true;
		line->pts[end]->isNULL = true;
	}
	
	for (int i = 1; i < line->pts.size() - 1; i++) {
		line->pts[i]->isNULL = true;
	}
	line->isNULL = true;
}

void CGeoLayer::MergeTwoPolygon(CGeoPolygon* leftPolygon, CGeoPolygon* rightPolygon)
{ 
	//CGeoPolyline inValidLine;//
	int rightSize = rightPolygon->commonEdge.size();
	int RightPolygonID = rightPolygon->PosinCommonLayer;
	int LeftPolygonID = leftPolygon->PosinCommonLayer;
	for (int i = 0; i < rightSize; i++) {
		CGeoPolyline* polyline = rightPolygon->commonEdge[i];		
		if (!polyline->isNULL) {
			//需要更改其左右值
			if (polyline->leftPolygonID == RightPolygonID) {
				polyline->leftPolygonID = LeftPolygonID;
			}
			else {
				polyline->rightPolygonID = LeftPolygonID;
			}
			if (polyline->rightPolygonID == polyline->leftPolygonID){
				

				polyline->isNULL = true;
				//inValidLine = *polyline;
			}
			else
				leftPolygon->commonEdge.push_back(polyline);
		}
	}
	//int  s = 1;
	//for (int i = 0; i < leftPolygon->commonEdge.size(); i++) {
	//	if (leftPolygon->commonEdge[i]->isNULL)//只有在左右属性一致以及面积过小的情况下isNULL = true，还有后期的链合并的时候会产生isNULL
	//		leftPolygon->mergeCommonEdge(leftPolygon->commonEdge[i]);
	//}
	//将rightpolygon的commonCircle加到leftpolygon的commonCircle
	/*for (int i = 0; i < rightPolygon->commoncircles.size(); i++) {
		int count = 0;
		for (int j = 0; j < rightPolygon->commoncircles[i].size(); j++) {
			if (!rightPolygon->commoncircles[i][j]->isNULL)
				count++;
		}
		if (count > 0) {
			leftPolygon->commoncircles.push_back(rightPolygon->commoncircles[i]);
		}
	}*/
	leftPolygon->shapeArea = leftPolygon->shapeArea + rightPolygon->shapeArea;
	rightPolygon->isNULL = true;
	CRect rect1 = leftPolygon->objectMABR;
	CRect rect2 = rightPolygon->objectMABR;
	leftPolygon->objectMABR->left=rect1.left<rect2.left?rect1.left:rect2.left;
	leftPolygon->objectMABR->right=rect1.right>rect2.right?rect1.right:rect2.right;
	leftPolygon->objectMABR->bottom=rect1.bottom<rect2.bottom?rect1.bottom:rect2.bottom;
	leftPolygon->objectMABR->top=rect1.top>rect2.top?rect1.top:rect2.top;
	//下面合并在公共链删除后可以合并的链
	//CPoint ptOri =  inValidLine.pts[0]->GetCPoint();
	//CPoint ptEnd =  inValidLine.pts[inValidLine.pts.size()-1]->GetCPoint();
	//vector<CGeoPolyline*> oriPtLine;
	//vector<CGeoPolyline*> endPtLine;
	//for (int i = 0;i<leftPolygon->commonEdge.size();i++)
	//{
	//	CGeoPolyline* tempLine = leftPolygon->commonEdge[i];
	//	CPoint tempOript = tempLine->pts[0]->GetCPoint();
	//	CPoint tempEndpt = tempLine->pts[tempLine->pts.size()-1]->GetCPoint();
	//	if (tempOript==ptOri||tempEndpt==ptOri)
	//	{
	//		oriPtLine.push_back(tempLine);
	//	}
	//	if (tempOript==ptEnd||tempEndpt==ptEnd)
	//	{
	//		endPtLine.push_back(tempLine);
	//	}
	//}
	//if (oriPtLine.size()!=2||endPtLine.size()!=2)
	//{
	//	AfxMessageBox("不能合并啊");
	//}else{
	//	if (leftPolygon->judgeLineMerge(oriPtLine[0],oriPtLine[1]))
	//	{
	//		int style = leftPolygon->styleLineMerge(oriPtLine[0],oriPtLine[1]);
	//		leftPolygon->mergeCommonEdge(oriPtLine[0],oriPtLine[1],style);
	//	}
	//	if (leftPolygon->judgeLineMerge(endPtLine[0],endPtLine[1]))
	//	{
	//		int style = leftPolygon->styleLineMerge(endPtLine[0],endPtLine[1]);
	//		leftPolygon->mergeCommonEdge(endPtLine[0],endPtLine[1],style);
	//	}
	//}
	//leftPolygon->rebuildCircle();
	int sss=1;
}

void CGeoLayer::MergeTwoPolygon(CGeoPolygon * leftPolygon, CGeoPolygon * rightPolygon,int num)
{
	CGeoPolyline inValidLine;
	int rightSize = rightPolygon->commonEdge.size();
	int leftSize = leftPolygon->commonEdge.size();
	int RightPolygonID = rightPolygon->PosinCommonLayer;
	int LeftPolygonID = leftPolygon->PosinCommonLayer;
	if (num != RightPolygonID&&num != LeftPolygonID) {
		AfxMessageBox("合并面是什么鬼");
	}
	CRect rect1 = leftPolygon->objectMABR;
	CRect rect2 = rightPolygon->objectMABR;
	if (num == RightPolygonID) {
		for (int i = 0; i < rightSize; i++) {
			CGeoPolyline* polyline = rightPolygon->commonEdge[i];
			if (!polyline->isNULL) {
				//需要更改其左右值
				if (polyline->leftPolygonID == RightPolygonID) {
					polyline->leftPolygonID = LeftPolygonID;
					polyline->leftAttribute = leftPolygon->objectAttribute;
				}
				else {
					polyline->rightPolygonID = LeftPolygonID;
					polyline->rightAttribute = leftPolygon->objectAttribute;;
				}
				if (polyline->rightPolygonID == polyline->leftPolygonID){
					polyline->isNULL = true;
					inValidLine = *polyline;
				}
				else
					leftPolygon->commonEdge.push_back(polyline);
			}
		}
		leftPolygon->objectMABR->left=rect1.left<rect2.left?rect1.left:rect2.left;
		leftPolygon->objectMABR->right=rect1.right>rect2.right?rect1.right:rect2.right;
		leftPolygon->objectMABR->bottom=rect1.bottom<rect2.bottom?rect1.bottom:rect2.bottom;
		leftPolygon->objectMABR->top=rect1.top>rect2.top?rect1.top:rect2.top;
		leftPolygon->shapeArea = leftPolygon->shapeArea + rightPolygon->shapeArea;
		rightPolygon->isNULL = true;
		//下面合并在公共链删除后可以合并的链
		CPoint ptOri =  inValidLine.pts[0]->GetCPoint();
		CPoint ptEnd =  inValidLine.pts[inValidLine.pts.size()-1]->GetCPoint();
		vector<CGeoPolyline*> oriPtLine;
		vector<CGeoPolyline*> endPtLine;
		for (int i = 0;i<leftPolygon->commonEdge.size();i++)
		{
			CGeoPolyline* tempLine = leftPolygon->commonEdge[i];
			CPoint tempOript = tempLine->pts[0]->GetCPoint();
			CPoint tempEndpt = tempLine->pts[tempLine->pts.size()-1]->GetCPoint();
			if (tempOript==ptOri||tempEndpt==ptOri)
			{
				oriPtLine.push_back(tempLine);
			}
			if (tempOript==ptEnd||tempEndpt==ptEnd)
			{
				endPtLine.push_back(tempLine);
			}
		}
		if (oriPtLine.size()!=2||endPtLine.size()!=2)
		{
			AfxMessageBox("不能合并啊");
		}else{
			if (leftPolygon->judgeLineMerge(oriPtLine[0],oriPtLine[1]))
			{
				int style = leftPolygon->styleLineMerge(oriPtLine[0],oriPtLine[1]);
				leftPolygon->mergeCommonEdge(oriPtLine[0],oriPtLine[1],style);
			}
			if (leftPolygon->judgeLineMerge(endPtLine[0],endPtLine[1]))
			{
				int style = leftPolygon->styleLineMerge(endPtLine[0],endPtLine[1]);
				leftPolygon->mergeCommonEdge(endPtLine[0],endPtLine[1],style);
			}
		}
		leftPolygon->rebuildCircle();
	}
	else {
		for (int i = 0; i < leftSize; i++) {
			CGeoPolyline* polyline = leftPolygon->commonEdge[i];
			if (!polyline->isNULL) {
				//需要更改其左右值
				if (polyline->leftPolygonID == LeftPolygonID) {
					polyline->leftPolygonID = RightPolygonID;
					polyline->leftAttribute =rightPolygon->objectAttribute;
				}
				else {
					polyline->rightPolygonID = RightPolygonID;
					polyline->rightAttribute = rightPolygon->objectAttribute;;
				}
				if (polyline->rightPolygonID == polyline->leftPolygonID){
					polyline->isNULL = true;
					inValidLine = *polyline;
				}
				else
					rightPolygon->commonEdge.push_back(polyline);
			}
		}
		rightPolygon->objectMABR->left=rect1.left<rect2.left?rect1.left:rect2.left;
		rightPolygon->objectMABR->right=rect1.right>rect2.right?rect1.right:rect2.right;
		rightPolygon->objectMABR->bottom=rect1.bottom<rect2.bottom?rect1.bottom:rect2.bottom;
		rightPolygon->objectMABR->top=rect1.top>rect2.top?rect1.top:rect2.top;
		rightPolygon->shapeArea = leftPolygon->shapeArea + rightPolygon->shapeArea;
		leftPolygon->isNULL = true;
		//下面合并在公共链删除后可以合并的链
		CPoint ptOri =  inValidLine.pts[0]->GetCPoint();
		CPoint ptEnd =  inValidLine.pts[inValidLine.pts.size()-1]->GetCPoint();
		vector<CGeoPolyline*> oriPtLine;
		vector<CGeoPolyline*> endPtLine;
		for (int i = 0;i<rightPolygon->commonEdge.size();i++)
		{
			CGeoPolyline* tempLine = rightPolygon->commonEdge[i];
			CPoint tempOript = tempLine->pts[0]->GetCPoint();
			CPoint tempEndpt = tempLine->pts[tempLine->pts.size()-1]->GetCPoint();
			if (tempOript==ptOri||tempEndpt==ptOri)
			{
				oriPtLine.push_back(tempLine);
			}
			if (tempOript==ptEnd||tempEndpt==ptEnd)
			{
				endPtLine.push_back(tempLine);
			}
		}
		if (oriPtLine.size()!=2||endPtLine.size()!=2)
		{
			AfxMessageBox("不能合并啊");
		}else{
			if (rightPolygon->judgeLineMerge(oriPtLine[0],oriPtLine[1]))
			{
				int style = rightPolygon->styleLineMerge(oriPtLine[0],oriPtLine[1]);
				rightPolygon->mergeCommonEdge(oriPtLine[0],oriPtLine[1],style);
			}
			if (rightPolygon->judgeLineMerge(endPtLine[0],endPtLine[1]))
			{
				int style = rightPolygon->styleLineMerge(endPtLine[0],endPtLine[1]);
				rightPolygon->mergeCommonEdge(endPtLine[0],endPtLine[1],style);
			}
		}
		rightPolygon->rebuildCircle();
	}
	
}

void CGeoLayer::deleteCircle()
{
	int size = objects.size();
	for (int i = 0; i < size; i++) {
		CGeoPolygon* polygon = (CGeoPolygon*)(objects[i]);
		polygon->deleteCircle();
	}
}

bool CGeoLayer::PolygonCanBeDelete(CGeoPolygon * polygon,vector<double> areaParameter)
{ 
	bool temp = false;
	int count = 0;
	int i = 0;
	int size = areaParameter.size();
	if (i > size - 1) goto loop;
	if (polygon->objectAttribute =="0500"&&polygon->shapeArea < areaParameter[i])
		count++;
	i++;
	if (i > size - 1) goto loop;
	if (polygon->objectAttribute =="0210"&&polygon->shapeArea < areaParameter[i])
		count++;
	i++;
	if (i > size - 1) goto loop;
	if (polygon->objectAttribute =="0290"&&polygon->shapeArea < areaParameter[i])
		count++;
	i++;
	if (i > size - 1) goto loop;
	if (polygon->objectAttribute == "0713"&&polygon->shapeArea <areaParameter[i])
		count++;
	i++;
	loop:
	if (count > 0)
		temp = true;
	return temp;
}

void CGeoLayer::ReoganizeCommonCircle()
{
	if (objects.size() == 0)
	{
		AfxMessageBox("出问题了");
	}
	else {
		for (int i = 0; i <  objects.size();i++) {
			CGeoPolygon* polygon = (CGeoPolygon*)(objects[i]);
			if (!polygon->isNULL) {
			
				polygon->ReoganizeCommonCircle();
			
			}			
		}
	}
}

//pt1ID与pt2ID分别代表着在objectTemp中的下标
LinePointPosition CGeoLayer::findCommonPointInLine(int polygon1, int circle1ID, int pt1ID, int polygon2, int circle2ID, int pt2ID) {
	//通过object数组来判断，但是在objecttemp中操作
	int x = pt1ID;
	int y = pt2ID;
	LinePointPosition linePointPos;
	linePointPos.line1Ori = x;
	linePointPos.line2End = y;
	
	CGeoPolygon* polygonObjectTemp1 = (CGeoPolygon*)(objectsTemp[polygon1]);
	CGeoPolygon* polygonObjectTemp2 = (CGeoPolygon*)(objectsTemp[polygon2]);
	CGeoPolygon* polygonObject1 = (CGeoPolygon*)(objects[polygon1]);
	CGeoPolygon* polygonObject2 = (CGeoPolygon*)(objects[polygon2]);
	int pt1Num = polygonObjectTemp1->circles[circle1ID]->GetSize();//要考虑要不要加减一
	int pt2Num = polygonObjectTemp2->circles[circle2ID]->GetSize();
	CGeoPolyline* polyline = new CGeoPolyline();
	
	if (pt1ID == 0) {
		CMyPoint* myPoint1 = polygonObjectTemp1->circles[circle1ID]->GetPointAt(0);
		CMyPoint* myPoint2 = polygonObjectTemp1->circles[circle1ID]->GetPointAt(1);
		CMyPoint* myPoint3 = polygonObjectTemp1->circles[circle1ID]->GetPointAt(pt1Num-2);
		
		int point1 = myPoint1->Getx();
		int point2 = myPoint2->Getx();
		int point3 = myPoint3->Getx();
			
		if (point1 != -1&&point2 != -1&&point3 !=-1) {
			int flag = 0;
			int m = pt2ID + 1;
			if (m >pt2Num - 2) {
				m = 0;
			}
			for (int i = pt1Num - 2; i >= 0; i--) {				
				CMyPoint* myPoint5 = polygonObjectTemp2->circles[circle2ID]->GetPointAt(m);
				CMyPoint* myPoint4 = polygonObjectTemp1->circles[circle1ID]->GetPointAt(i);
				int point5 = myPoint5->Getx();
				int point4 = myPoint4->Getx();
				if (point4 == -1 || point5 == -1) 	break;
				if (myPoint4->GetCPoint() == myPoint5->GetCPoint()) {
					int ptPos1 = myPoint4->ptID;//pt在object数组中环中的位置
					int ptPos2 = myPoint5->ptID;
					if (!polygonObject1->circles[circle1ID]->GetPointAt(ptPos1)->isCommon) {//若未加入数组
							CMyPoint* myPoint =polygonObject1->circles[circle1ID]->GetPointAt(ptPos1);
							this->commonPoint.push_back(myPoint);
							myPoint->isCommon = true;
							myPoint->PosInCommonPoint = commonPoint.size() - 1;						
							polygonObject2->circles[circle2ID]->setPoint(ptPos2, myPoint);
						}
					else {
							int temp = polygonObject1->circles[circle1ID]->GetPointAt(ptPos1)->PosInCommonPoint;
							if (!polygonObject2->circles[circle2ID]->GetPointAt(ptPos2)->isCommon) {
								polygonObject2->circles[circle2ID]->setPoint(ptPos2, commonPoint[temp]);
							}
						}

					polyline->pts.push_back(polygonObject1->circles[circle1ID]->GetPointAt(ptPos1));
					linePointPos.line1Ori = i;
					linePointPos.line2End = m;
				}
				else
					break;
				m++;
				if (m >pt2Num - 2) {//????
					m = 0;
				}
				if (i == 0) flag = 1;
			}
			reverse(polyline->pts.begin(), polyline->pts.end());//反转
			if (flag == 1) {//说明是一个岛，只与一个面相邻,但是另一个面不一定是只有一条链，可能只是环中的一条链与岛相邻
				linePointPos.line1End = pt1Num - 1;
				linePointPos.line2Ori = y;
				polyline->pts.push_back(polygonObject1->circles[circle1ID]->GetPointAt(0));
				goto loop;
			}
		}
	}
	for (int i = x; i < pt1Num; i++) {//w尾点与首点未统一
		CMyPoint* myPoint1 = polygonObjectTemp1->circles[circle1ID]->GetPointAt(i);
		CMyPoint* myPoint2 = polygonObjectTemp2->circles[circle2ID]->GetPointAt(y);
		int point1 = myPoint1->Getx();
		int point2 = myPoint2->Getx();
		if (point1 == -1|| point2 == -1) 	break;			
		if (myPoint1->GetCPoint()== myPoint2 ->GetCPoint()) {//该面i环m点与polygon面j环n点相同,同时将该点（已经位于公共点的点）加入公共链中
				//一条原理：不存在一个未加入公共点数组的点，而与之坐标相等的相邻面的点已经加入公共点数组,一条链一定有左右两个面，但是两个面不一定只有1条公共边
			int ptPos1 = myPoint1->ptID;//pt在object数组中环中的位置
			int ptPos2 = myPoint2->ptID;
			CMyPoint* myPoint = polygonObject1->circles[circle1ID]->GetPointAt(ptPos1);
			if (!myPoint->isCommon) {//若未加入数组,iscommon是用来判断点的拓扑关系是否构建
				
				this->commonPoint.push_back(myPoint);
				myPoint->isCommon = true;
				myPoint->PosInCommonPoint = commonPoint.size() - 1;
				polygonObject2->circles[circle2ID]->setPoint(ptPos2, myPoint);
			}
			else {
				int temp = myPoint->PosInCommonPoint;
				if (!polygonObject2->circles[circle2ID]->GetPointAt(ptPos2)->isCommon) {
					polygonObject2->circles[circle2ID]->setPoint(ptPos2, commonPoint[temp]);
				}
			}
				
			polyline->pts.push_back(myPoint);
			linePointPos.line1End = i;
			linePointPos.line2Ori = y;
		}
		else 
			break;
		y--;
		if (y < 0) {
			y = pt2Num - 2;
		}
	}
	loop:
	if (polyline->GetSize() == 1) {
		linePointPos.line1Ori = -1;
		return linePointPos;
	}
	else {//好像这样circle，环的顺序不用调整
		int size1 = polyline->pts.size();
		this->commonEdge.push_back(polyline);
		polygonObject1->commonEdge.push_back(polyline);
		//polygonObject1->commoncircles[circle1ID].push_back(polyline);
		polygonObject2->commonEdge.push_back(polyline);
		//polygonObject2->commoncircles[circle2ID].push_back(polyline);
		polyline->PosinCommonLayer = this->commonEdge.size() - 1;
		polyline->leftPolygonID = polygon2;
		polyline->leftAttribute = polygonObject2->objectAttribute;
		polyline->rightPolygonID = polygon1;
		polyline->rightAttribute = polygonObject1->objectAttribute;
		polyline->pts[0]->isJointPt = true;
		polyline->pts[polyline->pts.size()-1]->isJointPt = true;
		linePointPos.posInCommonLayer = polyline->PosinCommonLayer;
		return linePointPos;
	}
}

void CGeoLayer::BuildMABR()
{
	int length = objectsTemp.size();
	if(length>0){
		for (i = 0; i < length; i++) {
			CRect* rect = objectsTemp[i]->GetObjectMABR();
			objects[i]->objectMABR = rect;
			objectMABRs.push_back(rect);
		}
	}else{
		for (i = 0; i < objects.size(); i++) {
			CRect* rect = objects[i]->GetObjectMABR();
			objects[i]->objectMABR = rect;
			objectMABRs.push_back(rect);
		}
	}
}

void CGeoLayer::GetObjectIntersectMABR()//在同一图层内处理
{
	int size = objectsTemp.size();
	if (size>0)
	{
		for (i = 0; i < size - 1; i++) {
			for (unsigned int j = i + 1; j < size; j++) {
				BOOL a = objectsTemp[i]->isObjectMABRintersect(objectsTemp[j]);
				if (a) {
					objectsTemp[i]->intersectMABRpos.push_back(j);
					objectsTemp[j]->intersectMABRpos.push_back(i);
				}
			}
		}
	}else{
		int size1 = objects.size();
		for (i = 0; i < size - 1; i++) {
			for (unsigned int j = i + 1; j < size; j++) {
				BOOL a = objects[i]->isObjectMABRintersect(objects[j]);
				if (a) {
					objects[i]->intersectMABRpos.push_back(j);
					objects[j]->intersectMABRpos.push_back(i);
				}
			}
		}
	}
}

void CGeoLayer::setRect(CRect rect)
{
	this->rectLWC = rect;
}

CRect CGeoLayer::getRect()
{
	return this->rectLWC;
}

// 对拓扑图层更新范围
CRect CGeoLayer::updateTopoRect()
{
	CRect newRcet;
	double nLeft,nRight,nTop,nBottom;
	CGeoPolygon* polygon0 = (CGeoPolygon*)objects[0];
	polygon0->getObjectTopoRect();
	CRect oriRcet = polygon0->objectTopoRect;
	nLeft = oriRcet.left;
	nRight = oriRcet.right;
	nTop = oriRcet.top;
	nBottom = oriRcet.bottom;

	int size = objects.size();
	for (int i = 0; i < size; i++) {
		CGeoPolygon* polygon = (CGeoPolygon*)objects[i];
		if (!polygon->isNULL) {
			polygon->getObjectTopoRect();
			CRect topoRcet = polygon->objectTopoRect;
			if (nLeft >topoRcet.left )
				nLeft = topoRcet.left;
			if (nRight <topoRcet.right)
				nRight = topoRcet.right;
			if (nTop > topoRcet.top)
				nTop = topoRcet.top;
			if (nBottom < topoRcet.bottom)
				nBottom = topoRcet.bottom;
		}
	}
	newRcet.left = nLeft;
	newRcet.right = nRight;
	newRcet.top = nTop;
	newRcet.bottom = nBottom;
	return newRcet;
}


// 得到objects中每一个object的面积
void CGeoLayer::getObjectArea()
{
	int size = objects.size();
	for (int i = 0; i < size; i++) {
		CGeoPolygon* polygon = (CGeoPolygon*)objects[i];
		if (!polygon->isNULL) {
			polygon->getArea();
		}
	}
}

////构建commonCircle容量
//void CGeoLayer::rebuildCircle()
//{
//	int size = objects.size();
//	for (int i = 0; i < size; i++) {
//		CGeoPolygon* polygon = (CGeoPolygon*)(objects[i]);
//		polygon->rebuildCircle();
//	}
//}


// 对TOPO数据中的误差内部环进行删除，同时产生了面积，周长等属性
void CGeoLayer::delErrorCircle()
{
	int size = objects.size();
	for (int i = 0; i < size; i++) {
		CGeoPolygon* polygon = (CGeoPolygon*)objects[i];
		if (!polygon->isNULL) {
			polygon->delErrorCircle();
			int size1 = polygon->circlePoints.size();
			for (int j = 0; j < size1; j++) {
				vector<CMyPoint*> pts = polygon->circlePoints[j];
				int size2 = pts.size();
				for (int m = 0; m < size2 - 1; m++) {
					if (pts[m]->GetCPoint() == pts[m + 1]->GetCPoint()) {
						int ssss = 1;
					}
				}
			}
		}
		
	}
}


// 对拓扑数据进行更新，出现在线化简与面化简的情况下
void CGeoLayer::updateTopoLayer()
{
	int jjj = 0;
	int size = objects.size();
	for (int i = 0; i < size; i++) {
		CGeoPolygon* polygon = (CGeoPolygon*)objects[i];
		if (!polygon->isNULL) {
			polygon->getCirclePoint();
			int size1 = polygon->circlePoints.size();
			for (int j = 0; j < size1; j++) {
				vector<CMyPoint*> pts = polygon->circlePoints[j];
				int size2 = pts.size();
				for (int m = 0; m < size2 - 1; m++) {
					if (pts[m]->GetCPoint() == pts[m + 1]->GetCPoint()) {
						int ssss = 1;
					}
				}
			}
			polygon->getArea();
			polygon->getTopoPointNum();
			polygon->getCircleLength();
			polygon->delErrorCircle();
		}
	}
	jjj = 1;
}




// 得到在点在图层中所在面的ID值，若都不在，返回-1
int CGeoLayer::getSelectPolygonID(CPoint pt)
{
	int selectID = -1;
	int length = objects.size();
	int count = 0;
	for(int i = 0;i<length;i++){
		if(!objects[i]->isNULL){
			CGeoPolygon* polygon = (CGeoPolygon*)objects[i];
			CMyPoint myPt(pt);
			if(myPt.isInRect(polygon->objectMABR)){			
				if(polygon->isPtInNew(pt)){
					selectID = i;
					break;
				}
					
			}
		}
	}
	if(count>1)
		AfxMessageBox("点同时属于两个面？？？");
	return selectID;
}


// 建筑物化简




void CGeoLayer::buildSimple(double angle, double area, double length)
{	
	int size1 = objects.size();
	for (int i = 0; i < size1; i++) {
		if (!objects[i]->isNULL) {
			CString leftTwoAtt = objects[i]->objectAttribute.Left(2);
			if (leftTwoAtt=="05")
			{
				CGeoBuilding* buildPolygon = (CGeoBuilding*)objects[i];
				buildPolygon->buildPolygonSimple(angle,area,length);
			}
		}	
	}
}


// 构建每个面上基于拓扑结构的公共环
void CGeoLayer::buildCircle(void)
{
	int size1 = objects.size();
	for (int i = 0; i < size1; i++) {
		int sss= 0;
		if (i ==250)
			sss=1;
		if (i ==251)
			sss=1;
		if (i ==252)
			sss=1;
		if (i ==253)
			sss=1;
		if (!objects[i]->isNULL) {
			CGeoPolygon* polygon = (CGeoPolygon*)objects[i];
			polygon->rebuildCircle();		
		}	
	}
}


// 得到道路中心线
void CGeoLayer::getRoadCentreLine(void)
{
	int size1 = objects.size();
	for (int i = 0; i < size1; i++) {
		if (!objects[i]->isNULL) {
			CString leftTwoAtt = objects[i]->objectAttribute.Left(2);
			if (leftTwoAtt=="06")
			{
				CGeoRoad* roadPolygon = (CGeoRoad*)objects[i];
				roadPolygon->generateRoadCentreLine();
			}
		}	
	}
}


// zf,0623,按点数
void CGeoLayer::ObjectFilter(void)
{
	for(int i = 0;i<objects.size();i++)
	{
		objects[i]->ObjectFilter();
		//circlePointList.push_back(objects[i]->getPointNum());//获取点数
	}
	//getNewArray_Point();  //计算用
}


// zf，按面积
void CGeoLayer::ObjectFilterByArea(void)
{
	for(int i = 0;i<objects.size();i++)
	{
		objects[i]->ObjectFilterByArea();
		//circleAreaList.push_back(objects[i]->getCircleArea());
	}
	//getNewArray_Area();  //计算用
}

// //二维数组转化成一维数组,点,zf,0701
void CGeoLayer::getNewArray_Point(void)
{
	if(zf_point.size() != 0)
		vector<int>().swap(zf_point);
	for(int i = 0;i<circlePointList.size();i++)
	{
		for(int j = 0;j<circlePointList[i].size();j++)
		{
			zf_point.push_back(circlePointList[i][j]);
		}
	}
	if(circlePointList.size() != 0)
		vector<vector<int>>().swap(circlePointList);
}

// 二维数组转化为一维数组，面积,zf,0701
void CGeoLayer::getNewArray_Area(void)
{
	for(int i = 0;i<circleAreaList.size();i++)
	{
		for(int j = 0;j<circleAreaList[i].size();j++)
		{
			zf_area.push_back(circleAreaList[i][j]);
		}
	}
	circleAreaList.clear();
}

// 寻找一般的外接矩形，zf，0704
void CGeoLayer::FindRectangle(void)
{
	for(int i = 0;i<objects.size();i++)
	{
		objects[i]->FindRectangle();
	}
}


// zf,0708,最小获取外接矩形
void CGeoLayer::FindMABR(void)
{
	for(int i = 0;i<objects.size();i++)
	{
		objects[i]->FindMABR();
	}
}


// 最小外接矩形优化，0712，zf
void CGeoLayer::optimize(void)
{
	bool hasgetMBAR;
	for(int i = 0;i<objects.size();i++)
	{
		hasgetMBAR = objects[i]->optimize();
		if(!hasgetMBAR) break;
	}
	if(!hasgetMBAR) AfxMessageBox("要先求得最小外接矩形！！！");
	//else AfxMessageBox("优化成功");
}

// 去除冗余点，zf，0717
void CGeoLayer::RemoveRedundant(void)
{
	for(int i = 0;i<objects.size();i++)
	{
		objects[i]->RemoveRedundant();
	}
}


// 1003 zf
void CGeoLayer::DeleteTail(void)
{
	for(int i = 0;i<objects.size();i++)
	{
		objects[i]->DeleteTail();
	}
}
