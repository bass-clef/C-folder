
#include <windows.h>
#include <math.h>

#define PI			3.1415926535
#define radian(d)	(d * PI / 180.0)

POINT* RoteBlt(HDC hToDC, float cx, float cy, float sx, float sy, HDC hFromDC, int ix, int iy, int ixs, int iys, float angle){
	float
		// 中心
		hx = sx/2,
		hy = sy/2,

		// 各頂点と中心との角度
		a1 = atan2(hy - 0, hx - 0) + (float)radian(angle),
		a2 = atan2(hy - 0, hx - sx) + (float)radian(angle),
		a3 = atan2(hy - sy, hx - 0) + (float)radian(angle),

		// 中心と頂点の距離(どの頂点も同じ距離)
		r1 = sqrt(powf(hx - 0, 2) + powf(hy - 0, 2)),
		r2 = sqrt(powf(hx - sx, 2) + powf(hy - 0, 2)),
		r3 = sqrt(powf(hx - 0, 2) + powf(hy - sy, 2));

	POINT pt[3] = {
		// 左上, 右上, 左下
		{(int)cx+(int)(cos(a1)*r1 + hx), (int)cy+(int)(sin(a1)*r1 + hy)},
		{(int)cx+(int)(cos(a2)*r2 + hx), (int)cy+(int)(sin(a2)*r2 + hy)},
		{(int)cx+(int)(cos(a3)*r3 + hx), (int)cy+(int)(sin(a3)*r3 + hy)}
	};

	if (ixs != 0 && iys != 0)
		PlgBlt(hToDC, pt, hFromDC, ix, iy, ixs, iys, NULL, 0, 0);
	return &pt[0];
}

void DCRevision(HDC hDC, int xMax, int yMax, COLORREF cClr, COLORREF aClr, COLORREF sClr){
	COLORREF clr;
	for(int y=0; y<yMax; y++) for(int x=0; x<xMax; x++){
		clr = GetPixel(hDC, x, y);
		if (clr > cClr) {
			clr += aClr;
			if (clr > 0x00FFFFFF) clr = 0x00FFFFFF;
		}else {
			clr -= sClr;
			if (clr < 0) clr = 0;
		}
		SetPixel(hDC, x, y, clr);
	}
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD fdwReason,LPVOID lpvReserved){
	
	return TRUE;
}

