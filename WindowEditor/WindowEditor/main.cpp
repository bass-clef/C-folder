
#include <windows.h>
#include <stdio.h>
#include "app.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	MSG		msg;
	WNDCLASSEX wce;
	HWND	hWnd, hEdit;

	// �N���X�̍쐬�E�o�^
	if (!ClassInit(wce)) return 0;
	// �E�B���h�E�̍쐬�E�\��
	if (!WindowInit(wce.hInstance, hWnd, nCmd)) return 0;

	hEdit = CreateWindowEx(
		0, "LISTBOX", "", 0x54010141,
		0, 0, 640, 480,
		hWnd, NULL, hInst, NULL
	);
	if (!hEdit)return FALSE;
	ShowWindow(hEdit, nCmd);

	while(1) {
		Sleep(1);	// 1ms�ҋ@
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			// �V�X�e�����C��
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUIT�����Ă���
			TranslateMessage(&msg);	// �L�[�{�[�h�𗘗p�\�ɂ���
			DispatchMessage(&msg);	// �����Windows�ɖ߂�
		}else {

		}
	}

	return 0;
}

// �E�B���h�E�v���[�W���[
LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	PAINTSTRUCT ps;

	switch(uMsg){
	case WM_DESTROY:
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

