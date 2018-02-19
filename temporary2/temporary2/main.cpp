
#include <Windows.h>
//#define noquit

#ifdef noquit
class TEST {
public:
	TEST() {
		::MessageBoxA(NULL, "コンストラクタ", "test", MB_OK);
	}

	~TEST() {
		::MessageBoxA(NULL, "デストラクタ", "test", MB_OK);
	}
};
#endif

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

void setwh(HWND hWnd, int x, int y){
	RECT rc = { 0, 0, x, y };
	AdjustWindowRectEx( &rc, GetWindowLongA(hWnd, GWL_STYLE),
		GetMenu(hWnd) != NULL ? true : false,
		GetWindowLong(hWnd, GWL_EXSTYLE) );

	SetWindowPos(hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER
		);
}

static DWORD __stdcall ThreadProc(DWORD dw){
	MessageBox(NULL, "うわ", "", MB_OK);
	return true;
}

static void AfterThreadProc(){
}

BOOL PleaseDoQuit(DWORD ProcessID) {
	HANDLE h;
	char buf[255] = { 0 };
	LPVOID ThreadProcAdd, tpa_lpvoid;
	DWORD tpa = (DWORD)ThreadProc, atpa = (DWORD)AfterThreadProc;
	
	h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
	if(! h) return false;
	
	ThreadProcAdd = VirtualAllocEx(h, NULL, 512, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (! ThreadProcAdd) return false;

	if (! WriteProcessMemory(h, ThreadProcAdd, tpa_lpvoid,
		(tpa)-(atpa), NULL)) return false;

	HANDLE TempHandle = CreateRemoteThread(h, NULL, NULL,
		(LPTHREAD_START_ROUTINE)GetProcAddress(
			::GetModuleHandle("user32.dll"), "PostQuitMessage"),
		NULL, NULL, NULL);

	WaitForSingleObject(TempHandle, INFINITE);
	CloseHandle(h);
	VirtualFreeEx(h, ThreadProcAdd, 0, MEM_RELEASE);
	return true;
} 

// ウィンドウプロージャー
LRESULT __stdcall WindowProc(HWND hWnd, UINT uMsg , WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_DESTROY: PostQuitMessage(0); return 0;

	#ifdef noquit
	case WM_KEYDOWN:
		if (wp == 'Q') PostQuitMessage(0);
		return 0;
	case WM_QUIT:
	case WM_CLOSE:
		::MessageBoxA(hWnd, "終了できないよーんｗｗ", "test", MB_OK);
		return 0;
	#else
	case WM_KEYDOWN:
		if (wp == 13) {
			POINT pt;
			DWORD ProcessID;

			GetCursorPos(&pt);
			HWND hTargetWnd = WindowFromPoint(pt);

			GetWindowThreadProcessId(hTargetWnd, &ProcessID);
			PleaseDoQuit(ProcessID);
			ErrorMessageBox();
		}
		return 0;

	#endif

	}
	return DefWindowProcA(hWnd, uMsg, wp, lp);
}

// クラス作成
BOOL ClassInit(HINSTANCE hInst, WNDCLASSEX &wce, char* AppName){

	wce.style		= CS_HREDRAW | CS_VREDRAW;	// スタイル
	wce.lpfnWndProc	= WindowProc;	// プロージャ関数
	wce.cbClsExtra	= wce.cbWndExtra = 0;
	wce.hInstance	= hInst;	// プログラムのハンドル
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
		0, AppName, AppName, WS_OVERLAPPEDWINDOW - WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
		NULL, NULL, hInstance, NULL
	);
	if (!hWnd)return FALSE;
	ShowWindow(hWnd, nCmd);

	return TRUE;
}

// システムメイン
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	#ifdef noquit
	TEST test;
	#endif

	WNDCLASSEX wce;
	wce.cbSize		= sizeof(wce);	// 構造体のsize
	if (! ClassInit(hInst, wce, "testing")) return 0;
	
	HWND hWnd;
	if (! WindowInit(hInst, hWnd, nCmd, "testing")) return 0;

	#ifdef noquit
	::MessageBoxA(NULL, "ウィンドウの準備完了", "test", MB_OK);
	#else
	setwh(hWnd, 400, 40);
	HDC hDC = GetDC(hWnd);
	SelectObject(hDC, GetStockObject(OEM_FIXED_FONT));
	char str[128];

	wsprintf(str, "終了したいウィンドウにカーソルを合わせて");
	TextOut(hDC, 5, 0, str, lstrlen(str));
	wsprintf(str, "Ｅｎｔｅｒキーを押してください");
	TextOut(hDC, 5, 20, str, lstrlen(str));
	#endif
	
	MSG msg;
	while(1) {
		// クラス登録とウィンドウの作成
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			// システムメイン
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUITが来てたら
			TranslateMessage(&msg);	// キーボードを利用可能にする
			DispatchMessage(&msg);	// 制御をWindowsに戻す
		}
		Sleep(1);
	}

	#ifdef noquit
	::MessageBoxA(NULL, "後処理の完了", "test", MB_OK);
	#else
	ReleaseDC(hWnd, hDC);
	#endif

	return 0;
}



