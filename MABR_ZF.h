#pragma once
#include<vector>
class MABR_ZF
{
public:
	MABR_ZF(void);
	~MABR_ZF(void);
	// 多边形的序号，0708，zf
	int ID;
	// 多边形的面积，zf，0708
	double area;

	// 多边形的顶点，zf，0708
	vector<CPoint*> vertices;
};

