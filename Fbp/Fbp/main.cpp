
//
// Falling block puzzle games
//

#include "Fbp.h"

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
	ChangeWindowMode(TRUE), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK); //�E�B���h�E���[�h�ύX�Ə������Ɨ���ʐݒ�

	// Fbp ������
	Initialize();

	while(ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0){
		// �v�Z
		Calc();

		// �`��
		Draw();
	}

	// Fbp �I������
	Exitialize();

	DxLib_End(); // DX���C�u�����I������
	return 0;
}  


