
#include <windows.h>
#include <stdio.h>
#include "app.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	MSG		msg;
	WNDCLASSEX wce;
	HWND	hWnd, hEdit;

	// クラスの作成・登録
	if (!ClassInit(wce)) return 0;
	// ウィンドウの作成・表示
	if (!WindowInit(wce.hInstance, hWnd, nCmd)) return 0;

	hEdit = CreateWindowEx(
		0, "LISTBOX", "", 0x54010141,
		0, 0, 640, 480,
		hWnd, NULL, hInst, NULL
	);
	if (!hEdit)return FALSE;
	ShowWindow(hEdit, nCmd);

	while(1) {
		Sleep(1);	// 1ms待機
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			// システムメイン
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUITが来てたら
			TranslateMessage(&msg);	// キーボードを利用可能にする
			DispatchMessage(&msg);	// 制御をWindowsに戻す
		}else {

		}
	}

	return 0;
}

// ウィンドウプロージャー
LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	PAINTSTRUCT ps;

	switch(uMsg){
	case WM_DESTROY:
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

