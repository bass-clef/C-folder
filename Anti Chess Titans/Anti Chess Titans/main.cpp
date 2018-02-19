
#include <Windows.h>
#include <string>

using namespace std;
namespace {
	char AppName[] = "Anti Chess Titans";
}

void __stdcall ErrorMessageBox(){
	LPVOID lpMsgBuf;

	FormatMessage(//エラー表示文字列作成
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR)&lpMsgBuf, 0, NULL
	);

	MessageBox(NULL, (const char*)lpMsgBuf, NULL, MB_OK);	//メッセージ表示
	LocalFree(lpMsgBuf);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_DESTROY: PostQuitMessage(0); return 0;
	case WM_LBUTTONDOWN:
		if (HIWORD(lp) <= 20) SendMessage(hWnd, WM_NCLBUTTONDOWN, 2, 0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	const int xsize = 8, ysize = 8;
	HWND hWnd, hCTWnd = ::FindWindowA(NULL, "Chess Titans");
	int nx = 50, ny = 50;
	char str[128];

	#pragma region /// Chess Titans
	if (hCTWnd == NULL) {
		::MessageBoxA(NULL, "Chess Titans が起きていません。", "エラー", MB_OK);
		return 0;
	}

	RECT rc;
	GetWindowRect(hCTWnd, &rc);
//	wsprintf(str, "Location : %d x %d\nField size : %d x %d", rc.left, rc.top, );
	::MessageBoxA(NULL, str, "", MB_OK);
	#pragma endregion

	// クライアントエリアサイズ
	RECT wndrc = { 0, 0, nx*xsize, ny*ysize };

	#pragma region /// クラス・ウィンドウ の作成
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
	if (! RegisterClassEx(&wce)){ ErrorMessageBox(); return 1; }

	hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST,
		AppName, AppName,
		WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right, rc.bottom,
		NULL, NULL, NULL, NULL
	);
	if (hWnd == NULL) { ErrorMessageBox(); return 1; }
//	SetLayeredWindowAttributes(hWnd, 0xFFFFFF, 255, LWA_COLORKEY);
	#pragma endregion

	int i = 0;
	i += 0xF;
	i += 0xFF;
	i += 0xFFF;
	i += 0xFFFF;
	i += 0xFFFFF;
	i += 0xFFFFFF;
	i += 0xFFFFFFF;
	i += 0xFFFFFFFF;
	i -= 0xF;
	i -= 0xFF;
	i -= 0xFFF;
	i -= 0xFFFF;
	i -= 0xFFFFF;
	i -= 0xFFFFFF;
	i -= 0xFFFFFFF;
	i -= 0xFFFFFFFF;
	i *= 0xF;
	i *= 0xFF;
	i *= 0xFFF;
	i *= 0xFFFF;
	i *= 0xFFFFF;
	i *= 0xFFFFFF;
	i *= 0xFFFFFFF;
	i *= 0xFFFFFFFF;
	i /= 0xF;
	i /= 0xFF;
	i /= 0xFFF;
	i /= 0xFFFF;
	i /= 0xFFFFF;
	i /= 0xFFFFFF;
	i /= 0xFFFFFFF;
	i /= 0xFFFFFFFF;
	i = 0xF;
	i = 0xFF;
	i = 0xFFF;
	i = 0xFFFF;
	i = 0xFFFFF;
	i = 0xFFFFFF;
	i = 0xFFFFFFF;
	i = 0xFFFFFFFF;

	MSG msg;
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

	return 0;
}