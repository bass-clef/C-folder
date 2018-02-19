


#ifndef DEFINE_H
#define DEFINE_H

// コンパイラエラー無視
#pragma warning(disable:4244)	// 暗示的型変換 例:DOUBLE->LONG(小数点切り詰め)

// ヘッダーファイル読み込み
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <shlobj.h>
#include <stdarg.h>
// STL
#include <string>

// 参照ライブラリ追加
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "msimg32.lib")


// エラー処理用クラス その定数列挙
enum STG_ERROR {
	ERROR_IMG = 100,
	E_IMG_PLAYER,
	E_IMG_ENEMY
};
class S_ERROR {
	long number;
	long line;

public:
	void seterror(STG_ERROR error){
		number = error;
		line = __LINE__;
	}
	long nget(){ return number; }
	long lget(){ return line; }
};


// グローバル変数・グローバル関数・定数 の定義
namespace {
	const int FPS = 60;		// set frames person second
	DWORD now_time = 0;		// now frames person second
	DWORD start_time = timeGetTime();
	char AppName[] = "mono";	// アプリケーション名
	const double PI = 3.1415926535;	// 円周率
	const POINT	sWin = {640, 480};	// ウィンドウサイズ
	S_ERROR e;	// エラー処理用
	// do
	char	g_str[128];	// グローバル変数 (char)
	int		g_int[4];	// グローバル変数 (int)
};

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

// 度・ラジアン 交互変換
double inline torad(double degree){
	return degree * PI / 180.0;
}
double inline todeg(double radian){
	return radian * 180.0 / PI;
}

// 書式返し
char* strf(char* buffer, char* format, ...);
char* strf_s(char* buffer, size_t _Size_In_Bytes, char* format, ...);


#define DPOINT		CPOINT<double>
#define MAX			LONG_MAX
// end define



// クラス定義 (小関数・ストラクタのみ)

// 座標
template <class T=int> class CPOINT {
protected:
	T x, y;	// 座標

public:
	CPOINT operator=(DPOINT &dpt) { x = dpt.x; y = dpt.y; }
	CPOINT() { x = 0; y = 0; }
	void	set(T x, T y)		{ this->x = x; this->y = y; }
	T		xget()				{ return x; }
	void	xset(T x)			{ this->x = x; }
	T		yget()				{ return y; }
	void	yset(T y)			{ this->y = y; }
};

// 体力
class HP {
protected:
	int hp;		// 現在の値
	int hpmax;	// 最大の値
	int sub;	// 減少する値
	int add;	// 加算する値

public:
	HP() { hp = 0; hpmax = 0; sub = 0; add = 0; }
	void	set(int hp)			{ this->hp = hp; }
	int		get()				{ return hp; }
	void	maxset(int max)		{ this->hpmax = max; }
	int		maxget()			{ return hpmax; }
	void	asub()				{ hp -= sub; }
	void	subset(int sub)		{ this->sub = sub; }
	int		subget()			{ return sub; }
	void	aadd()				{ hp += add; }
	void	addset(int add)		{ this->add = add; }
	int		addget()			{ return add; }

};

// 基クラス (座標, フレーム, フラグ, 角度, 速度, 画像ID, 画像サイズx, y)
class BASE : public DPOINT {
private:
	DPOINT		p;				// 座標

protected:
	int			frame;			// 動いてからのフレーム
	char		flag;			// フラグ
	double		radius, degree;	// 半径(速度), 回転
	HBITMAP		hBmp;			// 画像用Bitmap
	HDC			hDC;			// 画像用DC
	CPOINT<>	size;			// 画像直径
	DPOINT		hsize;			// 画像半径
	CPOINT<>	div;			// 分割画像直径
	DPOINT		hdiv;			// 分割画像半径
	CPOINT<>	ndiv;			// 分割画像番号xy
	UINT		crClr;			// 画像透過色

public:
	// コンストラクタ
	BASE(){
		radius = 0;
		degree = 0;
		hDC = NULL;
		hBmp = NULL;
		crClr = 0;
	}
	// デストラクタ
	~BASE(){
		if (hDC == NULL) return;
		DeleteObject(hBmp);
		DeleteDC(hDC);
		hDC = NULL;
	}

	DPOINT	dpget()				{ p.set(x, y); return p; }
	void	frameAdd()			{ frame ++; }
	void	frameSet(int frame)	{ this->frame = frame; }
	int		frameGet()			{ return frame; }
	void	flagSet(int flag)	{ this->flag = flag; }
	int		flagGet()			{ return flag; }
	void	dset(double degree)	{ this->degree = degree; }
	double	dget()				{ return degree; }
	void	rset(double radius)	{ this->radius = radius; }
	double	rget()				{ return radius; }
	double	sxget()				{ return size.xget(); }
	double	syget()				{ return size.yget(); }
	double	hsxget()			{ return hsize.xget(); }
	double	hsyget()			{ return hsize.yget(); }
	void	crset(UINT crClr)	{ this->crClr = crClr; }
};

class BODY : public BASE, public HP {
public:
	// 計算
	virtual void calc() = 0;
	// 画像読み込み
	bool init(HINSTANCE hInst, HDC hDC, char* filename, LONG x = 0L, LONG y = 0L, COLORREF crClr = 0, LONG divx = 1, LONG divy = 1);
	// 描画 (透過色は透過)
	void draw(HDC hDC);
	// 半径を中心として描画 (透過色は透過)
	void hdraw(HDC hDC);
	// 分割半径を中心として分割して描画 (透過色は透過)
	void ddraw(HDC hDC);
};

class PLAYER : public BODY {
public:
	void calc();
};

class ENEMY : BODY{
};

class ITEM : public BASE{
};

enum SHOTING {S_P, S_E, S_MAX};
class STG{
protected:
	HINSTANCE	hInst;
	HWND		hWnd;
	HDC			hDC, hMDC;
	HBITMAP		hMBmp;
	HFONT		hFont;
	HBRUSH		hBrush;
	HPEN		hPen;

	PLAYER		player;
	ENEMY		*enemy;
	ITEM		*item;

	bool		redraw;		// 再描画ビット
	double		magnif;		// 倍率
	void draw();
	void inline validback(){
		BitBlt(hDC, 0, 0, ginfo(hDC, GI_X), ginfo(hDC, GI_Y),
			hMDC, 0, 0, SRCCOPY);
	}

	LONG		px, py;		// mes用座標
	char*		buf;		// mes用バッファポインタ
	size_t		_SizeInBytes;
	void inline pos(LONG px, LONG py){
		this->px = px;
		this->py = py;
	}
	void inline sbuf(char* text, size_t _SizeInBytes){
		buf = text;
		this->_SizeInBytes = _SizeInBytes;
	}
	void inline bmes(char* format, ...){
		va_list argptr;
		
		va_start(argptr, format);
		vsprintf_s(buf, _SizeInBytes, format, argptr);
		va_end(argptr);

		mes(buf);
	}
	void inline mes(char* text){
		pmes(px, py, text);
	}
	void inline pmes(LONG px, LONG py, char* text){
		SIZE size;
		GetTextExtentPoint32(hMDC, text, lstrlen(text), &size);

		RECT rc = {px, py, px+size.cx, py+size.cy};
		DrawText(hMDC, text, lstrlen(text), &rc, DT_WORDBREAK |
			DT_LEFT | DT_TOP | DT_EXPANDTABS);
		this->px = rc.left;
		this->py = rc.bottom;
	}

public:
	STG() {
		hInst = NULL;
		hWnd = NULL;
		hDC = NULL;
		hMDC = NULL;
		hMBmp = NULL;
		hFont = NULL;
		hBrush = NULL;
		hPen = NULL;
	}
	~STG(){ exit(); }
	bool init(HINSTANCE hInst, HWND hWnd);
	void exit();

	void calc();
	void redrawf(){ redraw = true; }

};

//end define



#endif
