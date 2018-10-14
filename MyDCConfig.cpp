#include "stdafx.h"
#include "MyDCConfig.h"


CMyDCConfig::CMyDCConfig()
{
	isShowTopo = false;
	isShowOriMap = false;
	isShowExport = false;
	Attribute = "0000";
	reserveColor = RGB(0, 250, 0);
	fillstyle = 0;
	//isAdd_Layer = false;
}


CMyDCConfig::~CMyDCConfig()
{
}
