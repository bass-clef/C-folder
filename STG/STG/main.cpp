

#include "define.h"




LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	if (uMsg == WM_DESTROY) {PostQuitMessage(0); return 0; }
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){
	HWND	hWnd;

	{/// クラス・ウィンドウ の作成
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
	}

	// 初期化
	STG		stg;
	stg.init(hInst, hWnd);
	if (e.nget()){
		char error_str[256];
		wsprintf(error_str, "予期していたエラーが発生しました。\n\
			error number is %d.\nline is %d\n\
			終了します。", e.nget(), e.lget());
		MessageBox(NULL, error_str, "error.", MB_OK);
		return 0;
	}

	MSG		msg;
	while(1) {
		now_time = timeGetTime() - start_time;
		Sleep(1);
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			// システムメイン
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUITが来てたら
			TranslateMessage(&msg);	// キーボードを利用可能にする
			DispatchMessage(&msg);	// 制御をWindowsに戻す
		}else if (now_time > 1000/FPS) {
			start_time = timeGetTime();
			stg.calc();
		}
		
	}

	return 0;
}
