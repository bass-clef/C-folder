
// EnumWindows

#include <windows.h>

static int count = 0, wndmax = 0;
static WNDCLASSEX wc;


BOOL WINAPI DllMain( HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved){
	return true;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lp){
	if (count == wndmax) return false;
	if (GetWindowTextLength(hWnd) > 0) {
		((HWND *)lp)[count] = hWnd;
		count ++;
	}
	return true;
}

int __stdcall GetWindows(HWND* &hWnd, int wmax){
	count = 0;
	wndmax = wmax;
//	hWnd = new HWND[wmax];
	EnumWindows(EnumWindowsProc, (LPARAM)&hWnd);
//	delete[] hWnd;

	return count;
}

LRESULT __stdcall WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

bool __stdcall CreateChildClass(HINSTANCE hInst, HWND hWnd){
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInst;
	wc.hIcon         = (HICON)GetClassLong(hWnd, GCL_HICON);
	wc.hCursor       = (HCURSOR)GetClassLong(hWnd, GCL_HCURSOR);
	wc.hbrBackground = (HBRUSH)GetClassLong(hWnd, GCL_HBRBACKGROUND);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ChildWindows";
	wc.hIconSm       = (HICON)GetClassLong(hWnd, GCL_HICONSM);
	if (RegisterClassEx(&wc) == 0) return false;
	return true;
}

bool __stdcall DeleteChildClass(HINSTANCE hInst){
	return UnregisterClass("ChildWindows", hInst);
}

HWND __stdcall CreateChildWindow(HINSTANCE hInst, HWND hParentWnd, 
	DWORD dwExStyle, char* wTitle, int cx, int cy, int Width, int Height){
	if (wc.cbSize != sizeof(WNDCLASSEX))return NULL;

	HWND hWnd;
	hWnd = CreateWindowEx(dwExStyle, "ChildWindows", wTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, cx, cy, Width, Height,
		hParentWnd, 0, hInst, 0);

	return hWnd;
}
