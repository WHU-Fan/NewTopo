
// NewTopoView.cpp : CNewTopoView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "NewTopo.h"
#endif

#include "NewTopoDoc.h"
#include "NewTopoView.h"
#include"GeoPolygon.h"
#include "MainFrm.h"
#include <algorithm>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "MyDialog.h"
#include"PolygonSimplifyDialog.h"
// CNewTopoView

IMPLEMENT_DYNCREATE(CNewTopoView, CView)

BEGIN_MESSAGE_MAP(CNewTopoView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_OPEN, &CNewTopoView::OnFileOpen)
	ON_COMMAND(ID_ZOOMIN, &CNewTopoView::OnZoomin)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_UPDATE_COMMAND_UI(ID_ZOOMIN, &CNewTopoView::OnUpdateZoomin)
	ON_COMMAND(ID_WHOLE, &CNewTopoView::OnWhole)
	ON_COMMAND(ID_TRANSLATION, &CNewTopoView::OnTranslation)
	ON_UPDATE_COMMAND_UI(ID_TRANSLATION, &CNewTopoView::OnUpdateTranslation)
	ON_COMMAND(ID_TOPOBUILD, &CNewTopoView::OnTopobuild)
	
	ON_COMMAND(ID_SIMPLIFY, &CNewTopoView::OnSimplify)
	ON_COMMAND(ID_POLYGONSIMPLIFY, &CNewTopoView::OnPolygonsimplify)
	ON_COMMAND(ID_ADDORIMAP, &CNewTopoView::OnAddorimap)
	ON_COMMAND(ID_TOPOSAVE, &CNewTopoView::OnToposave)
//	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	
	ON_COMMAND(ID_SELECTPOLYGONID, &CNewTopoView::OnSelectpolygonid)
	ON_COMMAND(ID_FINDATTRIBUTE, &CNewTopoView::OnFindattribute)
	ON_COMMAND(ID_MERGEGONSAREA, &CNewTopoView::OnMergegonsarea)
	ON_COMMAND(ID_MERGEPOLYGONSAMEATT, &CNewTopoView::OnMergepolygonsameatt)
	ON_WM_SIZE()
	ON_COMMAND(ID_POLYGON_SELECT, &CNewTopoView::OnPolygonSelect)
	ON_COMMAND(ID_ADDDATA, &CNewTopoView::OnAdddata)
	ON_COMMAND(ID_PART_TOPO, &CNewTopoView::OnPartTopo)
//	ON_COMMAND(ID_BUILDSINPLE, &CNewTopoView::OnBuildSimple)
ON_COMMAND(ID_BUILDSIMPLE, &CNewTopoView::OnBuildsimple)
ON_COMMAND(ID_REFINEROAD, &CNewTopoView::OnRefineroad)
ON_COMMAND(ID_ADJOINPOLYGONQUERY, &CNewTopoView::OnAdjoinpolygonquery)
ON_COMMAND(ID_FILTER, &CNewTopoView::OnFilter)
ON_COMMAND(ID_FILTER_byAREA, &CNewTopoView::OnFilterByArea)
ON_COMMAND(ID_FindRectangle, &CNewTopoView::OnFindRectangle)
ON_COMMAND(ID_MABR, &CNewTopoView::OnMABR)
ON_COMMAND(ID_OPTIMIZE, &CNewTopoView::OnOptimize)
ON_COMMAND(ID_RemoveRedundant, &CNewTopoView::OnRemoveredundant)
ON_COMMAND(ID_final, &CNewTopoView::OnFinal)
ON_COMMAND(ID_deleteTail, &CNewTopoView::OnDeletetail)
END_MESSAGE_MAP()

// CNewTopoView 构造/析构

CNewTopoView::CNewTopoView()//by wmj
	: IsSizeChanged(false)
{
	// TODO: 在此处添加构造代码
	isMouseDown = FALSE;
	currrentOperateStatus = 0;
	map = NULL;
	isTopo = false;
	dcMem = NULL;
	selectID = -1;
	MAPSCALE = 1000;
}


CNewTopoView::~CNewTopoView()
{
	if (map != NULL)
		delete map;
}
// 将十进制转换成十六进制，并能被程序读取
int CNewTopoView::OnChangeByteOrderTenToSixteen(int indata)
{
	int yushu;
	int i = 7;
	char ss[8];
	if (indata == 0) return 0;
	while (indata > 0) {
		yushu = indata % 16;
		//a[i] = yushu;
		char k;
		if (yushu > 9)
			k = 'a' + yushu - 10;
		else
			k = '0' + yushu;
		ss[i] = k;
		indata = indata / 16;
		i--;
	}
	////****** 进行倒序  
	for (int j = 0 ; j<i+1; j++) {
		ss[j] = '0';
	}
	int t,temp;
	t = ss[0]; ss[0] = ss[6]; ss[6] = t;
	t = ss[1]; ss[1] = ss[7]; ss[7] = t;
	t = ss[2]; ss[2] = ss[4]; ss[4] = t;
	t = ss[3]; ss[3] = ss[5]; ss[5] = t;
	for (i = 0; i < 8; i++) {
		if (ss[i] !='0') {
			temp = i;
			break;
		}	
	}
	int k;
	int num = 0;
	for (i = temp; i < 8; i++) {
		if (ss[i] >= 'a'&&ss[i] <= 'f')
			k = 10 + ss[i] - 'a';
		else
			k = ss[i] - '0';
		num = num * 16 + k;
	}
	return num;
}
//进制转换，十六进制转换十进制，属于shp文件读取时用到的函数
int CNewTopoView::OnChangeByteOrder(int indata)
{
	char ss[9];
	char ee[8];
	unsigned long val = unsigned long(indata);
	ultoa(val, ss, 16);// 将十六进制的数(val)转到一个字符串(ss)中,itoa(val,ss,16);  
	int i;
	int length = strlen(ss);
	if (length != 8)
	{
		for (i = 0; i<8 - length; i++)
			ee[i] = '0';
		for (i = 0; i<length; i++)
			ee[i + 8 - length] = ss[i];
		for (i = 0; i<8; i++)
			ss[i] = ee[i];
	}
	////****** 进行倒序  
	int t;
	t = ss[0]; ss[0] = ss[6]; ss[6] = t;
	t = ss[1]; ss[1] = ss[7]; ss[7] = t;
	t = ss[2]; ss[2] = ss[4]; ss[4] = t;
	t = ss[3]; ss[3] = ss[5]; ss[5] = t;
	////******  
	//****** 将存有十六进制数 (val) 的字符串 (ss) 中的十六进制数转成十进制数  
	int value = 0;
	for (i = 0; i<8; i++)
	{
		int k;
		if (ss[i] == 'a' || ss[i] == 'b' || ss[i] == 'c' || ss[i] == 'd' || ss[i] == 'e' || ss[i] == 'f')
			k = 10 + ss[i] - 'a';
		else
			k = ss[i] - '0';
		//printf("k=%d\n",k);  
		value = value + int(k*(int)pow((DOUBLE)16, 7 - i));
	}
	//printf("value=%d\n",value);  
	return(value);
}

void CNewTopoView::readData(CString& filename,int& layerIndex)//目前完成图层layer中每个面状polygon的点数组已填充，子环数组已填充
{
	//----------------------------------------------------------------  
	// 打开坐标文件 
	CFileDialog fDLG(true);
	if (fDLG.DoModal() != IDOK) {
		return;
	}
	 filename = fDLG.GetPathName();
	//filename = "G:\\翁老师拓扑项目\\数据\\实验数据\\小型数据\\试试.shp";
	
	FILE * m_ShpFile_fp = fopen(filename, "rb");//wmj
	if (m_ShpFile_fp == NULL)
	{
		MessageBox("Open File Failed");
		exit(0);
	}
	
	//加判断，如果打开新图，delete map
	if(this->currrentOperateStatus==0){
		if(map!=NULL)
			map=NULL;
		map = new CGeoMap();
	}
	CGeoLayer* layer = new CGeoLayer();//wmj
	
	//否则，直接新建图层	
		
	//----------------------------------------------------------------  
	// 读取坐标文件头的内容开始  
	int fileCode = -1;
	int fileLength = -1;
	int version = -1;
	int shapeType = -1;

	int i;
	int FileCode;
	int Unused;
	int FileLength;
	int Version;
	int ShapeType;
	double Xmin;
	double Ymin;
	double Xmax;
	double Ymax;
	double Zmin;
	double Zmax;
	double Mmin;
	double Mmax;

	fread(&FileCode, sizeof(int), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Filecode里面去，每次读一个int型字节的长度，读取一次  
	FileCode = OnChangeByteOrder(FileCode);//将读取的FileCode的值转化为十进制的数  

	for (i = 0; i<5; i++)
		fread(&Unused, sizeof(int), 1, m_ShpFile_fp);

	fread(&FileLength, sizeof(int), 1, m_ShpFile_fp);//读取FileLength  
	FileLength = OnChangeByteOrder(FileLength);//将FileLength转化为十进制的数  

	fread(&Version, sizeof(int), 1, m_ShpFile_fp);//读取Version的值  

	fread(&ShapeType, sizeof(int), 1, m_ShpFile_fp);//读取ShapeType的值  

	fread(&Xmin, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Xmin里面去，每次读取一个double型字节长度，读取一次  

	fread(&Ymin, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Ymin里面去，每次读取一个double型字节长度，读取一次  

	fread(&Xmax, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Xmax里面去，每次读取一个double型字节长度，读取一次  

	fread(&Ymax, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Ymax里面去，每次读取一个double型字节长度，读取一次  

	float dx = Xmax-Xmin;
	float dy = Ymax-Ymin;

	float xx = log(dx);
	float yy = log(dy);
	float xy = xx>yy?xx:yy;

	MAPSCALE = exp(double(10-(int)xy));

	CRect rect(Xmin*MAPSCALE, Ymin*MAPSCALE, Xmax*MAPSCALE, Ymax*MAPSCALE);//wmj
    layer->setRect(rect);
	

	fread(&Zmin, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Zmin里面去，每次读取一个double型字节长度，读取一次  

	fread(&Zmax, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Zmax里面去，每次读取一个double型字节长度，读取一次  

	fread(&Mmin, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Mmin里面去，每次读取一个double型字节长度，读取一次  

	fread(&Mmax, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Mmax里面去，每次读取一个double型字节长度，读取一次  

							   // 读取坐标文件头的内容结束  

							   //读取面状目标的实体信息
	int RecordNumber;
	int ContentLength;
	//int countNum = 0;

	switch (ShapeType)
	{
	case 5://polygon
	{
		
		while ((fread(&RecordNumber, sizeof(int), 1, m_ShpFile_fp) != 0))//从图层第一个面开始循环
		{
			fread(&ContentLength, sizeof(int), 1, m_ShpFile_fp);
			RecordNumber = OnChangeByteOrder(RecordNumber);//RecordNumber代表了图层面的序号，1,2,3,4...
			if (RecordNumber==252)
			{
				int ssssss =0;
			}
			ContentLength = OnChangeByteOrder(ContentLength);
			int shapeType;
			double Box[4];
			int NumParts;
			int NumPoints;
			int *Parts;
			fread(&shapeType, sizeof(int), 1, m_ShpFile_fp);
			//读Box
			for (i = 0; i < 4; i++)
				fread(Box + i, sizeof(double), 1, m_ShpFile_fp);
			//读NumParts和NumPoints
			fread(&NumParts, sizeof(int), 1, m_ShpFile_fp);//表示构成当前面状目标的子环的个数
		
			fread(&NumPoints, sizeof(int), 1, m_ShpFile_fp);//表示构成当前面状目标所包含的坐标点个数
		
			//读Parts和Points
			Parts = new int[NumParts];//记录了每个子环的坐标在Points数组中的起始位置
			for (i = 0; i < NumParts; i++)
				fread(Parts + i, sizeof(int), 1, m_ShpFile_fp);
			int pointNum;
			
			CGeoPolygon* polygon;
			polygon = new CGeoPolygon();
			polygon->circleNum = NumParts;
			CPoint tempPoint(0, 0);
			for (i = 0; i < NumParts; i++)//面中子环的循环//内岛是逆时针，外环是顺时针
			{
				if (i != NumParts - 1)
					pointNum = Parts[i + 1] - Parts[i];//每个子环的长度 
				else
					pointNum = NumPoints - Parts[i];
				
				printf("pointNum每个子环长度=%d\n", pointNum);
				double *PointsX;
				double *PointsY;
				CGeoPolyline* polyline = new CGeoPolyline();
				polyline->circleID = i;
				PointsX = new double[pointNum];//用于存放读取的点坐标x值
				PointsY = new double[pointNum];//用于存放y值

				for (int j = 0; j < pointNum; j++)
				{
					fread(PointsX + j, sizeof(double), 1, m_ShpFile_fp);
					fread(PointsY + j, sizeof(double), 1, m_ShpFile_fp);
					double a= PointsX[j];
					double b= PointsY[j];
					double a1 = double(a*MAPSCALE / 10) * 10;
					double b1 = double(b*MAPSCALE / 10) * 10;
					CMyPoint * point1 = new CMyPoint(CPoint(a1,b1));//wmj这个point1最后需要释放么，同时这个point1有必要new一下么

					if (point1->GetCPoint()!= tempPoint) {
						polyline->AddPoint(point1);//把该子环所有的点添加到一条链上
						point1->circleID = i;
						point1->ptID = j;
						tempPoint = point1->GetCPoint();
					}					
				}
				CPoint pt1 = polyline->pts[0]->GetCPoint();
				CPoint pt2 = polyline->pts[polyline->pts.size()-1]->GetCPoint();
				if(pt1!=pt2){
					CString str;
					str.Format("%d数据首尾点不一致",RecordNumber);
					//AfxMessageBox(str);
					CMyPoint * pointend = new CMyPoint(pt1);
					polyline->pts.push_back(pointend);
				}
					 polygon->AddCircle(polyline); //将polyline链添加到对应的子环数组<circles>中
					 for (int mm = 0; mm < polyline->pts.size()-1; mm++) {
						 CMyPoint* pt1 = polyline->pts[mm];
						 CMyPoint* pt2 = polyline->pts[mm+1];
						 if (pt1->GetCPoint() == pt2->GetCPoint()) {
							   AfxMessageBox("读进来的点有相邻两点一致");
						  }
					  }
				

				delete[] PointsX;
				delete[] PointsY;
			}//一个面的子环循环结束，同时该面的点已加入到polygon，子环数组<circles>已填充
			//layer->objects.push_back((CGeoObject*)polygon);
			layer->AddObject((CGeoObject*)polygon,&(layer->objects));//将该polygon加入到objects中
			polygon->PosinCommonLayer = layer->objects.size()-1;
			int circleSize = polygon->circles.size();
			for (int i = 0;i<circleSize;i++)
			{
				polygon->circles[i]->polygonID = polygon->PosinCommonLayer;
			}
			if (RecordNumber == 429) {
				int iii = 111;
			}
			if (RecordNumber == 430) {
				int iii = 333;
			}
			if (RecordNumber == 431) {
				int iii = 555;
			}
			if (RecordNumber == 432) {
				int iii = 777;
			}
			delete[] Parts;
		}
		layer->BuildMABR();
		map->AddLayer(layer);	
		layerIndex = map->getLayerSize()-1;
	}
	
	if(layerIndex==0){
		double a = rect.left;
		double b = rect.right;
		double c = rect.top;
		double d = rect.bottom;
		map->setMapRect(rect);
		
	}
	rectWC = map->GetMapRect();
	oldRect = map->GetMapRect();
	
	break;
	case 1://point
		break;
	case 3://polyline
		break;
	default:
		break;
	}
}

void CNewTopoView::WriteShp(CString& filename)
{
	CFileDialog fDLG(false);
	if (fDLG.DoModal() != IDOK) {
		return;
	}
	filename = fDLG.GetPathName();

	FILE * m_ShpFile_fp = fopen(filename, "wb");//wmj
	if (m_ShpFile_fp  == NULL) {
		return;
	}
	int i;
	int FileCode = 9994;
	int Unused;
	int FileLength=1;
	int Version = 1001;
	int ShapeType = 5;
	double Xmin = map->TopoRect.left / MAPSCALE;//要除以MAPSCALE
	double Ymin = map->TopoRect.top/MAPSCALE;
	double Xmax=map->TopoRect.right / MAPSCALE;
	double Ymax = map->TopoRect.bottom / MAPSCALE;
	double Zmin=0;
	double Zmax=0;
	double Mmin=0;
	double Mmax=0;
	FileCode = OnChangeByteOrderTenToSixteen(FileCode);
	fwrite(&FileCode, sizeof(int), 1, m_ShpFile_fp);
	//要转成16进制
	for (i = 0; i < 5; i++)
		fwrite(&Unused, sizeof(int), 1, m_ShpFile_fp);
	FileLength = OnChangeByteOrderTenToSixteen(FileLength);
	fwrite(&FileLength, sizeof(int), 1, m_ShpFile_fp);
	fwrite(&Version, sizeof(int), 1, m_ShpFile_fp);
	fwrite(&ShapeType, sizeof(int), 1, m_ShpFile_fp);
	fwrite(&Xmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Ymin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Xmax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Ymax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Zmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Zmax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Mmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Mmax, sizeof(double), 1, m_ShpFile_fp);
	//写文件头结束

	int RecordNumber = map->getTopoLayer()->objects.size();
	for (int i = 1; i <= RecordNumber; i++) {
		CGeoPolygon* polygon =(CGeoPolygon* )map->getTopoLayer()->objects[i - 1];
		int temp = i;
		temp = OnChangeByteOrderTenToSixteen(temp);
		fwrite(&temp, sizeof(int), 1, m_ShpFile_fp);
		int ContentLength = 0;
		ContentLength = OnChangeByteOrderTenToSixteen(ContentLength);
		fwrite(&ContentLength, sizeof(int), 1, m_ShpFile_fp);
		int shapeType = 5;
		fwrite(&shapeType, sizeof(int), 1, m_ShpFile_fp);
		CRect objectRect =polygon->objectTopoRect;
		double Box[4] = {objectRect.left,objectRect.right,objectRect.top,objectRect.bottom };
		for (int j = 0; j < 4; j++) {
			fwrite(Box+j, sizeof(double), 1, m_ShpFile_fp);
		}
		int NumParts = polygon->circleNum;//子环个数
		fwrite(&NumParts, sizeof(int), 1, m_ShpFile_fp);
		int NumPoints = polygon->TopoPointNum;//表示构成当前面状目标所包含的坐标点个数
		fwrite(&NumPoints, sizeof(int), 1, m_ShpFile_fp);
		int *Parts;
		Parts = new int[NumParts];//记录了每个子环的坐标在Points数组中的起始位置
		*(Parts) = 0;
		int temp1 = 0;
		for (int j = 0; j < NumParts-1; j++) {
			temp1 = temp1+ polygon->circlePointNum[j];
			*(Parts+j+1) =temp1;
		}
		for (int j = 0; j < NumParts; j++) {
			int sss = *(Parts + j);
			fwrite(Parts + j, sizeof(int), 1, m_ShpFile_fp);
		}
		for (int j = 0; j < NumParts; j++) {
			vector<CMyPoint*> pts = polygon->circlePoints[j];
			int size = pts.size();
			for (int n = 0; n < size - 1; n++) {
				if (pts[n] == pts[n + 1]) {
					int lllll = 1;
				}
			}
			for (int m = 0; m < size; m++) {
				double x = (double)pts[m]->Getx()/MAPSCALE;
				double y = (double)pts[m]->Gety()/ MAPSCALE;
				fwrite(&x, sizeof(double), 1, m_ShpFile_fp);
				fwrite(&y, sizeof(double), 1, m_ShpFile_fp);
			}
		}
	}
	fclose(m_ShpFile_fp);
}

void CNewTopoView::DBFintoSHP(vector<CString> cc, vector<double> shapeArea,int layerIndex)
{
	CGeoLayer* layer = map->getLayerAt(layerIndex);
	int size = layer->objects.size();
	for (int i = 0; i < size; i++) {
		CGeoPolygon* polygon = dynamic_cast<CGeoPolygon*>(layer->objects[i]);
		polygon->objectAttribute = cc[i];
		polygon->shapeArea = shapeArea[i];
	}
	int a = 1;
}

BOOL CNewTopoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

void CNewTopoView::OnPrepareDC(CDC * pDC, CPrintInfo * pInfo)
{
	if(map == NULL)//wmj
		return;//当isOpen是true时，执行；isOpen是false时，返回

	pDC->SetMapMode(MM_ANISOTROPIC);
	CRect rectDC;
	SIZE size;
	CPoint pt;
	this->GetClientRect(&rectDC);
	pt = rectDC.CenterPoint();
	size.cx = rectDC.Width();
	size.cy = rectDC.Height();
	pDC->SetViewportExt(size);
	pDC->SetViewportOrg(pt);

	pt = rectWC.CenterPoint();
	float scaleX, scaleY,scale;//wmj

	scaleX = rectWC.Width() / rectDC.Width();
	scaleY = rectWC.Height() / rectDC.Height();
	scale = scaleX > scaleY ? scaleX : scaleY;
	map->mapScale = scale;
	char p[20];
	CStatusBar *pStatus=(CStatusBar *)AfxGetMainWnd()->GetDescendantWindow(AFX_IDW_STATUS_BAR);

	sprintf(&p[0],"x=  %d ",map->mapScale);
	pStatus->SetPaneText(pStatus->CommandToIndex(ID_SEPARATOR),&p[0]);
	size.cx = rectDC.Width() * scale*1.02;
	size.cy = -rectDC.Height() *scale*1.02; 
	pDC->SetWindowExt(size);
	pDC->SetWindowOrg(pt);

	CView::OnPrepareDC(pDC, pInfo);
}

// CNewTopoView 绘制

void CNewTopoView::OnDraw(CDC* pDC)
{
	CNewTopoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	//if (!pDoc)
	//	return;

	CRect rect;

	GetClientRect(&rect);                                                 //用于缓冲作图的内存DC

	                                               //内存中承载临时图象的位图
	if (dcMem == NULL) {
		dcMem = new CDC();
		dcMem->CreateCompatibleDC(pDC);//依附窗口DC创建兼容内存DC
		bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());//创建兼容位图
		dcMem->SelectObject(&bmp);                          //将位图选择进内存DC
	}
	if (IsSizeChanged) {
		bmp.DeleteObject();
		bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		dcMem->SelectObject(&bmp);
		IsSizeChanged = false;
	}
	pDC->DPtoLP(&rect);
	OnPrepareDC(dcMem);
	dcMem->FillSolidRect(rect, RGB(255, 255, 255));
	//dcMem->FillSolidRect(rect, pDC->GetBkColor());//按原来背景填充客户区，不然会是黑色

	if (map != NULL)
		map->Draw(dcMem,dcCFG);
	pDC->FillSolidRect(rect, RGB(255, 255, 255));
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), dcMem, rect.left, rect.top, SRCCOPY);;//将内存DC上的图象拷贝到前台


	
	// TODO: 在此处为本机数据添加绘制代码
}


// CNewTopoView 打印

BOOL CNewTopoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CNewTopoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CNewTopoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CNewTopoView 诊断

#ifdef _DEBUG
void CNewTopoView::AssertValid() const
{
	CView::AssertValid();
}

void CNewTopoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}


CNewTopoDoc* CNewTopoView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNewTopoDoc)));
	return (CNewTopoDoc*)m_pDocument;
}
#endif //_DEBUG


// CNewTopoView 消息处理程序


void CNewTopoView::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码
	this->currrentOperateStatus = 0;
	
	CString filename = "";
	int layerIndex = 0;
	this->readData(filename,layerIndex);
	this->OnReadDbf(filename,layerIndex);

	this->Invalidate();
}


void CNewTopoView::OnZoomin()//wmj
{
	// TODO: 在此添加命令处理程序代码
	this->currrentOperateStatus = ZOOMIN;

}

void CNewTopoView::OnUpdateZoomin(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(this->currrentOperateStatus == ZOOMIN);
}


void CNewTopoView::OnLButtonDown(UINT nFlags, CPoint point)//wmj
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (map == NULL)
		 return;
	if (this->currrentOperateStatus == ZOOMIN) 
	{
		pt1 = pt2 = point;
		
		isMouseDown = true;
	}
	if (this->currrentOperateStatus == TRANSLATE) {
		pt1 = pt2 = point;
		isMouseDown = true;
	}
	
	
	CView::OnLButtonDown(nFlags, point);
}


void CNewTopoView::OnLButtonUp(UINT nFlags, CPoint point)//wmj
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	//pt2 = point;
	if (this->currrentOperateStatus == ZOOMIN) 
	{
		isMouseDown = false;
		
		CDC* pDC = this->GetDC();
		OnPrepareDC(pDC);
		pDC->DPtoLP(&pt1);
		pDC->DPtoLP(&pt2);
		rect.left = pt1.x > pt2.x ? pt2.x : pt1.x;
		rect.right = pt1.x < pt2.x ? pt2.x : pt1.x;
		rect.top = pt1.y > pt2.y ? pt2.y : pt1.y;
		rect.bottom = pt1.y< pt2.y ? pt2.y : pt1.y;
		
		//pDC->DPtoLP(&rect);	
		if (abs(rect.Width()) < MAPSCALE || abs(rect.Height() )< MAPSCALE)
		{
			rect.left = rectWC.left + rectWC.Width()/4;
			rect.top = rectWC.top + rectWC.Height() / 4;
			rect.right = rectWC.right - rectWC.Width() / 4;
			rect.bottom = rectWC.bottom - rectWC.Height() / 4;
		}
		rectWC = rect;		

		Invalidate();
	}

	if (this->currrentOperateStatus == TRANSLATE) {
		isMouseDown = false;
		if (pt1 == pt2) return;
		CPoint p2(pt2), p1(pt1);
		CDC *pDC = GetDC();
		OnPrepareDC(pDC);
		pDC->DPtoLP(&p2);
		pDC->DPtoLP(&p1);
		CPoint pt = p2 - p1;
		rectWC.OffsetRect(-pt);
		Invalidate();
		
	}

	if(this->currrentOperateStatus==POLYGONSELECT){
		CDC* pDC = this->GetDC();
		OnPrepareDC(pDC);
		CPoint pt = point;
		pDC->DPtoLP(&pt);
		this->MouseSelectPolygon(pt);
		Invalidate();
	}

	if (this->currrentOperateStatus==ADJOINPOLYGONQUERY)
	{
		CDC* pDC = this->GetDC();
		OnPrepareDC(pDC);
		CPoint pt = point;
		pDC->DPtoLP(&pt);
		this->MouseSelectPolygon(pt);
		Invalidate();
	}

	CView::OnLButtonUp(nFlags, point);
}


void CNewTopoView::OnMouseMove(UINT nFlags, CPoint point)//wmj
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!isMouseDown) return;

	CDC* pDC = this->GetDC();
	if (this->currrentOperateStatus == ZOOMIN) 
	{
		
		pDC->SetROP2(R2_NOTXORPEN);
		pDC->Rectangle(pt1.x, pt1.y, pt2.x, pt2.y);
		pt2 = point;
		pDC->Rectangle(pt1.x, pt1.y, pt2.x, pt2.y);
	}
	else if (this->currrentOperateStatus == TRANSLATE&&isMouseDown) {
		CRect rect;
		GetClientRect(&rect);
		int width = rect.Width();
		int height = rect.Height();
		CDC *pDC = GetDC();
		OnPrepareDC(pDC);
		CPoint p2(pt2), p1(pt1);
		pDC->DPtoLP(&p2);
		pDC->DPtoLP(&p1);
		CPoint pt = p2 - p1;

		pDC->DPtoLP(&rect);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),dcMem, rect.left - pt.x, rect.top - pt.y, SRCCOPY);

		if (pt.x>0)
		{//填左侧区域
			CRect rect1(0, 0, pt2.x - pt1.x, height);
			pDC->DPtoLP(&rect1);
			pDC->FillSolidRect(rect1, pDC->GetBkColor());
		}
		else
		{//填右侧区域
			CRect rect1(width + (pt2.x - pt1.x), 0, width, height);
			pDC->DPtoLP(&rect1);
			pDC->FillSolidRect(rect1, pDC->GetBkColor());
		}
		if (pt.y>0)
		{//填上侧区域
			CRect rect1(0, height + (pt2.y - pt1.y), width, height);
			pDC->DPtoLP(&rect1);
			pDC->FillSolidRect(rect1, pDC->GetBkColor());
		}
		else
		{//填下侧区域			
			CRect rect1(0, 0, width, pt2.y - pt1.y);
			pDC->DPtoLP(&rect1);
			pDC->FillSolidRect(rect1, pDC->GetBkColor());
		}
		pt2 = point;
		//Invalidate();
	}

	
	this->OnPrepareDC(pDC);
	//pDC->DPtoLP(&point);
	CString szCoordinate;  
	//获得状态栏的指针  
	//CStatusBar* pStatusBar=(CStatusBar*)GetParentFrame()->  
		GetDescendantWindow(ID_VIEW_STATUS_BAR);  
	//pDC->DPtoLP(&point);
	 
	szCoordinate.Format("(%8d,%8d)",point.x,point.y);  
	//在状态栏的第二个窗格中输出当前鼠标位置  
	((CMainFrame*)GetParent())->SetMessageText(szCoordinate);  
	
	ReleaseDC(pDC);
	

	CView::OnMouseMove(nFlags, point);
}


void CNewTopoView::OnWhole()//wmj
{
	// TODO: 在此添加命令处理程序代码
	this->currrentOperateStatus = ONWHOLE;
	rectWC = map->getMapRect();
	Invalidate();
}


void CNewTopoView::OnTranslation()
{
	// TODO: 在此添加命令处理程序代码
	this->currrentOperateStatus = TRANSLATE;
}


void CNewTopoView::OnUpdateTranslation(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(this->currrentOperateStatus == TRANSLATE);
}


void CNewTopoView::OnTopobuild()
{
	// TODO: 在此添加命令处理程序代码
	
	//map->getTopoLayer()->isTopLayer = true;
	if (map == NULL) {
		CString str = "没有数据，请输入数据";
		AfxMessageBox(str);
		return;
	}
	long t1 = GetTickCount();//程序段开始前取得系统运行时间(ms)            
	
	this->currrentOperateStatus = TOPOBUILD;
	if (!dcCFG.isShowTopo) {
		dcCFG.isShowTopo = true;
	}
	//dcCFG.isShowTopo = !dcCFG.isShowTopo;
	if (dcCFG.isShowTopo) {//如何重新建立拓扑
		if (!isTopo) {
			map->MapTopo();
			isTopo = true; 
			map->isTopoed = true;
		}		
	}	
	/*int size = map->getLayerSize();
	for (int i = 0; i < size; i++) {
		map->getLayerAt(i)->PolylineDeleted();
	}*/
	long t2 = GetTickCount();//程序段结束后取得系统运行时间(ms)    
	CString str;
	str.Format("拓扑构建time:%dms", t2 - t1);//前后之差即程序运行时间        

	AfxMessageBox(str);
	Invalidate();
}

void CNewTopoView::OnSimplify()
{
	bool onePolygon = false;
	if(this->currrentOperateStatus==SELECT||this->currrentOperateStatus==POLYGONSELECT)
		onePolygon=true;
	CString str;
	CMyDialog myDlg;
	double d = 0;
	if (myDlg.DoModal() == IDOK)
	{
		d = myDlg.m_lineTelorance;
	}
	bool flag = true;

	if (map != NULL) {
		if (map->isTopoed) {
			if (map->getTopoLayer()->commonEdge.size() != 0) {
				flag = false;
			}
		}
	}
	
	if (flag) {
		str = "未构建拓扑，请先构建拓扑";
		AfxMessageBox(str);
		return;
	}
	else {
		long t1 = GetTickCount();//程序段开始前取得系统运行时间(ms) 
		d = d*MAPSCALE;
		if(onePolygon&&selectID!=-1){
			CGeoLayer* layer = map->getTopoLayer();
			if(selectID>layer->objects.size()){
				AfxMessageBox("select超出范围！");
			}
			CGeoPolygon* polygon = (CGeoPolygon*)layer->objects[selectID];
			int length = polygon->commonEdge.size();
			for(int i =0;i<length;i++){
				CGeoPolyline* line = polygon->commonEdge[i];
				if(!line->isNULL){
					line->DouglasPeuckerDataCompress(d);
				}
			}
		}else{
			map->simplify(d);
			map->getTopoLayer()->IsLineSimply =1;//每次执行线化简则赋值为1，如果再执行面化简则判断：if(Islinesimply==1)Islinesimply==2;
			long t2 = GetTickCount();//程序段结束后取得系统运行时间(ms)        
			str.Format("线压缩time:%dms", t2 - t1);//前后之差即程序运行时间     
			AfxMessageBox(str);
		}
	}
	
	Invalidate();
	// TODO: 在此添加命令处理程序代码
}


void CNewTopoView::OnPolygonsimplify()
{
	
	// TODO: 在此添加命令处理程序代码
}


void CNewTopoView::OnAddorimap()
{
	dcCFG.isShowOriMap = !dcCFG.isShowOriMap;
	Invalidate();
	// TODO: 在此添加命令处理程序代码
}


void CNewTopoView::OnToposave()
{
	dcCFG.isShowExport = !dcCFG.isShowExport;
	bool flag = true;
	CString str;
	if (map != NULL) {
		if (map->isTopoed) {
			if (map->getTopoLayer()->commonEdge.size() != 0) {//如果建立拓扑且有值，则flag=false;
				flag = false;
			}
		}
	}
	if (flag) {
		str = "未构建拓扑，请先构建拓扑";
		AfxMessageBox(str);
		return;
	}

	if(map->getTopoLayer()->IsLineSimply==1){//最后进行的是链化简就要重新计算面积等
		if (!map->getTopoLayer()->isPolygonSimply) {//只经过链化简，而没有经过面化简
			map->getTopoLayer()->ReoganizeCommonCircle();
			map->getTopoLayer()->delErrorCircle();
			map->getTopoLayer()->isPolygonSimply = true;
		}
		else {//同时经过线化简及面化简，就要commonCircle中的不知道会不会变，然后重置其他数组
			map->getTopoLayer()->updateTopoLayer();
		}
	}
	else {
		if (!map->getTopoLayer()->isPolygonSimply) {
			map->getTopoLayer()->ReoganizeCommonCircle();
			map->getTopoLayer()->delErrorCircle();
			map->getTopoLayer()->isPolygonSimply = true;
		}
	}
	map->getTopoRect();//重新计算边界
	
	CString filename = "";
	WriteShp(filename);
	WriteDbf(filename);
	Invalidate();
	// TODO: 在此添加命令处理程序代码
}


void CNewTopoView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	if (this->currrentOperateStatus == ZOOMIN||this->currrentOperateStatus==TRANSLATE)
	{
		rect.left = rectWC.left - rectWC.Width() / 2;
		rect.top = rectWC.top - rectWC.Height() / 2;
		rect.right = rectWC.right + rectWC.Width() / 2;
		rect.bottom = rectWC.bottom + rectWC.Height() / 2;
		if (rect.Width() > oldRect.Width()*1.5)
		{
			rect.left = oldRect.left - oldRect.Width() / 7;
			rect.top = oldRect.top - oldRect.Height() /7;
			rect.right = oldRect.right + oldRect.Width() /7;
			rect.bottom = oldRect.bottom + oldRect.Height() / 7;
		}
		rectWC = rect;

		Invalidate();
	}
	CView::OnRButtonDblClk(nFlags, point);
}



BOOL CNewTopoView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}
void CNewTopoView::OnReadDbf(CString filename,int layerIndex)//CString DbfFileName)
{
	//CString DbfFileName = "F:\\拓扑关系\\实验数据\\共边2\\共边.dbf";
	int n = filename.ReverseFind('.');
	filename = filename.Left(n);
	filename = filename + ".dbf";
	FILE* m_DbfFile_fp;//****Dbf文件指针
					   //打开dbf文件
	if ((m_DbfFile_fp = fopen(filename, "rb")) == NULL)
	{
		return;
	}
	int i, j;
	//////****读取dbf文件的文件头  开始
	BYTE version;//3'\x3'
	fread(&version, 1, 1, m_DbfFile_fp);

	BYTE date[3];
	for (i = 0; i<3; i++)
	{
		fread(date + i, 1, 1, m_DbfFile_fp);
	}

	int RecordNum;//文件中的记录条数
	fread(&RecordNum, sizeof(int), 1, m_DbfFile_fp);
	short HeaderByteNum;//文件头中的字节数
	fread(&HeaderByteNum, sizeof(short), 1, m_DbfFile_fp);
	short RecordByteNum;//一条记录中的字节长度
	fread(&RecordByteNum, sizeof(short), 1, m_DbfFile_fp);
	short Reserved1;
	fread(&Reserved1, sizeof(short), 1, m_DbfFile_fp);
	BYTE Flag4s;//0'\0'
	fread(&Flag4s, sizeof(BYTE), 1, m_DbfFile_fp);
	BYTE EncrypteFlag;//0'\0'
	fread(&EncrypteFlag, sizeof(BYTE), 1, m_DbfFile_fp);
	int Unused[3];//16-27
	for (i = 0; i<3; i++)
	{
		fread(Unused + i, sizeof(int), 1, m_DbfFile_fp);
	}
	int a = Unused[0];
	int b = Unused[1];
	int c = Unused[2];
	BYTE MDXFlag;//28
	fread(&MDXFlag, sizeof(BYTE), 1, m_DbfFile_fp);

	BYTE LDriID;
	fread(&LDriID, sizeof(BYTE), 1, m_DbfFile_fp);
	short Reserved2;
	fread(&Reserved2, sizeof(short), 1, m_DbfFile_fp);
	BYTE name[11];
	BYTE fieldType;
	int Reserved3;
	BYTE fieldLength;
	BYTE decimalCount;
	short Reserved4;
	BYTE workID;
	short Reserved5[5];
	BYTE mDXFlag1;
	int fieldscount;
	fieldscount = (HeaderByteNum - 32) / 32;
	int s = 1;
	//读取记录项信息－共有8个记录项
	for (i = 0; i< fieldscount; i++)
	{
		RecordItem recordItem;
		//FieldName----11   bytes
		fread(name, 11, 1, m_DbfFile_fp);
		memcpy(recordItem.name, name, 11);
		//FieldType----1     bytes
		fread(&fieldType, sizeof(BYTE), 1, m_DbfFile_fp);
		recordItem.fieldType = fieldType;
		//Reserved3----4     bytes
		Reserved3 = 0;
		fread(&Reserved3, sizeof(int), 1, m_DbfFile_fp);
		recordItem.Reserved3 = Reserved3;
		//FieldLength--1     bytes
		fread(&fieldLength, sizeof(BYTE), 1, m_DbfFile_fp);
		recordItem.fieldLength = fieldLength;
		int s = fieldLength;
		int d = s;
		//DecimalCount-1   bytes
		fread(&decimalCount, sizeof(BYTE), 1, m_DbfFile_fp);
		recordItem.decimalCount = decimalCount;
		//Reserved4----2     bytes
		Reserved4 = 0;
		fread(&Reserved4, sizeof(short), 1, m_DbfFile_fp);
		recordItem.Reserved4 = Reserved4;
		//WorkID-------1    bytes
		fread(&workID, sizeof(BYTE), 1, m_DbfFile_fp);
		recordItem.workID = workID;
		//Reserved5----10   bytes
		for (j = 0; j<5; j++)
		{
			fread(Reserved5 + j, sizeof(short), 1, m_DbfFile_fp);
		}
		memcpy(recordItem.Reserved5, Reserved5, 5);
		//MDXFlag1-----1  bytes
		fread(&mDXFlag1, sizeof(BYTE), 1, m_DbfFile_fp);
		recordItem.mDXFlag1 = mDXFlag1;
		recordItems.push_back(recordItem);
	}
	BYTE terminator;
	fread(&terminator, sizeof(BYTE), 1, m_DbfFile_fp);
	//读取dbf文件头结束

	CString Soil_code, suit;
	int ObjectID, Ecrm, Elevt;
	double shapeArea, shapeLength;
	CString Dest, Ec, cc;
	BYTE deleteFlag;
	char media[40];
	vector<CString> polygonAttribute;
	vector<double> ShapeArea;
	//读取dbf文件记录  开始
	for (i = 0; i<RecordNum; i++)
	{
		fread(&deleteFlag, sizeof(BYTE), 1, m_DbfFile_fp); //读取删除标记  1字节
														   //读取 ObjectID int
		for (j = 0; j<40; j++)
			strcpy(media + j, "\0");
		for (j = 0; j<10; j++)
			fread(media + j, sizeof(char), 1, m_DbfFile_fp);   //--10
		ObjectID = atoi(media);//6个空，4个数

		//读取 Dest string
		for (j = 0; j<40; j++)
			strcpy(media + j, "\0");
		for (j = 0; j<32; j++)
			fread(media + j, sizeof(char), 1, m_DbfFile_fp);   //--32
		Dest = media;//第一个'/'其余均为‘’

		//读取 Ec string
		for (j = 0; j<40; j++)
			strcpy(media + j, "\0");
		for (j = 0; j<16; j++)
			fread(media + j, sizeof(char), 1, m_DbfFile_fp);  //--16
		Ec = media;//同上
		//读取 EcRm int
		for (j = 0; j<40; j++)
			strcpy(media + j, "\0");
		for (j = 0; j<10; j++)
			fread(media + j, sizeof(char), 1, m_DbfFile_fp);   //--10
		Ecrm = atoi(media);//5个''，5个值
		//读取 Elevt int
		for (j = 0; j<40; j++)
			strcpy(media + j, "\0");
		for (j = 0; j<10; j++)
			fread(media + j, sizeof(char), 1, m_DbfFile_fp);   //--10
		Elevt = atoi(media);
		//读取 Cc int
		for (j = 0; j<40; j++)
			strcpy(media + j, "\0");
		for (j = 0; j<8; j++)
			fread(media + j, sizeof(char), 1, m_DbfFile_fp);   //--8
		cc = media;//4值4''
		cc = cc.Left(4);
		//读取 shape_length double
		for (j = 0; j<40; j++)
			strcpy(media + j, "\0");
		for (j = 0; j<19; j++)
			fread(media + j, sizeof(char), 1, m_DbfFile_fp);   //--19
		shapeLength = atof(media);//带e的
		//读取 shape_Area double

		for (j = 0; j<40; j++)
			strcpy(media + j, "\0");
		for (j = 0; j<19; j++)
			fread(media + j, sizeof(char), 1, m_DbfFile_fp);   //--19
		shapeArea = atof(media);
		int s = 1;

		polygonAttribute.push_back(cc);
		ShapeArea.push_back(shapeArea);
	}
	//读取dbf文件记录  结束
	DBFintoSHP(polygonAttribute, ShapeArea,layerIndex);
	int ffff = 1;
}

// 完成对dbf的输出
void CNewTopoView::WriteDbf(CString filename)
{
	int n = filename.ReverseFind('.');
	filename = filename.Left(n);
	filename = filename + ".dbf";
	FILE* m_DbfFile_fp;//****Dbf文件指针
					   //打开dbf文件
	if ((m_DbfFile_fp = fopen(filename, "wb")) == NULL)
	{
		return;
	}
	int i, j;
	//////****读取dbf文件的文件头  开始
	BYTE version = 4;
	fwrite(&version, 1, 1, m_DbfFile_fp);
	CTime t = CTime::GetCurrentTime();
	int d = t.GetDay() ;
	int y = t.GetYear() % 2000;
	int m = t.GetMonth();
	BYTE date[3];
	date[0] = y;//17'\x11'
	date[1] = m;//7'\a'
	date[2] = d;
	for (i = 0; i<3; i++)
	{
		fwrite(date + i, 1, 1, m_DbfFile_fp);
	}

	int RecordNum = map->getTopoLayer()->objects.size();//文件中的记录条数
	fwrite(&RecordNum, sizeof(int), 1, m_DbfFile_fp);
	short HeaderByteNum = 289;//文件头中的字节数
	fwrite(&HeaderByteNum, sizeof(short), 1, m_DbfFile_fp);
	short RecordByteNum=125;//一条记录中的字节长度
	fwrite(&RecordByteNum, sizeof(short), 1, m_DbfFile_fp);
	short Reserved1=0;
	fwrite(&Reserved1, sizeof(short), 1, m_DbfFile_fp);
	BYTE Flag4s=0;
	fwrite(&Flag4s, sizeof(BYTE), 1, m_DbfFile_fp);
	BYTE EncrypteFlag=0;
	fwrite(&EncrypteFlag, sizeof(BYTE), 1, m_DbfFile_fp);
	int Unused[3] = { 0,0,0 };//16-27
	for (i = 0; i<3; i++)
	{
		fwrite(Unused + i, sizeof(int), 1, m_DbfFile_fp);
	}
	BYTE MDXFlag=0;//28
	fwrite(&MDXFlag, sizeof(BYTE), 1, m_DbfFile_fp);

	BYTE LDriID=0;
	fwrite(&LDriID, sizeof(BYTE), 1, m_DbfFile_fp);
	short Reserved2=0;
	fwrite(&Reserved2, sizeof(short), 1, m_DbfFile_fp);
	/*BYTE name[11];
	BYTE fieldType;
	int Reserved3;
	BYTE fieldLength;
	BYTE decimalCount;
	short Reserved4;
	BYTE workID;
	short Reserved5[5];
	BYTE mDXFlag1;*/
	int fieldscount;
	fieldscount = (HeaderByteNum - 32) / 32;
	for (i = 0; i< fieldscount; i++)
	{
		RecordItem recordItem = recordItems[i];
		//FieldName----11   bytes
		fwrite(recordItem.name, 11, 1, m_DbfFile_fp);
		//'DEST'然后一堆'\0'
		//FieldType----1     bytes
		fwrite(&(recordItem.fieldType), sizeof(BYTE), 1, m_DbfFile_fp);
	    //67'C'
		//Reserved3----4     bytes
		fwrite(&(recordItem.Reserved3), sizeof(int), 1, m_DbfFile_fp);
		
		//FieldLength--1     bytes
		fwrite(&(recordItem.fieldLength), sizeof(BYTE), 1, m_DbfFile_fp);
		//32''
		int s = recordItem.fieldLength;
		int d = s;
		//DecimalCount-1   bytes
		fwrite(&(recordItem.decimalCount), sizeof(BYTE), 1, m_DbfFile_fp);
		
		//Reserved4----2     bytes
		fwrite(&(recordItem.Reserved4), sizeof(short), 1, m_DbfFile_fp);
		
		//WorkID-------1    bytes
		fwrite(&(recordItem.workID), sizeof(BYTE), 1, m_DbfFile_fp);
		
		//Reserved5----10   bytes
		for (j = 0; j<5; j++)
		{
			fwrite(recordItem.Reserved5 + j, sizeof(short), 1, m_DbfFile_fp);
		}
		
		//MDXFlag1-----1  bytes
		fwrite(&(recordItem.mDXFlag1), sizeof(BYTE), 1, m_DbfFile_fp);
	}
	BYTE terminator=13;
	fread(&terminator, sizeof(BYTE), 1, m_DbfFile_fp);
	//写dbf文件头结束
	CString Soil_code, suit;
	int  Ecrm, Elevt;
	double shapeArea, shapeLength;
	CString Dest, Ec, cc;
	BYTE deleteFlag;
	char media[40];
	vector<CString> polygonAttribute;
	vector<double> ShapeArea;
	for (i = 0; i<RecordNum; i++)
	{
		CGeoPolygon* polygon = (CGeoPolygon*) map->getTopoLayer()->objects[i];
		deleteFlag = 32;
		fwrite(&deleteFlag, sizeof(BYTE), 1, m_DbfFile_fp); //读取删除标记  1字节
		//读取 ObjectID int
		stringstream ss;
	    ss << i;
		string str= ss.str();
		int length = str.length();
		//media = str.data();
		memset(media, '\0', 40);
		for (int m = 0; m < 10-length; m++) {
			media[m] = ' ';
		}
		//memset(media, '\0', 40);
		for (int c = 10 - length; c < 10; c++) {
			media[c] = str[c - 10 + length];
		}
		//*(media + length) = '\0';
		for (j = 0; j<10; j++)
			fwrite(media + j, sizeof(char), 1, m_DbfFile_fp);   //--10
		//ObjectID = atoi(media);


		//读取 Dest string
		memset(media, '\0', 40);
		media[0] = '/';
		for (int c = 1; c <32; c++) {
			media[c] = ' ';
		}
		for (j = 0; j<32; j++)
			fwrite(media + j, sizeof(char), 1, m_DbfFile_fp);   //--32
		Dest = media;

		//读取 Ec string
		for (j = 0; j<16; j++)
			fwrite(media + j, sizeof(char), 1, m_DbfFile_fp);  //--16
		Ec = media;


		//读取 EcRm int
		ss << -8888;
		str = ss.str();
		length = str.length();
		memset(media, '\0', 40);
		for (int m = 0; m < 10 - length; m++) {
			media[m] = ' ';
		}
		for (int c = 10 - length; c < 10; c++) {
			media[c] = str[c - 10 + length];
		}

		//*(media + length + 1) = '\0';
		for (j = 0; j<10; j++)
			fwrite(media + j, sizeof(char), 1, m_DbfFile_fp);   //--10
		Ecrm = atoi(media);
		//读取 Elevt int
		
		for (j = 0; j<10; j++)
			fwrite(media + j, sizeof(char), 1, m_DbfFile_fp);   //--10
		Elevt = atoi(media);

		//读取 Cc int
		str = polygon->objectAttribute;
		memset(media, '\0', 40);
		length = str.length();
		for (int c = 0; c < length; c++) {
			media[c] = str[c];
		}
		for (int c =length; c < 8; c++) {
			media[c] = ' ';
		}
		for (j = 0; j<8; j++)
			fwrite(media + j, sizeof(char), 1, m_DbfFile_fp);   //--8
		cc = media;
		cc = cc.Left(4);
		//读取 shape_length double
		CString str1;
		double shape_length = polygon->TopoLength;
		shape_length = shape_length / MAPSCALE;
		str1.Format(_T("%.11e"), shape_length);
		memset(media, '\0', 40);
		media[0]= ' ';
		for (int c = 1; c < 16; c++) {
			media[c] = str1[c - 1];
		}
		if (str1.GetLength() == 18) {//+号后面是三位
			for (int c = 16; c < 19; c++) {
				media[c] = str1[c - 1];
			}
		}
		else {//+号后面是两位
			media[16] ='0';
			media[17] = str1[15];
			media[18] = str1[16];
		}
		//*(media + length ) = '\0';
		for (j = 0; j<19; j++)
			fwrite(media + j, sizeof(char), 1, m_DbfFile_fp);   //--19
		shapeLength = atof(media);
		//读取 shape_Area double

		double shape_area = polygon->TopoArea;
		shape_area = shape_area / (MAPSCALE*MAPSCALE);
		str1.Format(_T("%.11e"), shape_area);
		memset(media, '\0', 40);
		media[0] = ' ';
		for (int c = 1; c < 16; c++) {
			media[c] = str1[c - 1];
		}
		if (str1.GetLength() == 18) {//+号后面是三位
			for (int c = 16; c < 19; c++) {
				media[c] = str1[c - 1];
			}
		}
		else {//+号后面是两位
			media[16] = '0';
			media[17] = str1[15];
			media[18] = str1[16];
		}
		for (j = 0; j<19; j++)
			fwrite(media + j, sizeof(char), 1, m_DbfFile_fp);   //--19
		shapeArea = atof(media);
		int s = 1;
	}
	//读取dbf文件记录  结束
	fclose(m_DbfFile_fp);
}


void CNewTopoView::OnSelectpolygonid()
{
	// TODO: 在此添加命令处理程序代码
	this->currrentOperateStatus = SELECT;
		
	CSelectDialog dialog;
	int gaoliangID = -1;
	if (dialog.DoModal() == IDOK)
	{
		gaoliangID = dialog.m_SelectID;
	}
	if (map == NULL) {
		CString str = "没有数据，请输入数据";
		AfxMessageBox(str);
		return;
	}
	selectID = gaoliangID;
	if (dcCFG.gaoliangID.size()>0)
	{
		vector<int>().swap(dcCFG.gaoliangID);  
	}
	

	dcCFG.gaoliangID.push_back(gaoliangID);
	CGeoLayer *layer;
	if (isTopo) {
		layer =map->getTopoLayer() ;
	}
	else {
		layer =map->getLayerAt(0);
	}
	
	if (gaoliangID > layer->objects.size()||gaoliangID <0) {
		AfxMessageBox("超出范围，请重新输入！");
	
	}
	Invalidate();
}


void CNewTopoView::OnFindattribute()
{
	// TODO: 在此添加命令处理程序代码
	this->currrentOperateStatus = SELECT;

	CAttFindDialog dialog;
	CString str = "0000";
	if (dialog.DoModal() == IDOK) {
		str = dialog.attribute;
	}
	if (map == NULL) {
		CString str1 = "没有数据，请输入数据";
		AfxMessageBox(str1);
		return;
	}
	dcCFG.Attribute = str;
	dcCFG.reserveColor = dialog.lineColor;//获取的颜色变为保存颜色
	CGeoLayer* layer;
	if (isTopo) {
		layer = map->getTopoLayer();
	}
	else {
		layer = map->getLayerAt(0);
	}

	if (str=="0000") {
		AfxMessageBox("输入属性为“0000”请重新输入！");
		
	}
	Invalidate();
}


void CNewTopoView::OnMergegonsarea()//小面积
{
	// TODO: 在此添加命令处理程序代码
	CPolygonSimplifyDialog psDialog;
	double a = 0;
	double b = 0;
	double c = 0;
	double d = 0;
	if (psDialog.DoModal() == IDOK) {
		a = psDialog.MinBuildingArea;
		b = psDialog.MinGardenArea;
		c = psDialog.m_MinQitayuandiArea;
		d = psDialog.m_MinParkArea;
	}
	vector<double> areaParameter;
	areaParameter.push_back(a);
	areaParameter.push_back(b);
	areaParameter.push_back(c);
	areaParameter.push_back(d);
	CString str;
	bool flag = true;

	if (map != NULL) {
		if (map->isTopoed) {
			if (map->getTopoLayer()->commonEdge.size() != 0) {//如果建立拓扑，则flag=false；
				flag = false;
			}
		}
	}

	if (flag) {
		str = "未构建拓扑，请先构建拓扑";
		AfxMessageBox(str);
		return;
	}
	else {
		long t1 = GetTickCount();//程序段开始前取得系统运行时间(ms) 
		map->getTopoLayer()->PolylineDeleted(areaParameter);
		map->getTopoLayer()->ReoganizeCommonCircle();
		map->getTopoLayer()->delErrorCircle();
		map->getTopoLayer()->isPolygonSimply = true;
		if (map->getTopoLayer()->IsLineSimply == 1)
			map->getTopoLayer()->IsLineSimply = 2;//可以做到先进行线化简，再进行面化简则不会在保存时再rebuildcommoncircle
		long t2 = GetTickCount();//程序段结束后取得系统运行时间(ms)        
		str.Format("面化简time:%dms", t2 - t1);//前后之差即程序运行时间      
	}

	Invalidate();
}


void CNewTopoView::OnMergepolygonsameatt()//同属性
{
	// TODO: 在此添加命令处理程序代码
	CString str;
	bool flag = true;

	if (map != NULL) {
		if (map->isTopoed) {
			if (map->getTopoLayer()->commonEdge.size() != 0) {//如果建立拓扑，则flag=false；
				flag = false;
			}
		}
	}

	if (flag) {
		str = "未构建拓扑，请先构建拓扑";
		AfxMessageBox(str);
		return;
	}
	else {
		long t1 = GetTickCount();//程序段开始前取得系统运行时间(ms) 
		map->getTopoLayer()->PolylineDeleted();
		map->getTopoLayer()->ReoganizeCommonCircle();
		map->getTopoLayer()->delErrorCircle();
		map->getTopoLayer()->isPolygonSimply = true;
		if (map->getTopoLayer()->IsLineSimply == 1)
			map->getTopoLayer()->IsLineSimply = 2;//可以做到先进行线化简，再进行面化简则不会在保存时再rebuildcommoncircle
		long t2 = GetTickCount();//程序段结束后取得系统运行时间(ms)        
		str.Format("相同属性面合并time:%dms", t2 - t1);//前后之差即程序运行时间      
	}

	Invalidate();
}


void CNewTopoView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: 在此处添加消息处理程序代码
	IsSizeChanged = true;
	
	
	Invalidate();
}


void CNewTopoView::OnPolygonSelect()
{
	// TODO: 在此添加命令处理程序代码
	this->currrentOperateStatus = POLYGONSELECT;
}


// 通过鼠标点击选择某个面，同时高亮显示
void CNewTopoView::MouseSelectPolygon(CPoint pt)
{
	int gaoliang = map->getSelectPolygonID(pt);
	selectID = gaoliang;
	if(gaoliang==-1)
		AfxMessageBox("鼠标点击与空面");
	if (dcCFG.gaoliangID.size()>0)
	{
		vector<int>().swap(dcCFG.gaoliangID);  
	}
	if (this->currrentOperateStatus==POLYGONSELECT)
	{
		dcCFG.gaoliangID.push_back(gaoliang);
	}else if (this->currrentOperateStatus==ADJOINPOLYGONQUERY)
	{
		if (map->isTopoed)
		{
			CGeoPolygon* polygon =(CGeoPolygon*) map->getTopoLayer()->objects[gaoliang];
			int edgeSize = polygon->commonEdge.size();
			for (int i = 0;i<edgeSize;i++)
			{
				CGeoPolyline* line = polygon->commonEdge[i];
				if (line->leftPolygonID==selectID)
				{
					dcCFG.gaoliangID.push_back(line->rightPolygonID);
				}else if (line->rightPolygonID==selectID)
				{
					dcCFG.gaoliangID.push_back(line->leftPolygonID);
				}
			}
			sort(dcCFG.gaoliangID.begin(),dcCFG.gaoliangID.end());
			dcCFG.gaoliangID.erase(unique(dcCFG.gaoliangID.begin(), dcCFG.gaoliangID.end()), dcCFG.gaoliangID.end());
			if (dcCFG.gaoliangID[0]==-1)
			{
				vector<int>::iterator k = dcCFG.gaoliangID.begin();
				dcCFG.gaoliangID.erase(k);//删除第一个元素
			}
			
		}else{
			AfxMessageBox("未建立拓扑，无法查询，请先建立全局拓扑关系");
			dcCFG.gaoliangID.push_back(gaoliang);
		}
	}
	
}


void CNewTopoView::OnAdddata()
{
	// TODO: 在此添加命令处理程序代码
	this->currrentOperateStatus=ADDLAYER;
	CString filename = "";
	int layerIndex = 0;
	this->readData(filename,layerIndex);
	this->OnReadDbf(filename,layerIndex);
	//dcCFG.isAdd_Layer = true;
	this->Invalidate();
}


void CNewTopoView::OnPartTopo()
{
	// TODO: 在此添加命令处理程序代码
	if (map==NULL)
	{
		AfxMessageBox("地图为空，请打开数据！");
	} 
	else
	{
		bool flag = true;
		if (map->getTopoLayer()!=NULL)
		{
			if(map->getTopoLayer()->objects.size()==0){
				flag = false;
			}else{
				flag = true;
			}
		}else{
			flag=false;
		}
		if (flag)
		{
			//dcCFG.isAdd_Layer= true;
			map->mapPart_Topo();
		}else{
			AfxMessageBox("请先进行全局拓扑");
		}
	}
}




void CNewTopoView::OnBuildsimple()
{
		// todo: 在此添加命令处理程序代码
		bool flag = true;
		if (map != NULL) {
			if (map->isTopoed) {
				if (map->getTopoLayer()->commonEdge.size() != 0) {//如果建立拓扑，则flag=false；
					flag = false;
				}
			}
		}
	
		if (flag) {
			AfxMessageBox("未构建拓扑，请先构建拓扑");
			return;
		}
		double angle = 5;
		double area = 100;
		double length = 1000;
		map->getTopoLayer()->buildSimple(angle,area,length);
	
		Invalidate();
}


void CNewTopoView::OnRefineroad()
{
	// TODO: 在此添加命令处理程序代码
	map->getLayerAt(0)->getRoadCentreLine();
}


void CNewTopoView::OnAdjoinpolygonquery()
{
	this->currrentOperateStatus = ADJOINPOLYGONQUERY;
	// TODO: 在此添加命令处理程序代码
}


void CNewTopoView::OnFilter()   //zf,20180701
{
	// TODO: 在此添加命令处理程序代码
	 map->ObjectFilter();
	 //AfxMessageBox("按点数过滤成功！");
	 Invalidate();
}


void CNewTopoView::OnFilterByArea()   //zf,20180701
{
	// TODO: 在此添加命令处理程序代码
	map->ObjectFilterByArea();
	//AfxMessageBox("按面积过滤成功！");
	Invalidate();
}


void CNewTopoView::OnFindRectangle()   //zf,20180704
{
	// TODO: 在此添加命令处理程序代码
	map->FindRectangle();
	//AfxMessageBox("寻找一般外接矩形成功！");
	Invalidate();
}


void CNewTopoView::OnMABR()  //zf,0708
{
	// TODO: 在此添加命令处理程序代码
	map->FindMABR();
	//AfxMessageBox("寻找最小外接矩形成功！");
	Invalidate();
}


void CNewTopoView::OnOptimize()
{
	// TODO: 在此添加命令处理程序代码
	map->optimize();
	
	Invalidate();
}


void CNewTopoView::OnRemoveredundant()
{
	// TODO: 在此添加命令处理程序代码
	map->RemoveRedundant();
	//AfxMessageBox("");
	Invalidate();
}


void CNewTopoView::OnFinal()
{
	// TODO: 在此添加命令处理程序代码
	OnFilterByArea();
	OnRemoveredundant();
	OnDeletetail();
	OnMABR();
	OnOptimize();
}


void CNewTopoView::OnDeletetail()
{
	map->DeleteTail();
	//AfxMessageBox("去除尾巴成功！");
	Invalidate();
}
