
#include <windows.h>
#include "app.h"
#include "tetris.h"

// �E�B���h�E�v���[�W���[
LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_DESTROY:
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

// �A�v���P�[�V�������C��
bool WINDOW::App(){
	GameDraw(hMDC);
	return true;
}

// �V�X�e�����C��
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	// �N���X�o�^�ƃE�B���h�E�̍쐬
	CLASS c("TETRIS", WindowProc);
	WINDOW w(hInst, nCmd, "TETRIS", "TETRIS", 440);
	GameInit();

	w.MessageLoop();

	GameExit();
	return 0;
}

