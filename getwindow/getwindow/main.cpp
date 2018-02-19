
#include <Windows.h>

#define WM_GETWINDOW WM_APP+100

namespace {
	HWND hGWH = NULL;
	WNDPROC pParent = NULL;
	HWND hParentWnd = NULL;
	RECT ptAdd;
	bool fChild;
	bool fStart = 0;
}

// クラス作成
BOOL ClassInit(WNDCLASSEX &wce, char* AppName, WNDPROC wndproc){

	wce.cbSize		= sizeof(wce);	// 構造体のsize
	wce.style		= CS_HREDRAW | CS_VREDRAW;	// スタイル
	wce.lpfnWndProc	= wndproc;	// プロージャ関数
	wce.cbClsExtra	= wce.cbWndExtra = 0;
	wce.hInstance	= NULL;	// プログラムのハンドル
	wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// アイコン
	wce.hCursor		= LoadCursor(NULL, IDC_ARROW);	// マウスカーソル
	wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// ブラシ
	wce.lpszMenuName	= NULL;	// メニュー
	wce.lpszClassName	= AppName;	// クラス名
	wce.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wce))return FALSE;

	return TRUE;
}

LRESULT CALLBACK SubClassProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_MOVING:{
			RECT rc, trc;
			GetClientRect(hWnd, &rc);
			GetWindowRect(hWnd, &trc);

			long width = trc.right-trc.left;
			long height = trc.bottom-trc.top;
			long marw = (width-rc.right)/2;
			long marh = height-rc.bottom-marw;

			SetWindowPos(hGWH, NULL,
				((RECT*)lp)->left+marw+ptAdd.left,
				((RECT*)lp)->top+marh+ptAdd.top,
				0, 0, SWP_NOSIZE);
			SetActiveWindow(hGWH);
		}
	}
	return CallWindowProc(pParent, hWnd, uMsg, wp, lp);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch (uMsg){
	case WM_PAINT:{
		HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
		HDC hDC = GetDC(hWnd);

		FillRect(hDC, &ptAdd, hBrush);
		Arc(hDC, 0, 0, ptAdd.right, ptAdd.bottom, 0, 0, 0, 0);

		MoveToEx(hDC, ptAdd.right / 2, 0, NULL);
		LineTo(hDC, ptAdd.right / 2, ptAdd.bottom);

		MoveToEx(hDC, 0, ptAdd.bottom / 2, NULL);
		LineTo(hDC, ptAdd.right, ptAdd.bottom / 2);

		DeleteObject(hBrush);
		ReleaseDC(hWnd, hDC);
		break;
	}
	case WM_SIZING:{
		RECT *rc = (RECT*)lp;
		rc->right = rc->left + ptAdd.right;
		rc->bottom = rc->top + ptAdd.bottom;
		return 0;
	}
	case WM_LBUTTONDOWN:{
		SetLayeredWindowAttributes(hWnd, RGB(255, 255, 255), 0, LWA_COLORKEY);
		SendMessage(hWnd, WM_NCLBUTTONDOWN, 2, 0);

		POINT pt;
		RECT rc;
		GetWindowRect(hWnd, &rc);
		pt.x = rc.left + ptAdd.right/2;
		pt.y = rc.bottom + ptAdd.bottom/2;

		HWND hTWnd = WindowFromPoint(pt), hRWnd;
		if (fChild) hRWnd = GetAncestor(hTWnd, GA_ROOT);
		if ((hTWnd == hParentWnd) || ((fChild) && (hRWnd == hParentWnd))) {
			SetLayeredWindowAttributes(hWnd, 0, 255, 0);
			RECT trc;
			GetClientRect(hParentWnd, &rc);
			GetWindowRect(hParentWnd, &trc);

			long width = trc.right-trc.left;
			long height = trc.bottom-trc.top;
			long marw = (width-rc.right)/2;
			long marh = height-rc.bottom-marw;

			SetWindowPos(hWnd, NULL,
				trc.left + marw+ptAdd.left,
				trc.top  + marh+ptAdd.top,
				ptAdd.right, ptAdd.bottom, 0);
			return 0;
		}
		SendMessage(hParentWnd, WM_GETWINDOW, (WPARAM)MAKELONG(pt.x, pt.y),
			(LPARAM)hTWnd);

		SendMessage(hWnd, WM_DESTROY, 0, 0);
		return 0;
		}
	case WM_DESTROY:
		SetWindowLong(hParentWnd, GWL_WNDPROC, (LONG)pParent);
		ShowWindow(hWnd, SW_HIDE);
//		DestroyWindow(hWnd);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

HWND MakeGetWindowHandleObject(HWND hParent, int x, int y, int width, int height, bool child){

	if (!fStart) {
		WNDCLASSEX wce;
		if (!ClassInit(wce, "getwindow", WindowProc)) return NULL;
	}
	fStart = 1;

	RECT rc, trc;
	GetClientRect(hParent, &rc);
	GetWindowRect(hParent, &trc);

	long marw = ((trc.right-trc.left)-rc.right)/2;
	long marh = (trc.bottom-trc.top)-rc.bottom-marw;
	hGWH = CreateWindowEx(0, "getwindow", "",
		WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		trc.left+marw+x, trc.top+marh+y, width, height,
		hParent, NULL, NULL, NULL);

	SetWindowLong(hGWH, GWL_EXSTYLE, GetWindowLong(hGWH, GWL_EXSTYLE) | WS_EX_LAYERED);

	rc.left = 0, 0, width, height;
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	HDC hDC = GetDC(hGWH);

	FillRect(hDC, &rc, hBrush);
	Arc(hDC, 0, 0, width, height, 0, 0, 0, 0);

	MoveToEx(hDC, width/2, 0, NULL);
	LineTo(hDC, width/2, height);

	MoveToEx(hDC, 0, height/2, NULL);
	LineTo(hDC, width, height/2);

	DeleteObject(hBrush);
	ReleaseDC(hGWH, hDC);

	fChild = child;
	ptAdd.left = x;
	ptAdd.top = y;
	ptAdd.right = width;
	ptAdd.bottom = height;
	hParentWnd = hParent;
	pParent = (WNDPROC)GetWindowLong(hParent, GWL_WNDPROC);
	SetWindowLong(hParent, GWL_WNDPROC, (LONG)SubClassProc);

	return hGWH;
}

BOOL WINAPI DllMain(HINSTANCE hDllInst, DWORD fdwReason, LPVOID lpvReserved){
	return TRUE;
}


