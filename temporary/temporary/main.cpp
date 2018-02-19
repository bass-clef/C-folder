
#include <windows.h>
#include <shobjidl.h>
#include "application.h"

namespace {
	String str_g;
}

typedef void* (__stdcall *DLLFUNC)(...);

template<class DLLtype=DLLFUNC> class DLL{
public:
	enum {ER_NODLLNAME = 0, ER_NOFUNCNAME, ER_FREELIBRARY};


	HMODULE hm;
	DLLtype func;

	DLL(LPSTR dllName, LPSTR funcName){
		hm = NULL;
		func = NULL;
		import(dllName, funcName);
	}
	~DLL(){	if (free()) { throw ER_FREELIBRARY; return; } }

	void import(LPSTR dllName, LPSTR funcName){
		if (free()) { throw ER_FREELIBRARY; return; }

		hm = LoadLibrary(dllName);
		if (hm == NULL) { throw ER_NODLLNAME; return; }

		func = (DLLtype)GetProcAddress(hm, funcName);
		if (func == NULL) { throw ER_NOFUNCNAME; return; }
	}
	int free(){
		if (hm != NULL) {
			if (!FreeLibrary(hm)) { return ER_FREELIBRARY;}
			hm = NULL;
		}
		return 0;
	}

};

BOOL AddTaskBarButton(HWND hWnd, ITaskbarList3* &ptl, const WCHAR* szTip, HICON hIcon = NULL);
int UpdateLayeredImage( HWND hwnd, HBITMAP hbmp, int x, int y );

// ウィンドウプロージャー
LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_COMMAND:
		if (HIWORD(wp) == THBN_CLICKED) {
			if (LOWORD(wp) == 0) MessageBox(NULL, "Button 0 clicked", "", MB_OK);
			if (LOWORD(wp) == 1) MessageBox(NULL, "Button 1 clicked", "", MB_OK);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lp){

	char str[16];
	str_g += strf(str, "%d", hWnd);

	return true;
}

// アプリケーションメイン
bool WINDOW::App(){
	return false;
}


class XYZType {
public:
	double x, y, z;

	XYZType(double x = 0.0, double y = 0.0, double z = 0.0){
		this->x = x;
		this->y = y;
		this->z = z;
	}
} typedef Dpoint3d;

BOOL Cross_LineVSLine
(
XYZType L1,
XYZType L2,
XYZType L3,
XYZType L4,
XYZType *CrossP
)
{
	double ksi, eta, delta;
	double ramda, mu;

	ksi = (L4.y - L3.y)*(L4.x - L1.x) -
		(L4.x - L3.x)*(L4.y - L1.y);

	eta = (L2.x - L1.x)*(L4.y - L1.y) -
		(L2.y - L1.y)*(L4.x - L1.x);

	delta = (L2.x - L1.x)*(L4.y - L3.y) -
		(L2.y - L1.y)*(L4.x - L3.x);

	ramda = ksi / delta;
	mu = eta / delta;

	if ((ramda >= 0 && ramda <= 1) && (mu >= 0 && mu <= 1))
	{
		CrossP->x = L1.x + ramda*(L2.x - L1.x);
		CrossP->y = L1.y + ramda*(L2.y - L1.y);
		//交点の高さ
		CrossP->z = L1.z + ramda*(L2.z - L1.z);

		return TRUE;
	}

	return FALSE;
}

// 線分(p1,p2)と直線(p3,p4)の交点を求める
// 平行時は無限遠点が交点になっている（一応）
int crossPoint
(
const Dpoint3d &p1,
const Dpoint3d &p2,
const Dpoint3d &p3,
const Dpoint3d &p4,
Dpoint3d &ap1
)
{
	double dev = (p2.y - p1.y)*(p4.x - p3.x) - (p2.x - p1.x)*(p4.y - p3.y);
	if (!dev)
	{
		ap1.x = -1;
		ap1.y = -1;
		ap1.z = 0;
		return 0;
	}

	double d1, d2;

	d1 = (p3.y*p4.x - p3.x*p4.y);
	d2 = (p1.y*p2.x - p1.x*p2.y);

	ap1.x = d1*(p2.x - p1.x) - d2*(p4.x - p3.x);
	ap1.x /= dev;
	ap1.y = d1*(p2.y - p1.y) - d2*(p4.y - p3.y);
	ap1.y /= dev;

	return 1;//isRange(p1, p2, ap1) ? 1 : 0;
}


// システムメイン
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	// クラス登録とウィンドウの作成
	CLASS c("Test", WindowProc);
	WINDOW w(hInst, nCmd, "Test", "Test");

	// コメントインすると例外でる
//	DLL<LPSTR (__stdcall *)(void)> f("hspsock.dll", "_ipget@16");
//	f.func();

	XYZType L1(59, 224), L2(590, 2240), L3(127, -455), L4(1270, -455), result(0, 0, 0);
	bool b = crossPoint(L1, L2, L3, L4, result);
	
	String str(24);
	str.strf("%d", b);
	if (!b) return 0;
	str.print(w, 0, 0);
	str.strf("%f", result.x);
	str.print(w, 0, 20);
	str.strf("%f", result.y);
	str.print(w, 0, 40);
	str.strf("%f", result.z);
	str.print(w, 0, 60);

	w.Repaint();

	while (!w.MessageLoop()) Sleep(1);
	return 0;
}

BOOL AddTaskBarButton(HWND hWnd, ITaskbarList3* &ptl, const WCHAR* szTip, HICON hIcon){
	THUMBBUTTON buttons;//構造体
	
	buttons.dwMask = THB_ICON|THB_TOOLTIP|THB_FLAGS;
	buttons.iId = 0;
	if (hIcon == NULL) buttons.hIcon = (HICON)GetClassLong(hWnd, GCL_HICONSM);
	else buttons.hIcon = hIcon;

	wcscpy(buttons.szTip, szTip);//wcslen(TooltipName), TooltipName);
	buttons.dwFlags = THBF_ENABLED;
	
	ptl->ThumbBarAddButtons(hWnd, 1, &buttons);
	return true;
}

int UpdateLayeredImage( HWND hwnd, HBITMAP hbmp, int x, int y ){
	HDC hdcScreen = ::GetDC( NULL );
	HDC hdcMemory = ::CreateCompatibleDC( hdcScreen );

	SelectObject( hdcMemory, hbmp );

	RECT rect;
	GetWindowRect( hwnd, &rect );
	POINT pos = {rect.left, rect.top};
	SIZE szWindow = {x, y};
	POINT ptSrc = {0, 0};

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA };

	int ret = UpdateLayeredWindow( hwnd, hdcScreen, &pos, &szWindow, hdcMemory,
			&ptSrc, 0, &bf, ULW_ALPHA );

	::DeleteDC( hdcMemory );
	::ReleaseDC( NULL, hdcScreen );
	return ret;
}