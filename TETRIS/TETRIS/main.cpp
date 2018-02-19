
#include <windows.h>
#include "app.h"
#include "tetris.h"

// ウィンドウプロージャー
LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_DESTROY:
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

// アプリケーションメイン
bool WINDOW::App(){
	GameDraw(hMDC);
	return true;
}

// システムメイン
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	// クラス登録とウィンドウの作成
	CLASS c("TETRIS", WindowProc);
	WINDOW w(hInst, nCmd, "TETRIS", "TETRIS", 440);
	GameInit();

	w.MessageLoop();

	GameExit();
	return 0;
}

