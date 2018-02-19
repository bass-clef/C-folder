
#include <windows.h>
#include "application.h"

// クラス作成
BOOL ClassInit(WNDCLASSEX &wce, char* AppName){

	wce.cbSize		= sizeof(wce);	// 構造体のsize
	wce.style		= CS_HREDRAW | CS_VREDRAW;	// スタイル
	wce.lpfnWndProc	= DefWindowProc;	// プロージャ関数
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

// ウィンドウ作成・表示
BOOL WindowInit(HINSTANCE hInstance, HWND &hWnd, int nCmd, char* AppName){
	
	hWnd = CreateWindowEx(
		0, AppName, AppName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
		NULL, NULL, hInstance, NULL
	);
	if (!hWnd)return FALSE;
	ShowWindow(hWnd, nCmd);

	return TRUE;
}

// ウィンドウの情報
int GetWindowsInfo(UINT uGetInfoType){
	int ret = NULL;
	switch(uGetInfoType){
	case GWSI_DESKTOPX:{
		RECT	rc;
		GetWindowRect(GetDesktopWindow(), &rc);
		ret = rc.right;
		break;}
	case GWSI_DESKTOPY:{
		RECT	rc;
		GetWindowRect(GetDesktopWindow(), &rc);
		ret = rc.bottom;
		break;}
	}
	return ret;
}



// DCとBitmapを作成して選択
void CreateDCWithBitmap(HDC &hDC, HBITMAP &hbmp, int xSize, int ySize, HDC hDC2){
	hDC		= CreateCompatibleDC(hDC2);
	hbmp	= CreateCompatibleBitmap(hDC2, xSize, ySize);
	SelectObject(hDC, hbmp);
}

// ブラシを作成して選択
void CreateSelectBursh(HDC hDC, HBRUSH &hbr, COLORREF crColor){
	hbr = CreateSolidBrush(crColor);
	SelectObject(hDC, hbr);
}

// ペンを作成して選択
void CreateSelectPen(HDC hDC, HPEN &hpen, COLORREF crColor){
	hpen = CreatePen(PS_SOLID, 1, crColor);
	SelectObject(hDC, hpen);
}

// 文字列表示
void Print(HDC hDC, LPSTR Text, int xPos, int yPos) {
	int		Len = lstrlen(Text);
	char	*Str =  new char[Len+5];
	wsprintf(Str, "%s", Text);
//	DrawText(hDC, Str, Len, NULL, DT_NOCLIP);
	TextOut(hDC, xPos, yPos, Str, Len);
	delete[] Str;
}

// 線を描画
void DrawLine(HDC hDC, int x1, int y1, int x2, int y2){
	MoveToEx(hDC, x1, y1, NULL);
	LineTo(hDC, x2, y2);
}

// 四角形描画
void DrawRect(HDC hDC, int x1, int y1, int x2, int y2, UINT DrawType){
	switch(DrawType){
	case DR_FRAME:{
		RECT	rc = {x1, y1, x2, y2};
		FrameRect(hDC, &rc, (HBRUSH)GetCurrentObject(hDC, OBJ_PEN));
		break;}
	case DR_FILL:
		Rectangle(hDC, x1, y1, x2, y2);
		break;
	case DR_GRADATION:
		break;
	}
}

// 円描画
void DrawCircle(HDC hDC, int x1, int y1, int x2, int y2, UINT DrawType){
	switch(DrawType){
	case DC_FRAME:
		Arc(hDC, x1, y1, x2, y2, x2-x1, y1, x2-x1, y1);
		break;
	case DC_FILL:
		Ellipse(hDC, x1, y1, x2, y2);
		break;
	}
}

// DCをコピー
void CopyDC(HDC hToDC, HDC hFromDC, DWORD dwrop) {
	BitBlt(hToDC, 0, 0, GetDeviceCaps(hToDC, HORZRES), GetDeviceCaps(hFromDC, VERTRES),
		hFromDC, 0, 0, dwrop
	);
}


// DCとBitmapを削除
void DeleteDCWithBitmap(HDC hDC, HBITMAP hbmp){
	DeleteObject(hbmp);
	DeleteDC(hDC);
}


// 書式を施して戻り値で戻る
char* strf(char* buffer, char* format, ...){
	va_list argptr;
	
	va_start(argptr, format);
	vsprintf_s(buffer, sizeof(buffer), format, argptr);
	va_end(argptr);
	
	return buffer;
}

