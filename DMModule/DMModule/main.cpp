
#include <Windows.h>
#include <shobjidl.h>
#include <locale.h>
#include <Gdiplus.h>

namespace {
	THUMBBUTTON *buttons;//構造体
	ITaskbarList3* ptl;
	HWND hMWnd;
	int nButton;
}

BOOL AddTaskBarButton(int ButtonID , char* szTip, HICON hIcon = NULL){
	if (!hMWnd) return false;
	if (nButton <= ButtonID) return false;
	
	buttons[ButtonID].dwMask = THB_ICON | THB_TOOLTIP | THB_FLAGS;
	buttons[ButtonID].iId = ButtonID;
	if (hIcon == NULL)
		buttons[ButtonID].hIcon = (HICON)GetClassLong(hMWnd, GCL_HICONSM);
	else
		buttons[ButtonID].hIcon = hIcon;

	// WCHAR変換
	setlocale(LC_ALL, "japanese");
	size_t retSize;
	mbstowcs_s(&retSize, buttons[ButtonID].szTip, 260, szTip, strlen(szTip));

	buttons[ButtonID].dwFlags = THBF_ENABLED;
	
	return true;
}

bool MakeTaskBar(HWND hWnd, int nLen){
	if ((!nLen) || (!hWnd)) return false;

	// グローバル変数
	hMWnd = hWnd;
	nButton = nLen;

	// ボタン構造体初期化
	buttons = new THUMBBUTTON[nLen];

	// タスクバーの初期化
	CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList3, (void**)&ptl);

	return true;
}

HRESULT ApplyTaskBarButton(){
	return ptl->ThumbBarAddButtons(hMWnd, nButton, buttons);
}


//初期化
static void InitHDR32BIT(BITMAPINFOHEADER& bmi, int width, int height)
{
bmi.biSize = sizeof(BITMAPINFOHEADER);
bmi.biWidth = width;
bmi.biHeight = height;
bmi.biBitCount = 32;
bmi.biPlanes = 1;
bmi.biCompression = BI_BITFIELDS;
bmi.biSizeImage = bmi.biWidth * bmi.biHeight * (bmi.biBitCount / 8);
}

//32ビット型ビットフィールドの設定
static void InitBitFields32BIT(LPRGBQUAD pRGB)
{
//RGBQUADとDWORDは同じサイズです。ビットフィールドを指定する場合はLPDWORDにキャストする方が楽です（16ビット等では特に）
LPDWORD pBF = reinterpret_cast<LPDWORD>(pRGB);
pBF[0] = 0xff0000;
pBF[1] = 0x00ff00;
pBF[2] = 0x0000ff;
}

//DDBからDIBへの互換コピー
static HBITMAP ConvertToDIB(HBITMAP hDDB)
{
//変換元DDBの情報を取る
BITMAP bitmap = {0};
::GetObject(hDDB, sizeof(bitmap), &bitmap);
//ヘッダのサイズ（RGBQUAD * 3は、ビットフィールド用に必要です）
const int nHDRSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 3);
LPBITMAPINFO pbmi = static_cast<LPBITMAPINFO>(::calloc(nHDRSize, 1));
//32ビット用のヘッダを作成する
::InitHDR32BIT(pbmi->bmiHeader, bitmap.bmWidth, bitmap.bmHeight);
//32ビット用のフィールドを設定する
::InitBitFields32BIT(pbmi->bmiColors);

//DIBセクションの作成
LPVOID pBits = NULL;
HBITMAP hDIB = ::CreateDIBSection(NULL, pbmi, DIB_RGB_COLORS, &pBits, NULL, NULL);
//動的に割り当てたヘッダを解放する
::free(pbmi);

//DIB用HDC
HDC hDCDIB = ::CreateCompatibleDC(NULL);
//DDB用HDC
HDC hDCDDB = ::CreateCompatibleDC(NULL);
//それぞれをHDCに選択
::SelectObject(hDCDIB, hDIB);
::SelectObject(hDCDDB, hDDB);
//DDBからDIBへ互換コピー
::BitBlt(hDCDIB, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hDCDDB, 0, 0, SRCCOPY);
//それぞれのHDCを解放
::DeleteDC(hDCDIB);
::DeleteDC(hDCDDB);
//出来上がり
return hDIB;
}

HBITMAP CreateImage(HDC hDC, int x, int y, UINT width, UINT height){
	// DIBのサイズ
	DWORD imgsize = width*height;

	// ビットマップオブジェクトの作成
	int *lpBits = NULL;
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = 40;
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = imgsize*4;
	HBITMAP hBmp = CreateDIBSection(0, &bmi, DIB_RGB_COLORS, (void**)&lpBits, 0, 0);
	if (hBmp == NULL) return NULL;

	// ビットマップに描画
	HBITMAP hMBmp = ::CreateCompatibleBitmap(hDC, width*2, height);
	HDC hMDC = ::CreateCompatibleDC(hDC);
	SelectObject(hMDC, hMBmp);
	BitBlt(hMDC, 0, 0, width*2, height, hDC, x, y, SRCCOPY);

	UINT cy = height-1, cx = 0;
	for(UINT i=0; i<imgsize; i++){
		if (cx == width-1) { cx = 0; cy--; } else cx++;
		*(lpBits+i) = GetPixel(hMDC, cx, y+cy) |
			(GetRValue(GetPixel(hMDC, width+cx, y+cy) << 24));
	}
	DeleteObject(hMBmp);
	DeleteDC(hMDC);
	/**/
	return hBmp;
}

/*
void SetLayeredWindow(HWND hWnd, int width, int height, int type = -1, HGDIOBJ hObj = NULL, int ipx = 0, int ipy = 0, int flag = 0){
	if (type == -1)
		// アクティブにする
		SetActiveWindow(hWnd);
	else {
		// レイヤードウィンドウ作成
		::SetWindowLongA(hWnd, GWL_EXSTYLE,
			GetWindowLongA(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
//		hWnd = CreateWindowEx(WS_EX_LAYERED, "hspwnd0", "",
//			WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
//			x, y, width, height, NULL, NULL, hInst, NULL);
	}

	//ビットマップオブジェクトを作成するか既存のビットマップオブジェクトを使用するかどうかのフラグ
	HBITMAP hBmp;
	if (flag) hBmp = (HBITMAP)hObj;
	else hBmp = CreateImage((HDC)hObj, ipx, ipy, width, height);

	// DCにBMPを適用
	HDC hDC = GetDC(hWnd), hMemDC;
	hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBmp);

	// ウィンドウサイズ
	SIZE size = {width, height};

	// デバイスコンテキストにおけるレイヤの位置
	POINT pt = {0, 0};

	// レイヤーウィンドウ構成の透明度の値
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA};
	
	// レイヤーウィンドウにする
	::UpdateLayeredWindow(hWnd, hDC, NULL, &size, hMemDC, &pt, 0, &bf, ULW_ALPHA);

	// 表示
	ShowWindow(hWnd, SW_SHOW);

	// 開放処理
	ReleaseDC(hWnd, hDC);
	if (!flag) DeleteObject(hBmp);
	DeleteDC(hMemDC);
}
*/





BOOL WINAPI DllMain(HINSTANCE hDllInst, DWORD fdwReason, LPVOID lpvReserved){
	
	switch(fdwReason){
	case DLL_PROCESS_ATTACH:  // ロードされた
		hMWnd = NULL;
		buttons = NULL;
		break;
	case DLL_PROCESS_DETACH:  // アンロードされた
		if (buttons != NULL) delete[] buttons;
		break;
	}
	return TRUE;
}
