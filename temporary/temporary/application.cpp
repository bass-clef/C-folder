
#include <windows.h>
#include "application.h"

// �N���X�쐬
BOOL ClassInit(WNDCLASSEX &wce, char* AppName){

	wce.cbSize		= sizeof(wce);	// �\���̂�size
	wce.style		= CS_HREDRAW | CS_VREDRAW;	// �X�^�C��
	wce.lpfnWndProc	= DefWindowProc;	// �v���[�W���֐�
	wce.cbClsExtra	= wce.cbWndExtra = 0;
	wce.hInstance	= NULL;	// �v���O�����̃n���h��
	wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// �A�C�R��
	wce.hCursor		= LoadCursor(NULL, IDC_ARROW);	// �}�E�X�J�[�\��
	wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// �u���V
	wce.lpszMenuName	= NULL;	// ���j���[
	wce.lpszClassName	= AppName;	// �N���X��
	wce.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wce))return FALSE;

	return TRUE;
}

// �E�B���h�E�쐬�E�\��
BOOL WindowInit(HINSTANCE hInstance, HWND &hWnd, int nCmd, char* AppName){
	
	hWnd = CreateWindowEx(
		0, AppName, AppName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
		NULL, NULL, hInstance, NULL
	);
	if (!hWnd)return FALSE;
	ShowWindow(hWnd, nCmd);

	return TRUE;
}

// �E�B���h�E�̏��
int GetWindowsInfo(UINT uGetInfoType){
	int ret = NULL;
	switch(uGetInfoType){
	case GWSI_DESKTOPX:{
		RECT	rc;
		GetWindowRect(GetDesktopWindow(), &rc);
		ret = rc.right;
		break;}
	case GWSI_DESKTOPY:{
		RECT	rc;
		GetWindowRect(GetDesktopWindow(), &rc);
		ret = rc.bottom;
		break;}
	}
	return ret;
}



// DC��Bitmap���쐬���đI��
void CreateDCWithBitmap(HDC &hDC, HBITMAP &hbmp, int xSize, int ySize, HDC hDC2){
	hDC		= CreateCompatibleDC(hDC2);
	hbmp	= CreateCompatibleBitmap(hDC2, xSize, ySize);
	SelectObject(hDC, hbmp);
}

// �u���V���쐬���đI��
void CreateSelectBursh(HDC hDC, HBRUSH &hbr, COLORREF crColor){
	hbr = CreateSolidBrush(crColor);
	SelectObject(hDC, hbr);
}

// �y�����쐬���đI��
void CreateSelectPen(HDC hDC, HPEN &hpen, COLORREF crColor){
	hpen = CreatePen(PS_SOLID, 1, crColor);
	SelectObject(hDC, hpen);
}

// ������\��
void Print(HDC hDC, LPSTR Text, int xPos, int yPos) {
	int		Len = lstrlen(Text);
	char	*Str =  new char[Len+5];
	wsprintf(Str, "%s", Text);
//	DrawText(hDC, Str, Len, NULL, DT_NOCLIP);
	TextOut(hDC, xPos, yPos, Str, Len);
	delete[] Str;
}

// ����`��
void DrawLine(HDC hDC, int x1, int y1, int x2, int y2){
	MoveToEx(hDC, x1, y1, NULL);
	LineTo(hDC, x2, y2);
}

// �l�p�`�`��
void DrawRect(HDC hDC, int x1, int y1, int x2, int y2, UINT DrawType){
	switch(DrawType){
	case DR_FRAME:{
		RECT	rc = {x1, y1, x2, y2};
		FrameRect(hDC, &rc, (HBRUSH)GetCurrentObject(hDC, OBJ_PEN));
		break;}
	case DR_FILL:
		Rectangle(hDC, x1, y1, x2, y2);
		break;
	case DR_GRADATION:
		break;
	}
}

// �~�`��
void DrawCircle(HDC hDC, int x1, int y1, int x2, int y2, UINT DrawType){
	switch(DrawType){
	case DC_FRAME:
		Arc(hDC, x1, y1, x2, y2, x2-x1, y1, x2-x1, y1);
		break;
	case DC_FILL:
		Ellipse(hDC, x1, y1, x2, y2);
		break;
	}
}

// DC���R�s�[
void CopyDC(HDC hToDC, HDC hFromDC, DWORD dwrop) {
	BitBlt(hToDC, 0, 0, GetDeviceCaps(hToDC, HORZRES), GetDeviceCaps(hFromDC, VERTRES),
		hFromDC, 0, 0, dwrop
	);
}


// DC��Bitmap���폜
void DeleteDCWithBitmap(HDC hDC, HBITMAP hbmp){
	DeleteObject(hbmp);
	DeleteDC(hDC);
}


// �������{���Ė߂�l�Ŗ߂�
char* strf(char* buffer, char* format, ...){
	va_list argptr;
	
	va_start(argptr, format);
	vsprintf_s(buffer, sizeof(buffer), format, argptr);
	va_end(argptr);
	
	return buffer;
}

