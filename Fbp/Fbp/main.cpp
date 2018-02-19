
//
// Falling block puzzle games
//

#include "Fbp.h"

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
	ChangeWindowMode(TRUE), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK); //ウィンドウモード変更と初期化と裏画面設定

	// Fbp 初期化
	Initialize();

	while(ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0){
		// 計算
		Calc();

		// 描画
		Draw();
	}

	// Fbp 終了処理
	Exitialize();

	DxLib_End(); // DXライブラリ終了処理
	return 0;
}  


