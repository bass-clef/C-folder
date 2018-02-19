
#include "definition.h"

// クラス作成
BOOL ClassInit(WNDCLASSEX &wce, char* AppName, WNDPROC WndProc){

	wce.cbSize		= sizeof(wce);	// 構造体のsize
	wce.style		= CS_HREDRAW | CS_VREDRAW;	// スタイル
	wce.lpfnWndProc	= WndProc;	// プロージャ関数
	wce.cbClsExtra	= wce.cbWndExtra = 0;
	wce.hInstance	= NULL;	// プログラムのハンドル
	wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// アイコン
	wce.hCursor		= LoadCursor(NULL, IDC_ARROW);	// マウスカーソル
	wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// ブラシ
	wce.lpszMenuName	= NULL;	// メニュー
	wce.lpszClassName	= AppName;	// クラス名
	wce.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wce))return FALSE;

	return TRUE;
}

// ウィンドウ作成・表示
BOOL WindowInit(HINSTANCE hInstance, HWND &hWnd, int nCmd, char* AppName){
	
	hWnd = CreateWindowEx(
		0, AppName, AppName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
		NULL, NULL, hInstance, NULL
	);
	if (!hWnd) return FALSE;
	ShowWindow(hWnd, nCmd);

	return TRUE;
}

// エラーメッセージ表示
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


class SELWNDINFO{
public:
	HWND hWnd;
	HDC hDC, hMemDC;
	BITMAPINFO bmi;
	HBITMAP hBmp;
	RECT rc;
	int width, height;

	void GethDC(HWND hWnd){
		hDC = ::GetDC(hWnd);
	}

	void ReleasehDC(HWND hWnd){
		ReleaseDC(hWnd, hDC);
	}

	void CreatehDIBSection(LPBITMAPINFO lpbmi, UINT usage, void** ppvBits){
		hBmp = ::CreateDIBSection(hDC, lpbmi, usage, ppvBits, NULL, 0);
	}

	void CreateCompatiblehhMemDC(){
		hMemDC = CreateCompatibleDC(hDC);
	}

	void SelectObjecthMemDChBmp(){
		SelectObject(hMemDC, hBmp);
	}

	void CopyDC(DWORD rop){
		BitBlt(hMemDC, 0, 0, width, height, hDC, 0, 0, rop);
	}

	void SetWndSize(int width, int height){
		this->width = width;
		this->height = height;
	}

	void SetMainWnd(HWND hWnd){
		this->hWnd = hWnd;
	}

	template <class T> void GetWndSize(T width, T height){
		width = (T)this->width;
		height = (T)this->height;
	}
};

namespace {
	SELWNDINFO wi;
	BITMAPINFO bmi;
}

// デスクトップbits取得
void WatchWindow(HWND hWnd, HWND hTarget, LPDWORD &lpPixel){
	wi.SetMainWnd(hWnd);

	// スクリーン情報取得
	GetClientRect(hTarget, &wi.rc);
	wi.SetWndSize((int)wi.rc.right, (int)wi.rc.bottom);

	// DIBの情報を設定
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	wi.GetWndSize(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	// DIB Section の作成
	wi.GethDC(hWnd);
	wi.CreatehDIBSection(&bmi, DIB_RGB_COLORS, (void**)&lpPixel);
	wi.CreateCompatiblehhMemDC();
	wi.SelectObjecthMemDChBmp();
	wi.ReleasehDC(hWnd);

	// スクリーンを DIB Section にコピー
	wi.GethDC(hTarget);
	wi.CopyDC(SRCCOPY);
	wi.ReleasehDC(hTarget);
}

void SetClientSize(HWND hWnd, int width, int height){
	RECT rc = { 0, 0, width, height };

	::AdjustWindowRectEx(&rc, GetWindowLong(hWnd, GWL_STYLE), false, 0);
	::SetWindowPos(hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top, 
		SWP_NOMOVE | SWP_NOZORDER);

}

