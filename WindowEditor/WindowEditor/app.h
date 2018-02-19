
#ifndef APPLICATION_H
#define APPLICATION_H
#include <windows.h>
#include <stdarg.h>

// コールバック関連
LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);

// ウィンドウ関連
BOOL ClassInit(WNDCLASSEX &wce);
BOOL WindowInit(HINSTANCE hInstance, HWND &hWnd, int nCmd);
int GetWindowsInfo(UINT uGetInfoType);

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
char* Strf(char* buffer, char* format, ...);

// definition macro
#define Gwi					GetWindowsInfo
#define	GWSI_DESKTOPX		1
#define	GWSI_DESKTOPY		2

#define DR_FRAME			1
#define DR_FILL				2
#define DR_GRADATION		3

#define DC_FRAME			1
#define DC_FILL				2
#define DC_GRADATION		3

#endif
