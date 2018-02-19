//// _base_include_
#include <Windows.h>
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <stdarg.h>

#pragma comment(lib, "winmm.lib")

void __stdcall ErrorMessageBox(){
LPVOID lpMsgBuf;

FormatMessage(//エラー表示文字列作成
FORMAT_MESSAGE_ALLOCATE_BUFFER | 
FORMAT_MESSAGE_FROM_SYSTEM | 
FORMAT_MESSAGE_IGNORE_INSERTS,
NULL, GetLastError(),
MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
(LPTSTR)&lpMsgBuf, 0, NULL
);

MessageBox(NULL, (const char*)lpMsgBuf, NULL, MB_OK);	//メッセージ表示
LocalFree(lpMsgBuf);
}

//// _base_classese_and_defines_
typedef void * PDAT;							// データの実態へのポインタ
typedef int APTR;								// 配列データへのオフセット値
typedef struct{
//	Memory Data structure (2.5 compatible)
//
short	flag;		// type of val
short	mode;		// mode (0=normal/1=clone/2=alloced)
int		len[5];		// length of array 4byte align (dim)
int		size;		// size of Val
char	*pt;		// ptr to array

//	Memory Data structure (3.0 compatible)
//
void	*master;			// Master pointer for data
unsigned short	support;	// Support Flag
short	arraycnt;			// Array Set Count
int		offset;				// Array Data Offset
int		arraymul;			// Array Multiple Value 
} PVal;
typedef struct HSP3VARSET { 
//  PVal entry structure 
// 
int type; 
PVal *pval; 
APTR aptr; 
void *ptr; 
} HSP3VARSET; 
typedef struct HSPOBJINFO { 
//      Object Info (3.0) 
// 
short   owmode;     // objectのmode 
short   option;     // objectのoption(未使用・内部オブジェクトは0) 
void    *bm;        // objectが配置されているBMSCR構造体のポインタ 
HWND    hCld;       // objectのhandle 
int     owid;       // objectのValue(汎用) 
int     owsize;     // objectの使用サイズ(汎用) 
HSP3VARSET varset;  // objectから設定される変数の情報 
//      callback function 
void    (*func_notice)( struct HSPOBJINFO *, int ); 
void    (*func_objprm)( struct HSPOBJINFO *, int, void * ); 
void    (*func_delete)( struct HSPOBJINFO * ); 
} HSPOBJINFO;
#define BMSCR_FLAG_NOUSE	0
#define BMSCR_FLAG_INUSE	1
#define BMSCR_PALMODE_FULLCOLOR	0
#define BMSCR_PALMODE_PALETTECOLOR	1
#define HSPWND_TYPE_NONE 0
#define HSPWND_TYPE_BUFFER 1
#define HSPWND_TYPE_MAIN 2
#define HSPWND_TYPE_BGSCR 3
enum {
BMSCR_SAVEPOS_MOSUEX,
BMSCR_SAVEPOS_MOSUEY,
BMSCR_SAVEPOS_MOSUEZ,
BMSCR_SAVEPOS_MOSUEW,
BMSCR_SAVEPOS_MAX,
};
class BMSCR{
public:
//
//		Window data structure
//
int		flag;			// used flag
int		sx;			// X-size
int		sy;			// Y-size
int		palmode;		// palmode
HDC		hdc;			// buffer HDC
BYTE		*pBit;			// bitmap pointer
BITMAPINFOHEADER *pbi;			// infoheader
HBITMAP		dib;			// bitmap handle(DIB)
HBITMAP		old;			// bitmap handle(OLD)
RGBQUAD 	*pal;			// palette table
HPALETTE 	hpal;			// palette handle
HPALETTE 	holdpal;		// palette handle (old)
int		pals;			// palette entries
HWND		hwnd;			// window handle
HINSTANCE	 hInst;			// Instance of program
int		infsize;		// *pbi alloc memory size
int		bmpsize;		// *pBit alloc memory size

//		Window object setting
//
int		type;			// setting type
int		wid;			// window ID
short		fl_dispw;		// display window flag
short		fl_udraw;		// update draw window
int		wx,wy,wchg;		// actual window size x,y
int		viewx,viewy;		// buffer view point x,y
int		lx,ly;			// buffer view size x,y
int		cx,cy;			// object cursor x,y
int		ox,oy,py;		// object size x,y,py
int		texty;			// text Y-axis size
int		gx,gy,gmode;		// gcopy size
HBRUSH		hbr;			// BRUSH handle
HPEN		hpn;			// PEN handle
HFONT		hfont;			// FONT handle
HFONT		holdfon;		// FONT handle (old)
COLORREF	color;			// text color code
int		textspeed;		// slow text speed
int		cx2,cy2;		// slow text cursor x,y
int		tex,tey;		// slow text limit x,y
char		*prtmes;		// slow message ptr
int		focflg;			// focus set flag
int		objmode;		// object set mode
LOGFONT		logfont;		// logical font
int		style;			// extra window style
int		gfrate;			// halftone copy rate
int		tabmove;		// object TAB move mode
int		sx2;			// actual bitmap X size
SIZE		printsize;		// print,mes extent size

//		Class depend data
//
int		objstyle;		// objects style
HSPOBJINFO	*mem_obj;		// Window objects
int 		objmax;			// Max number of obj
int 		objlimit;		// Limit number of obj
short savepos[BMSCR_SAVEPOS_MAX];	// saved position
void 		*master_hspwnd;		// Parent hspwnd class

~BMSCR(){
if (dib != NULL) { DeleteObject(dib); dib = NULL; }
if (hdc != NULL) { DeleteDC(hdc); hdc = NULL; }
if (hbr != NULL) { DeleteObject(hbr); hbr = NULL; }
if (hpn != NULL) { DeleteObject(hpn); hpn = NULL; }
if (hfont != NULL) { DeleteObject(hfont); hfont = NULL; }
}
};


template <class T> class vectors {
public:
std::vector<T*> p;
size_t size;

vectors(){
size = 0;
}
~vectors(){
while(size){
pop();
}
}
void push(T *p){
this->p.push_back(p);
size++;
}
void pop(){
delete p.back();
this->p.pop_back();
size--;
}
};

//// _base_hsp_
namespace hsp {
// classes
class str : public std::string, public std::ostringstream {
public:
str(char* str){
this->assign(str);
}
str& operator=(int num){
char str[9];
wsprintf(str, "%d", num);
this->assign(str);
return *this;
}
str& operator=(const char *str){
this->assign(str);
return *this;
}
str& operator+=(int num){
char str[9];
wsprintf(str, "%d", num);
this->append(str);
return *this;
}
str& operator+=(const char* str){
this->append(str);
return *this;
}
str& operator+(int num){
char str[9];
wsprintf(str, "%d", num);
this->append(str);
return *this;
}
str& operator+(const char* str){
this->append(str);
return *this;
}
template<class T> str& operator+(T any){
this->operator<<(any);
this->append(this->std::ostringstream::str());
return *this;
}
};

// variables
vectors<BMSCR> bmscr_stack;
std::stack<int> repeat_nest;
std::stack<int> repeat_cntmax_nest;

int cnt = -1, cntmax = 0;
int stat = 0;
hsp::str refstr = "", tmpstr = "";
double refdval = 0.0;

HINSTANCE hInstance = NULL;
MSG msg;
DWORD time, old;
int selected_window = -1;
void *p;

// functions
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wparam, LPARAM lparam){
switch(uMsg){
case WM_DESTROY: PostQuitMessage(0); return 0;
}
return DefWindowProc(hWnd, uMsg, wparam, lparam);
}
inline int ginfo(int type){
switch(type){
case  3: return selected_window;
case 16: return GetRValue(bmscr_stack.p.at(selected_window)->color);
case 17: return GetGValue(bmscr_stack.p.at(selected_window)->color);
case 18: return GetBValue(bmscr_stack.p.at(selected_window)->color);
case 20: return ::GetSystemMetrics(SM_CXSCREEN);
case 21: return ::GetSystemMetrics(SM_CYSCREEN);
}
return 0;
}
inline HGDIOBJ objget(int type){
switch(type){
case  0: return bmscr_stack.p.at(selected_window)->hwnd;
case  1: return bmscr_stack.p.at(selected_window)->hdc;
case  2: return bmscr_stack.p.at(selected_window)->hInst;
}
return NULL;
}
inline char* strf(char *format, ...){
va_list argptr;

va_start(argptr, format);
UINT ui = _vscprintf(format, argptr)+1;
hsp::tmpstr.resize(ui);
memset((char*)hsp::tmpstr.c_str(), 0, ui-1);
vsprintf_s((char*)hsp::tmpstr.c_str(), ui, format, argptr);
va_end(argptr);

return (char*)hsp::tmpstr.c_str();
}
void init(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){
hInstance = hInst;
}
void getkey(int &var, int keycode){
var = ::GetAsyncKeyState(keycode) & 0x8000;
}
void mouse(int x, int y){
INPUT input[1];
input[0].type = INPUT_MOUSE;
input[0].mi.mouseData = 0, 0, 0;
input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
input[0].mi.dx = (DWORD)x * WORD(-1) / ::GetSystemMetrics(SM_CXSCREEN);
input[0].mi.dy = (DWORD)y * WORD(-1) / ::GetSystemMetrics(SM_CYSCREEN);
SendInput(1, input, sizeof(INPUT));
}
void screen(int wndid = 0, int width = 640, int height = 480, int mode = 0,
int px = CW_USEDEFAULT, int py = CW_USEDEFAULT,
int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT){
WNDCLASSEX wce;
wce.cbSize		= sizeof(wce);	// 構造体のsize
wce.style		= CS_HREDRAW | CS_VREDRAW;	// スタイル
wce.lpfnWndProc	= hsp::WndProc;	// プロージャ関数
wce.cbClsExtra	= wce.cbWndExtra = 0;
wce.hInstance	= hInstance;		// プログラムのハンドル
wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// アイコン
wce.hCursor		= LoadCursor(NULL, IDC_ARROW);		// マウスカーソル
wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// ブラシ
wce.lpszMenuName	= NULL;		// メニュー

char classname[16];
wsprintf(classname, "hspwnd%d", bmscr_stack.size);
wce.lpszClassName	= classname;	// クラス名
wce.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
if (! ::RegisterClassExA(&wce)) { ErrorMessageBox(); return; }

p = new BMSCR;
memset(p, 0, sizeof(BMSCR));
((BMSCR*)p)->style = 
WS_OVERLAPPEDWINDOW-WS_THICKFRAME-WS_MAXIMIZEBOX |
WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
WS_BORDER | WS_DLGFRAME | WS_GROUP;
((BMSCR*)p)->hwnd = CreateWindowEx(0x100,
classname, "", ((BMSCR*)p)->style,
px, py, width, height, NULL, NULL, hInstance, NULL);
if (((BMSCR*)p)->hwnd == NULL) { ErrorMessageBox(); return; }

((BMSCR*)p)->flag = BMSCR_FLAG_INUSE;
((BMSCR*)p)->sx = width;
((BMSCR*)p)->sy = height;
if (mode != 1) ((BMSCR*)p)->palmode = BMSCR_PALMODE_FULLCOLOR;

HDC hDC = GetDC(((BMSCR*)p)->hwnd);
((BMSCR*)p)->hdc = ::CreateCompatibleDC(hDC);
((BMSCR*)p)->dib = ::CreateCompatibleBitmap(hDC, width, height);
SelectObject(((BMSCR*)p)->hdc, ((BMSCR*)p)->dib);
PatBlt(((BMSCR*)p)->hdc, 0, 0, width, height, WHITENESS);
ReleaseDC(((BMSCR*)p)->hwnd, hDC);

((BMSCR*)p)->hInst = hInstance;
((BMSCR*)p)->type = HSPWND_TYPE_MAIN;
((BMSCR*)p)->wid = wndid;
((BMSCR*)p)->fl_dispw = true;
((BMSCR*)p)->fl_udraw = true;
((BMSCR*)p)->ox = 100;
((BMSCR*)p)->oy = 25;
((BMSCR*)p)->py = 18;
((BMSCR*)p)->gmode = 1;
((BMSCR*)p)->hbr = ::CreateSolidBrush(0);
SelectObject(((BMSCR*)p)->hdc, ((BMSCR*)p)->hbr);
((BMSCR*)p)->hpn = ::CreatePen(PS_SOLID, 1, 0);
SelectObject(((BMSCR*)p)->hdc, ((BMSCR*)p)->hpn);
((BMSCR*)p)->hfont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
SelectObject(((BMSCR*)p)->hdc, ((BMSCR*)p)->hfont);
((BMSCR*)p)->master_hspwnd = &bmscr_stack;

bmscr_stack.push(((BMSCR*)p));
selected_window = wndid;
}
void await(unsigned int ms){
time = GetTickCount()+ms;
while(time >= GetTickCount()) 
if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
// システムメイン
if (!GetMessage(&msg, NULL, 0, 0)) {// WM_QUITが来てたら
exit(0);
break;
}
TranslateMessage(&msg);	// キーボードを利用可能にする
DispatchMessage(&msg);	// 制御をWindowsに戻す
}
}
void wait(unsigned int ms){
time = GetTickCount()+ms;
while(time >= GetTickCount()) {
Sleep(1);
if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
// システムメイン
if (!GetMessage(&msg, NULL, 0, 0)) {// WM_QUITが来てたら
exit(0);
break;
}
TranslateMessage(&msg);	// キーボードを利用可能にする
DispatchMessage(&msg);	// 制御をWindowsに戻す
}
}
}
void stop(){
while(1) if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
// システムメイン
if (!GetMessage(&msg, NULL, 0, 0)) {// WM_QUITが来てたら
exit(0);
break;
}
TranslateMessage(&msg);	// キーボードを利用可能にする
DispatchMessage(&msg);	// 制御をWindowsに戻す
}else Sleep(1);
}
void title(const char *text){
SetWindowText(bmscr_stack.p.at(selected_window)->hwnd, text);
}
void repeat(int countmax = -1, int index = 0){
if (cnt != -1) {
hsp::repeat_nest.push(cnt);
hsp::repeat_cntmax_nest.push(cntmax);
}
cnt = index;
cntmax = countmax;
}
void loop(){
if (hsp::repeat_nest.size()) {
cnt = hsp::repeat_nest.top();
hsp::repeat_nest.pop();
cntmax = hsp::repeat_cntmax_nest.top();
hsp::repeat_cntmax_nest.pop();
}
}
void gsel(unsigned int wndid, int mode = 0){
switch(mode) {
case -1: ShowWindow(bmscr_stack.p.at(wndid)->hwnd, SW_HIDE);
break;
case  0: selected_window = wndid; break;
case  1: SetActiveWindow(bmscr_stack.p.at(wndid)->hwnd); break;
case  2:
SetWindowPos(bmscr_stack.p.at(wndid)->hwnd, HWND_TOPMOST, 
0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOMOVE |
SWP_NOOWNERZORDER | SWP_NOSIZE);
break;
}
}
void invalidate(int x, int y, int width, int height){
HDC hDC = GetDC(bmscr_stack.p.at(selected_window)->hwnd);
BitBlt(hDC, x, y,
width,
height,
bmscr_stack.p.at(selected_window)->hdc, x, y, SRCCOPY);
ReleaseDC(bmscr_stack.p.at(selected_window)->hwnd, hDC);
}
void redraw(short mode, int x = 0, int y = 0, int width = CW_USEDEFAULT, int height = CW_USEDEFAULT){
bmscr_stack.p.at(selected_window)->fl_udraw = mode;
if (bmscr_stack.p.at(selected_window)->fl_udraw) {
invalidate(x, y,
width == CW_USEDEFAULT ? bmscr_stack.p.at(selected_window)->sx : width,
height == CW_USEDEFAULT ? bmscr_stack.p.at(selected_window)->sy : height
);
//			InvalidateRect(bmscr_stack.p.at(selected_window)->hwnd, NULL, FALSE);
}
}
void color(short r, short g, short b){
bmscr_stack.p.at(selected_window)->color = RGB(r, g, b);
::SetDCBrushColor(bmscr_stack.p.at(selected_window)->hdc,
bmscr_stack.p.at(selected_window)->color);
::SetDCPenColor(bmscr_stack.p.at(selected_window)->hdc,
bmscr_stack.p.at(selected_window)->color);
::SetTextColor(bmscr_stack.p.at(selected_window)->hdc,
bmscr_stack.p.at(selected_window)->color);
::SetBkColor(bmscr_stack.p.at(selected_window)->hdc,
bmscr_stack.p.at(selected_window)->color);
::SetBkMode(bmscr_stack.p.at(selected_window)->hdc, TRANSPARENT);
}
void pos(int cx, int cy){
bmscr_stack.p.at(selected_window)->cx = cx;
bmscr_stack.p.at(selected_window)->cy = cy;
}
void mes(const char* text){
UINT size = lstrlen(text);
GetTextExtentPoint32( bmscr_stack.p.at(selected_window)->hdc,
text, size,
&bmscr_stack.p.at(selected_window)->printsize );
RECT rc = {
bmscr_stack.p.at(selected_window)->cx,
bmscr_stack.p.at(selected_window)->cy,
bmscr_stack.p.at(selected_window)->cx
+ bmscr_stack.p.at(selected_window)->printsize.cx,
bmscr_stack.p.at(selected_window)->cy
};

bmscr_stack.p.at(selected_window)->printsize.cy
+= DrawTextA( bmscr_stack.p.at(selected_window)->hdc, text,
size, &rc, DT_WORDBREAK | DT_LEFT | DT_TOP | DT_EXPANDTABS
| DT_CALCRECT);

bmscr_stack.p.at(selected_window)->printsize.cy = (rc.bottom - rc.top);
bmscr_stack.p.at(selected_window)->py
= bmscr_stack.p.at(selected_window)->printsize.cy;
bmscr_stack.p.at(selected_window)->cy
+= bmscr_stack.p.at(selected_window)->py;

DrawText( bmscr_stack.p.at(selected_window)->hdc, text,
size, &rc, DT_WORDBREAK | DT_LEFT | DT_TOP | DT_EXPANDTABS );
if (bmscr_stack.p.at(selected_window)->fl_udraw) redraw(1);
}
};

//// _base_namespace_
//// _base_function_
//// _base_main_
