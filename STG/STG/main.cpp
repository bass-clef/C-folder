

#include "define.h"




LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	if (uMsg == WM_DESTROY) {PostQuitMessage(0); return 0; }
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){
	HWND	hWnd;

	{/// �N���X�E�E�B���h�E �̍쐬
	WNDCLASSEX wce;
	wce.cbSize		= sizeof(wce);	// �\���̂�size
	wce.style		= CS_HREDRAW | CS_VREDRAW;	// �X�^�C��
	wce.lpfnWndProc	= WindowProc;	// �v���[�W���֐�
	wce.cbClsExtra	= wce.cbWndExtra = 0;
	wce.hInstance	= hInst;		// �v���O�����̃n���h��
	wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// �A�C�R��
	wce.hCursor		= LoadCursor(NULL, IDC_ARROW);		// �}�E�X�J�[�\��
	wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// �u���V
	wce.lpszMenuName	= NULL;		// ���j���[
	wce.lpszClassName	= AppName;	// �N���X��
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

	// ������
	STG		stg;
	stg.init(hInst, hWnd);
	if (e.nget()){
		char error_str[256];
		wsprintf(error_str, "�\�����Ă����G���[���������܂����B\n\
			error number is %d.\nline is %d\n\
			�I�����܂��B", e.nget(), e.lget());
		MessageBox(NULL, error_str, "error.", MB_OK);
		return 0;
	}

	MSG		msg;
	while(1) {
		now_time = timeGetTime() - start_time;
		Sleep(1);
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			// �V�X�e�����C��
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUIT�����Ă���
			TranslateMessage(&msg);	// �L�[�{�[�h�𗘗p�\�ɂ���
			DispatchMessage(&msg);	// �����Windows�ɖ߂�
		}else if (now_time > 1000/FPS) {
			start_time = timeGetTime();
			stg.calc();
		}
		
	}

	return 0;
}
