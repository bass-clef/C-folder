
#pragma once

// �ǉ������N
#pragma comment(lib, "ws2_32.lib")

// ���ʃC���N���[�h
#include <WinSock2.h>

// �v���g�^�C�v�錾
BOOL ClassInit(WNDCLASSEX &wce, char* AppName, WNDPROC WndProc=DefWindowProc);
BOOL WindowInit(HINSTANCE hInstance, HWND &hWnd, int nCmd, char* AppName);
void __stdcall ErrorMessageBox();
void WatchWindow(HWND hWnd, HWND hTarget, LPDWORD &lpPixel);
void SetClientSize(HWND hWnd, int width, int height);

