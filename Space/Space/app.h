

#ifndef APP_H
#define APP_H

#include <windows.h>

// ウィンドウ情報取得
enum GINFO_PARAMETER { GI_NOTHING, GI_X, GI_Y, GI_WX, GI_WY, GI_CX, GI_CY };
LONG inline ginfo(HANDLE handle, int gettype){
	switch(gettype){
	case GI_X:	return GetDeviceCaps((HDC)handle, HORZRES);
	case GI_Y:	return GetDeviceCaps((HDC)handle, VERTRES);
	case GI_WX:	break;
	case GI_WY:	break;
	case GI_CX: {
		RECT rc;
		GetClientRect((HWND)handle, &rc);
		return rc.right;
	}
	case GI_CY: {
		RECT rc;
		GetClientRect((HWND)handle, &rc);
		return rc.bottom;
	}
	}
	return GI_NOTHING;
}

#define PI 3.1415926535
double inline torad(double degree){
	return degree * PI / 180.0;
}
double inline todeg(double radian){
	return radian * 180.0 / PI;
}

#endif
