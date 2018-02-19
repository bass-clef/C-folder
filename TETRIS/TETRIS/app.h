
#ifndef APPLICATION_H
#define APPLICATION_H
#include <windows.h>
#include <stdarg.h>

namespace {
	int pow(int x, int y) {
		int num = 0;
		for(int i = 0; i < y; i++) {
			num *= x;
		}
		return num;
	}
}

// コールバック関連
LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);

// ウィンドウ関連
BOOL ClassInit(WNDCLASSEX &wce, char* AppName = "static");
BOOL WindowInit(HINSTANCE hInstance, HWND &hWnd, int nCmd, char* AppName = "static");
int GetWindowsInfo(UINT uGetInfoType, HWND hWnd = NULL);

// デバイスコンテキスト関連
void CreateDCWithBitmap(HDC &hDC, HBITMAP &hbmp, int xSize, int ySize, HDC hDC2);
void CreateSelectBursh(HDC hDC, HBRUSH &hbr, COLORREF crColor);
void CreateSelectPen(HDC hDC, HPEN &hpen, COLORREF crColor);
void Print(HDC hDC, LPSTR Str, int xPos = 0, int yPos = 0);
void DrawLine(HDC hDC, int x1, int y1, int x2, int y2);
void DrawRect(HDC hDC, int x1, int y1, int x2, int y2, UINT DrawType);
void DrawCircle(HDC hDC, int x1, int y1, int x2, int y2, UINT DrawType);
void CopyDC(HDC hToDC, HDC hFromDC, DWORD dwrop);
// ~Unload
void DeleteDCWithBitmap(HDC hDC, HBITMAP hbmp);

// 文字列
char* strf(char* buffer, char* format, ...);

// contain private class
template <class type> class Array{
	type *t;
public:
	Array(int Len){t = new type[Len]; for(int i = 0; i < Len; i++){t[i] = 0;} }
	Array(void){t = NULL;}
	~Array(void){if (t != NULL) delete[] t;}
	void set(int Len){
		if (t == NULL) {
			t = new type[Len]; for(int i = 0; i < Len; i++){t[i] = 0;}
		}else {

		}
	}

	operator type(){return t;}
	type operator=(type i){return i;}

};
class CLASS{
	WNDCLASSEX	wce;

public:
	CLASS(char* AppName = "static", WNDPROC WindowProc = DefWindowProc, UINT style = CS_HREDRAW | CS_VREDRAW, HINSTANCE hInstance = NULL){
		wce.cbSize		= sizeof(wce);	// 構造体のsize
		wce.style		= style;	// スタイル
		wce.lpfnWndProc	= WindowProc;	// プロージャ関数
		wce.cbClsExtra	= wce.cbWndExtra = 0;
		wce.hInstance	= hInstance;	// プログラムのハンドル
		wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// アイコン
		wce.hCursor		= LoadCursor(NULL, IDC_ARROW);	// マウスカーソル
		wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// ブラシ
		wce.lpszMenuName	= NULL;	// メニュー
		wce.lpszClassName	= AppName;	// クラス名
		wce.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
		if (!RegisterClassEx(&wce)){
			MessageBox(NULL, "Not created class.\n\nexit now.", "Create error", MB_OK | MB_ICONERROR);
			exit(1);
		}
	}
};
class WINDOW{
	HWND	hWnd;			// ウィンドウハンドル
	HDC		hDC, hMDC;		// デバイスコンテキスト/メモリ
	HFONT	hFont;			// フォント
	HPEN	hPen;			// ペン
	HBRUSH	hBrush;			// ブラシ
	HBITMAP	hMBitmap;		// メモリ用ビットマップ

public:
	// コンス・デス トラクター
	WINDOW(HINSTANCE hInstance, int nCmd, char* ClassName,
		char* WindowName = "", int Width = 640, int Height = 480,
		int xPos = CW_USEDEFAULT, int yPos = CW_USEDEFAULT){
		hWnd = CreateWindowEx(
			0, ClassName, WindowName, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, Width, Height,
			NULL, NULL, hInstance, NULL
		);

		if (!hWnd){
			MessageBox(NULL, "Not created Window.\n\nexit now.", "Create error", MB_OK | MB_ICONERROR);
			exit(1);
		}

		ShowWindow(hWnd, nCmd);
		hDC = GetDC(hWnd);

		hMDC = CreateCompatibleDC(hDC);
		hMBitmap = CreateCompatibleBitmap(hDC, Width, Height);
		SelectObject(hMDC, hMBitmap);
		PatBlt(hMDC, 0, 0, Width, Height, WHITENESS);

		hFont = (HFONT)GetStockObject(OEM_FIXED_FONT);
		SelectObject(hDC, hFont);
		SelectObject(hMDC, hFont);

		hBrush = CreateSolidBrush(0);
		SelectObject(hDC, hBrush);
		SelectObject(hMDC, hBrush);

		hPen = CreatePen(PS_SOLID, 1, 0);
		SelectObject(hDC, hPen);
		SelectObject(hMDC, hPen);
	}
	~WINDOW(){
		DeleteObject(hPen);
		DeleteObject(hFont);
		DeleteObject(hBrush);
		DeleteObject(hMBitmap);
		DeleteDC(hMDC);
		ReleaseDC(hWnd, hDC);
	}

	// オーバーロード
	operator HDC(){return hMDC;}
	operator HWND(){return hWnd;}
	// 要オーバーロード
	bool App(void);

	// メンバー関数
	void MessageLoop(){
		MSG msg;
		while(1){
			Sleep(1);	// 1ms待機
			if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
				// システムメイン
				if (!GetMessage(&msg, NULL, 0, 0))break;// WM_QUITが来てたら
				TranslateMessage(&msg);	// キーボードを利用可能にする
				DispatchMessage(&msg);	// 制御をWindowsに戻す
			}else {
				if (App()) Repaint();
			}
		}

	}
	void Repaint(){
		BitBlt(hDC, 0, 0, GetDeviceCaps(hDC, HORZRES),
			GetDeviceCaps(hMDC, VERTRES), hMDC, 0, 0, SRCCOPY
		);
	}
};
class String{
	char *str;
	int size;

public:
	// コンス・デス トラクター
	String(int Len = 0){
		size = NULL;
		allocate(Len);
	}
	String(char* ini){
		size = NULL;
		allocate(lstrlen(ini) + 1);
		lstrcpy(str, ini);
	}
	~String(){if (str != NULL) delete[] str;}

	// オーバーロード 同型
	operator char*(){return str;}
	operator String&(){return *this;}
	String& operator=(char* add){
		if (size <= lstrlen(add)) realloc(lstrlen(add) + 1);
		lstrcpy(str, add);
		return *this;
	}
	void operator+=(char* add){
		if (size <= (lstrlen(str) + lstrlen(add)))
			realloc(lstrlen(str) + lstrlen(add) + 1);
		lstrcat(str, add);
	}
	bool operator==(char* cmp){
		if (lstrlen(str) != lstrlen(cmp)) return false;
		bool f = 1;
		for(int i = 0; (f) && (lstrlen(str) > i); i++)
			if (str[i] != cmp[i]) f = 0;
		return f;
	}
	bool operator!=(char* cmp){
		return (!String::operator==(cmp));
	}
	void operator++(int){realloc(size+1);}
	// 他型
	operator int(){
		int num = 0, f = 0;
		for(int i = 0; i < lstrlen(str); i++){
			if ('0' <= str[i] && str[i] <= '9') f = 1;
			if (f) {
				if (!('0' <= str[i] && str[i] <= '9')) break;
				num += pow(str[i]-'0', f);
				if ('0' < str[i]) f ++;
			}
		}
		return num;
	}
	String& operator=(int num){
		char *dest = new char[11];
		if (size <= (lstrlen(str) + lstrlen(dest)))
			realloc(lstrlen(str) + lstrlen(dest) + 1);
		strf(dest, "%d", num);
		lstrcpy(str, dest);
		delete[] dest;
		return *this;
	}
	void operator+=(int add){
		char *dest = new char[11];
		strf(dest, "%d", add);
		if (size <= (lstrlen(str) + lstrlen(dest)))
			realloc(lstrlen(str) + lstrlen(dest) + 1);
		lstrcat(str, dest);
		delete[] dest;
	}

	// メンバー関数
	void allocate(int Len){
		if (size != NULL) {
			String::realloc(Len); 
			return;
		}
		str = new char[Len];
		for(int i = 0; i < Len; i++){str[i] = 0;}
		size = Len;
	}
	void realloc(int Len){
		if (size == NULL) {
			String::allocate(Len);
			return;
		}
		if (Len == NULL) Len = 1;
		size = Len;

		char *dest = new char[size];
		lstrcpy(dest, str);
		delete[] str;

		str = new char[size];
		lstrcpy(str, dest);

		delete[] dest;
	}
	int memsize(void){return size;}
	void print(HDC hDC, int cx, int cy){
		SIZE size;
		GetTextExtentPoint32(hDC, str, lstrlen(str), &size);

		RECT rc = {cx, cy, size.cx, size.cy};
		DrawText(hDC, str, lstrlen(str), &rc, DT_NOCLIP | DT_WORDBREAK |
			DT_LEFT | DT_TOP | DT_EXPANDTABS);
	}
};

// definition macro
#define Gwi					GetWindowsInfo
#define	GWSI_DESKTOPX		1
#define	GWSI_DESKTOPY		2

#define DR_FRAME			1
#define DR_FILL				2
#define DR_GRADATION		3
#define DR_CFRAME			4
#define DR_CFILL			5

#define DC_FRAME			1
#define DC_FILL				2
#define DC_GRADATION		3

#endif
