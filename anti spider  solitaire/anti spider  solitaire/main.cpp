
#include <Windows.h>

union COLOR {
	COLOR(){ clr = 0; }
	COLOR(HDC hDC, int x, int y) { clr = GetPixel(hDC, x, y); }

	LPCOLORREF pt() { return &clr; }
	COLORREF get(HDC hDC, int x, int y){
		clr = GetPixel(hDC, x, y);
		return clr;
	}
	BYTE r(){ return data[3]; }
	BYTE g(){ return data[2]; }
	BYTE b(){ return data[1]; }
	BYTE a(){ return data[0]; }

private:
	char data[4];
	COLORREF  clr;
};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	HWND hWnd = FindWindowA("Spider Solitaire", "スパイダー ソリティア");
	if (hWnd == NULL) {
		MessageBox(NULL, "スパイダーソリティアが起動されていません。", "Anti spider solitaire", MB_OK);
		return 0;
	}

	char str[128];
	RECT rc;

	GetClientRect(hWnd, &rc);

	HDC hDC = GetDC(hWnd);
	if (hDC != NULL) {
		COLOR clr(hDC, rc.right/2, rc.bottom/3*2);
		wsprintf(str, "color = %02X %02X %02X", clr.r(), clr.g(), clr.b());
		
		ReleaseDC(hWnd, hDC);
		MessageBox(NULL, str, "", MB_OK);
	}


}


