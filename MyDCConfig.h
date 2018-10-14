#pragma once
#include <vector>
using namespace std;
class CMyDCConfig
{
public:
	CMyDCConfig();
	~CMyDCConfig();

	int style;
	bool isShowTopo;
	bool isShowOriMap;
	bool isShowExport;
	float width;
	int lineStyle; 
	COLORREF lineColor;
	int fillstyle;
	COLORREF fillColor;
	vector<int> gaoliangID;
	CString Attribute;
	COLORREF reserveColor;
	//bool isAdd_Layer;
};

