
// Identified microbe -確認微生物-

#include <windows.h>
#include "define.h"

#include <stdio.h>

WNDPROC dsp;

LRESULT CALLBACK StaticProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	printf("0x%04X\n", uMsg);
	return CallWindowProc(dsp, hWnd, uMsg, wp, lp);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

//int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){
int main(){
	HWND	hWnd, hEdit;
	MSG		msg;

	WNDCLASSEX wce;
		wce.cbSize		= sizeof(wce);	// 構造体のsize
		wce.style		= CS_HREDRAW | CS_VREDRAW;	// スタイル
		wce.lpfnWndProc	= WindowProc;	// プロージャ関数
		wce.cbClsExtra	= wce.cbWndExtra = 0;
		wce.hInstance	= NULL;	// プログラムのハンドル
		wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// アイコン
		wce.hCursor		= LoadCursor(NULL, IDC_ARROW);	// マウスカーソル
		wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// ブラシ
		wce.lpszMenuName	= NULL;	// メニュー
		wce.lpszClassName	= "test";	// クラス名
		wce.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
		if (!RegisterClassEx(&wce)){
			MessageBox(NULL, "Not created class.\n\nexit now.", "Create error", MB_OK | MB_ICONERROR);
			exit(1);
		}

	hWnd = CreateWindowEx(0, 
			"test", "test",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, NULL, NULL
	);
	if (hWnd == NULL) return -1;
	ShowWindow(hWnd, SW_SHOW);

	hEdit = CreateWindowEx(0, "EDIT", "",
		WS_CHILD | WS_VISIBLE,
		0, 0, 300, 20, hWnd, NULL, NULL, NULL);
	ShowWindow(hEdit, SW_SHOW);

	dsp = (WNDPROC)GetWindowLong(hEdit, GWL_WNDPROC);
	printf("DefStaticWindowProcAddress[%d]", dsp);
	SetWindowLong(hEdit, GWL_WNDPROC, (LONG)StaticProc);

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

	SetWindowLong(hEdit, GWL_WNDPROC, (LONG)dsp);

	return 0;
}
