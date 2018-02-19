


#include <windows.h>
#include <math.h>
#include "app.h"

#include <shlobj.h>

#pragma comment(lib, "msimg32.lib")
#pragma warning(disable:4244)	// 暗示的型変換 例:DOUBLE->LONG(小数点切り詰め)

#define	WT_REDRAW	1100

class DPOINT {
public:
	double	x;
	double	y;
};

class IMAGE : public DPOINT {
	HBITMAP		hBmp;
	HDC			hDC;
	POINT		size;
	POINT		hsize;
	UINT		crClr;
	double		range, degree;

public:
	IMAGE(){ hDC = NULL; hBmp = NULL; crClr = 0; range = 0; degree = 0; }
	~IMAGE(){
		if (hDC == NULL) return;
		DeleteObject(hBmp);
		DeleteDC(hDC);
		hDC = NULL;
	}
	LONG	xget()				{ return x; }
	void	xset(LONG x)		{ this->x = x; }
	LONG	yget()				{ return y; }
	void	yset(LONG y)		{ this->y = y; }
	LONG	sxget()				{ return size.x; }
	LONG	syget()				{ return size.y; }
	LONG	hsxget()			{ return hsize.x; }
	LONG	hsyget()			{ return hsize.y; }
	void	dset(double degree)	{ this->degree = degree; }
	double	dget()				{ return degree; }
	void	rset(double range)	{ this->range = range; }
	double	rget()				{ return range; }

	bool init(HINSTANCE hInst, HDC hDC, char* filename, LONG x = 0L, LONG y = 0L, COLORREF crClr = 0){
		hBmp = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (hBmp == NULL) return false;

		this->hDC = CreateCompatibleDC(hDC);
		if (hDC == NULL) return false;
		SelectObject(this->hDC, hBmp);

		BITMAP bmp;
		if (! GetObject(hBmp, sizeof(BITMAP), &bmp)) return false;
		size.x = bmp.bmWidth;
		size.y = bmp.bmHeight;
		hsize.x = size.x/2;
		hsize.y = size.y/2;
		this->x = x;
		this->y = y;
		this->crClr = crClr;

		return true;
	}
	void draw(HDC hDC){
		TransparentBlt(hDC, x, y, size.x, size.y,
			this->hDC, 0, 0, size.x, size.y, crClr);
	}
	void hdraw(HDC hDC){// 半径を中心にして描画
		TransparentBlt(hDC, x-hsize.x, y-hsize.y, size.x, size.y,
			this->hDC, 0, 0, size.x, size.y, crClr);
	}
};

enum SP { SP_SUN = 0, SP_MER, SP_VEN, SP_EAR, SP_MAX };
class SPACE {
	HINSTANCE	hInst;
	HWND		hWnd;
	HDC			hDC, hMDC;
	HBITMAP		hMBmp;
	HFONT		hFont;
	HBRUSH		hBrush;
	HPEN		hPen;
	IMAGE		Img[SP_MAX];

	bool		redraw;		// 再描画ビット
	double		magnif;		// 倍率

	void inline validback(){ BitBlt(hDC, 0, 0, ginfo(hDC, GI_X), ginfo(hDC, GI_Y), hMDC, 0, 0, SRCCOPY); }
	void draw();

public:
	~SPACE(){ exit(); }
	bool init(HINSTANCE hInst, HWND hWnd);
	void exit();

	void calc();
	void redrawf(){ redraw = true; }
};

namespace {// グローバル変数
	char	AppName[] = "Space";
	POINT	sWin = { 1024, 600 };	// ウィンドウサイズ
	//LONG	AU = 149597870700;		// 1天文単位(m) (誤差3m)
	LONG	AU = 14959;
	SPACE sp;	// 宇宙クラス
}




bool SPACE::init(HINSTANCE hInst, HWND hWnd){
	bool b = true;
	this->hInst	= hInst;
	this->hWnd	= hWnd;
	hDC		= GetDC(hWnd);

	// 裏画面作成
	hMDC	= CreateCompatibleDC(hDC);
	hMBmp	= CreateCompatibleBitmap(hDC, ginfo(hDC, GI_X), ginfo(hDC, GI_Y));
	SelectObject(hMDC, hMBmp);
	// フォント
	hFont	= (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SelectObject(hMDC, hFont);
	// ブラシ
	hBrush	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	SelectObject(hMDC, hBrush);
	// ペン
	hPen	= (HPEN)GetStockObject(WHITE_PEN);
	SelectObject(hMDC, hPen);

	// 画像読み込み
	char pDesk[MAX_PATH];
	SHGetSpecialFolderPath(NULL, pDesk, CSIDL_DESKTOPDIRECTORY, FALSE);
	lstrcat(pDesk, "\\star\\");
	SetCurrentDirectory(pDesk);
	if (!Img[SP_SUN].init(hInst, hDC, "sun.bmp", 0, 0)){
		MessageBox(NULL, "画像[sun]が読み込まれませんでした", "エラー", MB_OK);
		b = false;
	}
	Img[SP_SUN].x = (ginfo(hWnd, GI_CX)-Img[SP_SUN].sxget())/2;
	Img[SP_SUN].y = (ginfo(hWnd, GI_CY)-Img[SP_SUN].syget())/2;

	if (!Img[SP_MER].init(hInst, hDC, "mercury.bmp", 0, 0)){
		MessageBox(NULL, "画像[mercury]が読み込まれませんでした", "エラー", MB_OK);
		b = false;
	}
	Img[SP_MER].rset(5800);

	if (!Img[SP_VEN].init(hInst, hDC, "venus.bmp", 0, 0)){
		MessageBox(NULL, "画像[venus]が読み込まれませんでした", "エラー", MB_OK);
		b = false;
	}
	Img[SP_VEN].rset(10820);

	if (!Img[SP_EAR].init(hInst, hDC, "earth.bmp", 0, 0)){
		MessageBox(NULL, "画像[earth]が読み込まれませんでした", "エラー", MB_OK);
		b = false;
	}
	Img[SP_EAR].rset(AU);

	magnif = 0.02;	// 倍率

	return b;
}

void SPACE::exit(){
	if (hDC == NULL) return;
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(hFont);
	DeleteObject(hMBmp);
	DeleteDC(hMDC);
	ReleaseDC(hWnd, hDC);
	hDC = NULL;
}

// 描画
namespace {
	char str_d[128];		// フリーの文字列
	char Month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	SYSTEMTIME syst;		// 現在時刻
}
void SPACE::draw(){
	Img[SP_SUN].hdraw(hMDC);
	Img[SP_MER].hdraw(hMDC);
	Img[SP_VEN].hdraw(hMDC);
	Img[SP_EAR].hdraw(hMDC);

	wsprintf(str_d, "画面サイズ[%d:%d]", ginfo(hWnd, GI_CX), ginfo(hWnd, GI_CY));
	TextOut(hMDC, 0,  0, str_d, lstrlen(str_d));
	wsprintf(str_d, "太陽の位置[%d:%d]", (LONG)Img[SP_SUN].x, (LONG)Img[SP_SUN].y);
	TextOut(hMDC, 0, 20, str_d, lstrlen(str_d));
	wsprintf(str_d, "地球の位置[%d:%d (%d)]", (LONG)Img[SP_EAR].x, (LONG)Img[SP_EAR].y, (LONG)Img[SP_EAR].dget());
	TextOut(hMDC, 0, 40, str_d, lstrlen(str_d));


	validback();	// 裏画面反映
}

// 計算
void SPACE::calc(){
	// 現在時刻
	GetLocalTime(&syst);

	// 太陽の位置
	Img[SP_SUN].x = (ginfo(hWnd, GI_CX))/2;
	Img[SP_SUN].y = (ginfo(hWnd, GI_CY))/2;

	// 水星の位置
	Img[SP_MER].dset(
		(Month[syst.wMonth-1] + syst.wDay) * (((double)AU)*2.0*PI/88.0/24.0)
	);
	Img[SP_MER].x = cos(torad(Img[SP_MER].dget()-90))*Img[SP_MER].rget()*magnif
		+Img[SP_SUN].x;
	Img[SP_MER].y = sin(torad(Img[SP_MER].dget()-90))*Img[SP_MER].rget()*magnif
		+Img[SP_SUN].y;

	// 金星の位置
	Img[SP_VEN].dset(
		(Month[syst.wMonth-1] + syst.wDay) * (((double)AU)*2.0*PI/225.0/24.0)
	);
	Img[SP_VEN].x = cos(torad(Img[SP_VEN].dget()-90))*Img[SP_VEN].rget()*magnif
		+Img[SP_SUN].x;
	Img[SP_VEN].y = sin(torad(Img[SP_VEN].dget()-90))*Img[SP_VEN].rget()*magnif
		+Img[SP_SUN].y;

	// 一日 = 公転軌道の円周 / 365
	// 地球の現在角度 公転軌道の円周=AU*2*PI
	Img[SP_EAR].dset(
		(Month[syst.wMonth-1] + syst.wDay) * 
		(( ((double)AU)*2.0*PI )/365.0)
	);
	// 地球の位置
//	Img[SP_EAR].x = cos(torad(Img[SP_EAR].dget()))*Img[SP_EAR].rget()*magnif
	Img[SP_EAR].x = cos(Img[SP_EAR].dget())*Img[SP_EAR].rget()*magnif
		+Img[SP_SUN].x;
//	Img[SP_EAR].y = sin(torad(Img[SP_EAR].dget()))*Img[SP_EAR].rget()*magnif
	Img[SP_EAR].y = cos(Img[SP_EAR].dget())*Img[SP_EAR].rget()*magnif
		+Img[SP_SUN].y;


	if (redraw) draw();
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_TIMER:
		sp.redrawf();
		if (wp == WT_REDRAW) sp.calc();
		break;
	case WM_DESTROY:
		KillTimer(hWnd, WT_REDRAW);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){
	HWND	hWnd;
	MSG		msg;

	WNDCLASSEX wce;
	wce.cbSize		= sizeof(wce);	// 構造体のsize
	wce.style		= CS_HREDRAW | CS_VREDRAW;	// スタイル
	wce.lpfnWndProc	= WindowProc;	// プロージャ関数
	wce.cbClsExtra	= wce.cbWndExtra = 0;
	wce.hInstance	= hInst;		// プログラムのハンドル
	wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// アイコン
	wce.hCursor		= LoadCursor(NULL, IDC_ARROW);		// マウスカーソル
	wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// ブラシ
	wce.lpszMenuName	= NULL;		// メニュー
	wce.lpszClassName	= AppName;	// クラス名
	wce.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wce)){
		MessageBox(NULL, "Not created class.\n\nexit now.", "Create error", MB_OK | MB_ICONERROR);
		return 1;
	}

	hWnd = CreateWindowEx(0, AppName, AppName, 
		WS_OVERLAPPEDWINDOW-WS_THICKFRAME-WS_MAXIMIZEBOX | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, sWin.x+6, sWin.y+28,
		NULL, NULL, NULL, NULL
	);
	if (hWnd == NULL) return -1;
	ShowWindow(hWnd, SW_SHOW);

	// 初期化
	if (!sp.init(hInst, hWnd)){
		MessageBox(NULL, "終了します", "exit now.", MB_OK);
		return 0;
	}

	SetTimer(hWnd, WT_REDRAW, 1000, NULL);
	SendMessage(hWnd, WM_TIMER, WT_REDRAW, 0);

	while(1) {
		Sleep(1);
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			// システムメイン
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUITが来てたら
			TranslateMessage(&msg);	// キーボードを利用可能にする
			DispatchMessage(&msg);	// 制御をWindowsに戻す
		}else {
		}
	}

	sp.exit();
	return 0;
}

