
#pragma once

// 追加リンク
#pragma comment(lib, "ws2_32.lib")

// 共通インクルード
#include <WinSock2.h>

// プロトタイプ宣言
BOOL ClassInit(WNDCLASSEX &wce, char* AppName, WNDPROC WndProc=DefWindowProc);
BOOL WindowInit(HINSTANCE hInstance, HWND &hWnd, int nCmd, char* AppName);
void __stdcall ErrorMessageBox();
void WatchWindow(HWND hWnd, HWND hTarget, LPDWORD &lpPixel);
void SetClientSize(HWND hWnd, int width, int height);

