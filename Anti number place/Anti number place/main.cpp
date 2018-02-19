
// Anti number place


#include <windows.h>

// �t�@�C�����[�J���ϐ�
namespace {
	char AppName[] = "Number place";
	const int ButtonMax = 3;
	enum BUTTON { BID_ALCALC=0, BID_CALC, BID_EXPECT, BID_CLEAR };
	const int xn = 9, yn = 9;			// ���Əc �̌�
	const int xsize = 40, ysize = 40;	// ���Əc �̑傫��
	const int bxn = 3, byn = 3;			// ���Əc �̃u���b�N�̌�
	const int xyn = xn*yn;				// ���~�c �̌�
	const int xysize = xsize*ysize;		// ���~�c �̑傫��
	HWND hButton[3];	// �q�E�B���h�E�̃n���h��
}


// ��֐�
void __stdcall ErrorMessageBox();
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);
HWND CreateEditBox(HINSTANCE hInst, HWND hParent, long ChildID, int X=0, int Y=0, int Width=0, int Height=0, char* text=NULL);
int CalcNumberPlace(char np[xn][yn], char npe[xn][yn][xn]);

// ���֐�
void pmes(HDC hDC, LONG px, LONG py, char* text);
void line(HDC hDC, LONG x1, LONG y1, LONG x2, LONG y2, LPPOINT ppt);
int toint(char c);
char* tostr(char *buf, int i);
void tocharbuf(char *buf, char *i);

// �}�N��
#pragma region mainloop macro
// ���C�����[�v
#define mainloop_begin(msg, b, time) MSG msg; while(b) {Sleep(time);\
	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){/* �V�X�e�����C�� */\
	if (!GetMessage(&msg, NULL, 0, 0)) break;/* WM_QUIT�����Ă��� */\
	TranslateMessage(&msg);	/* �L�[�{�[�h�𗘗p�\�ɂ��� */\
	DispatchMessage(&msg);	/* �����Windows�ɖ߂� */\
	}else
#define mainloop_end }
// �^�C�g���� [Anti number place] �̃_�C�A���O
#define anpBox(hWnd, text) MessageBox(hWnd, text, "Anti number place", MB_OK);
#pragma endregion

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){
	// �E�B���h�E�֘A�ϐ�
	WNDCLASSEX wce;
	POINT	sClient = { xn*xsize+2, yn*ysize+60 };
	POINT	sWindow = { sClient.x+6, sClient.y+28 };
	HWND	hWnd;
	#pragma region /// �N���X�E�E�B���h�E �̍쐬
	wce.cbSize		= sizeof(wce);	// �\���̂�size
	wce.style		= CS_HREDRAW | CS_VREDRAW;	// �X�^�C��
	wce.lpfnWndProc	= WindowProc;	// �v���[�W���֐�
	wce.cbClsExtra	= wce.cbWndExtra	= 0;
	wce.hInstance	= hInst;		// �v���O�����̃n���h��
	wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// �A�C�R��
	wce.hCursor		= LoadCursor(NULL, IDC_ARROW);		// �}�E�X�J�[�\��
	wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// �u���V
	wce.lpszMenuName	= NULL;		// ���j���[
	wce.lpszClassName	= AppName;	// �N���X��
	wce.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	if (! RegisterClassEx(&wce)){ ErrorMessageBox(); return 1; }

	// �e
	hWnd = CreateWindowEx(0,
		AppName, AppName,
		(WS_OVERLAPPEDWINDOW - WS_THICKFRAME) | WS_VISIBLE | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, sWindow.x, sWindow.y,
		NULL, NULL, NULL, NULL
	);
	if (hWnd == NULL) { ErrorMessageBox(); return 1; }
	#pragma endregion

	InvalidateRect(hWnd, NULL, FALSE);

	mainloop_begin(msg, 1, 1){
	} mainloop_end;

	return 0;
}


// �E�B���h�E�v���[�W��
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	static char np[xn][yn];
	static char npe[xn][yn][xn];
	static HFONT hFont, hSubFont;
	static HBRUSH hBrush, hBkBrush;
	static HDC hDC, hMDC;
	static HBITMAP hMBmp;
	static short px, py;

	switch(uMsg){
	#pragma region case WM_CREATE:
	case WM_CREATE:{
		//do
		tocharbuf("  3  56  ", np[0]);
		tocharbuf("   7 6 1 ", np[1]);
		tocharbuf("45    8 3", np[2]);
		tocharbuf("14      9", np[3]);
		tocharbuf("   9     ", np[4]);
		tocharbuf("6    1  4", np[5]);
		tocharbuf("     3  7", np[6]);
		tocharbuf("  1      ", np[7]);
		tocharbuf("3 9187   ", np[8]);
		//~do
		px = 0; py = 0;
		hFont = CreateFont(
			xsize-6, 0, 0, 0, FW_THIN, false, false, false,
			SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			FIXED_PITCH | FF_MODERN, "�l�r �S�V�b�N"
		);

		RECT rc;
		GetClientRect(hWnd, &rc);
		hSubFont = CreateFont(
			rc.right/xn/bxn-1, 0, 0, 0, FW_THIN, false, false, false,
			SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			FIXED_PITCH | FF_MODERN, "�l�r �S�V�b�N"
		);

		hBrush = CreateSolidBrush(RGB(0, 60, 255));
		hBkBrush = CreateSolidBrush(RGB(200, 230, 255));
		hDC = GetDC(hWnd);
		hMDC = CreateCompatibleDC(hDC);
		hMBmp = CreateCompatibleBitmap(hDC, rc.right, rc.bottom);
		SelectObject(hMDC, hMBmp);

		// Button �쐬
		int i;
		char ButtonText[ButtonMax][8] = {"AllCalc", "Calc", "Clear"};
		for(i=0; i<ButtonMax; i++){
			hButton[i] = CreateWindowEx(0,
				"BUTTON", ButtonText[i],
				WS_CHILD | WS_VISIBLE,
				rc.right/ButtonMax*i+(i != 0 ? 2 : 0), yn*ysize+2,
				rc.right/ButtonMax+(i == 0 ? 2 : 0), 20,
				hWnd, (HMENU)i,
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL
			);
			SendMessage(hButton[i], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);
		}
		break;
	}
	#pragma endregion
	#pragma region case WM_DESTROY:
	case WM_DESTROY:
		DeleteObject(hMBmp);
		DeleteDC(hMDC);
		ReleaseDC(hWnd, hDC);
		DeleteObject(hBkBrush);
		DeleteObject(hBrush);
		DeleteObject(hSubFont);
		DeleteObject(hFont);
		PostQuitMessage(0);
		return 0;
	#pragma endregion
	#pragma region case WM_PAINT:
	case WM_PAINT:{
		char str[2];
		RECT rc;
		// �N���A
		GetClientRect(hWnd, &rc);
		FillRect(hMDC, &rc, hBkBrush);

		// �g��
		for(int x=0; x<xn+1; x++){
			line(hMDC, x*xsize, 0, x*xsize, yn*ysize+2, NULL);
			if (!(x%3)) line(hMDC, x*xsize+1, 0, x*xsize+1, yn*ysize+2, NULL);
		}
		for(int y=0; y<yn+1; y++){
			line(hMDC, 0, y*ysize, xn*xsize+2, y*ysize, NULL);
			if (!(y%3)) line(hMDC, 0, y*ysize+1, xn*xsize+2, y*ysize+1, NULL);
		}

		// ���`��
		SelectObject(hMDC, hSubFont);
		SetBkMode(hMDC, TRANSPARENT);
		SetTextColor(hMDC, RGB(127, 127, 127));
		GetClientRect(hWnd, &rc);
		rc.bottom -= 60;
		for(int y=0; y<yn; y++) for(int x=0; x<xn; x++) for(int i=0; i<xn; i++)
		if (!np[x][y]) if (npe[x][y][i]){
			wsprintf(str, "%d", npe[x][y][i]);
			pmes(hMDC, x*xsize+4+i%bxn*(rc.right/xn/bxn),
				y*ysize+2+i/byn*(rc.bottom/yn/byn), str);
		}
		// �����`��
		SelectObject(hMDC, hFont);
		SetTextColor(hMDC, 0);
		for(int y=0; y<yn; y++) for(int x=0; x<xn; x++) if (np[x][y]) {
			wsprintf(str, "%d", np[x][y]);
			pmes(hMDC, x*xsize+11, y*ysize+4, str);
		}
		// �I���l�p�`�\��
		rc.left = px*xsize; rc.top = py*ysize;
		rc.right = rc.left+xsize; rc.bottom = rc.top+ysize;
		FrameRect(hMDC, &rc, hBrush);
		rc.left++; rc.top++; rc.right++; rc.bottom++;
		FrameRect(hMDC, &rc, hBrush);

		// �R�s�[
		GetClientRect(hWnd, &rc);
		BitBlt(hDC, 0, 0, rc.right, rc.bottom, hMDC, 0, 0, SRCCOPY);
		break;
	}
	#pragma endregion
	#pragma region case WM_KEYDOWN:
	case WM_KEYDOWN:
		switch(wp){
		case 37: if (px)		px--; break;
		case 39: if (px<xn-1)	px++; break;
		case 38: if (py)		py--; break;
		case 40: if (py<yn-1)	py++; break;
		default:
			if (('0' <= wp) && (wp <= '9')) np[px][py] = wp-'0';
		}
		
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	#pragma endregion
	#pragma region case WM_COMMAND:
	case WM_COMMAND:{
		int wmID = LOWORD(wp), wmEvent = HIWORD(wp);
		switch(wmID){
		case BID_ALCALC: case BID_CALC:
			// �v�Z
			RECALC:
			switch(CalcNumberPlace(np, npe)){
			case 0: if (wmID == BID_ALCALC) goto RECALC;
			case 1: break;
			case 2:
				anpBox(hWnd, "�ł��Ȃ���Ԃł��B")
				break;
			case 3:
				anpBox(hWnd, "���łɂł��Ă��܂��B");
				break;
			case 4:
				anpBox(hWnd, "���݂̉���ł͓������o���܂���ł����B");
			}
			InvalidateRect(hWnd, NULL, FALSE);	// �ĕ`��
			break;
		case BID_CLEAR:
			for(int y=0; y<yn; y++) for(int x=0; x<xn; x++){
				np[x][y] = 0;
				for(int i=0; i<xn; i++) npe[x][y][i] = 0;
			}
			InvalidateRect(hWnd, NULL, FALSE);	// �ĕ`��
			break;
		}
		if (GetFocus() != hWnd) SetFocus(hWnd);
		break;
	}
	#pragma endregion
	#pragma region case WM_CTLCOLOREDIT:
	case WM_CTLCOLOREDIT:
		SetBkMode((HDC)wp, TRANSPARENT);
		SetBkColor((HDC)wp, RGB(255, 255, 255));
		SetTextColor((HDC)wp, RGB(0, 0, 0));
		return (LRESULT)GetStockObject(WHITE_BRUSH);
	#pragma endregion
	#pragma region case WM_LBUTTONDOWN:
	case WM_LBUTTONDOWN:{
		px = (LOWORD(lp)-2)/xsize;
		py = (HIWORD(lp)-2)/ysize;
		InvalidateRect(hWnd, NULL, FALSE);	// �ĕ`��
		break;
	}
	#pragma endregion
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

// API �G���[���b�Z�[�W�{�b�N�X
void __stdcall ErrorMessageBox(){
	LPVOID lpMsgBuf;

	FormatMessage(//�G���[�\��������쐬
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR)&lpMsgBuf, 0, NULL
	);

	MessageBox(NULL, (const char*)lpMsgBuf, NULL, MB_OK);	//���b�Z�[�W�\��
	LocalFree(lpMsgBuf);
}

// EditBox�쐬
HWND CreateEditBox(HINSTANCE hInst, HWND hParent, long ChildID, int X, int Y, int Width, int Height, char* text){
	return CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"EDIT",
		text,
		WS_CHILD | WS_GROUP | WS_TABSTOP | WS_VISIBLE | ES_CENTER | ES_NUMBER,
		X, Y,
		Width, Height,
		hParent,
		(HMENU)ChildID,
		hInst, NULL
	);
}

// Number Place �v�Z
int CalcNumberPlace(char np[xn][yn], char npe[xn][yn][xn]){
	static bool old = false;
	bool npf[xn][yn];

	// �t���O�����擾
	int complete = xn*yn;
	for(int y=0; y<yn; y++) for(int x=0; x<xn; x++)
		if (np[x][y])	{ complete++; npf[x][y] = true; }
		else			{ complete--; npf[x][y] = false; }
	if (complete == 0) return 2;
	if (complete == (xn*yn)*2) return 3;

	// ��{���[��
	// �E���� ���łɂ��鐔���́u�����v�ɂ͓���Ȃ��Ƃ� ��{�������甲��
	// �E�c�� �V
	// �E�u���b�N���� �V
	complete = 0;
	int j=0, n=0, xx=0, yy=0;
	for(int y=0; y<yn; y++) for(int x=0; x<xn; x++){
		if (npf[x][y]) continue;

		// ���� [1, 2, ...]
		for(int i=0; i<xn; i++) npe[x][y][i] = i+1;
			
		//���ɂȂ�̐��������邩
		for(int i=0; i<xn; i++) if (npf[(x+i)%xn][y])
			npe[x][y][ np[(x+i)%xn][y]-1 ] = 0;
		
		//�c�ɂ����邩
		for(int i=0; i<yn; i++) if (npf[x][(y+i)%yn])
			npe[x][y][ np[x][(y+i)%yn]-1 ] = 0;

		//�u���b�N�̒��ɂ͂��邩
		xx = x/bxn*bxn; yy = y/byn*byn;
		for(j=0; j<byn; j++) for(int i=0; i<bxn; i++)
			if (npf[xx+i][yy+j]) npe[x][y][ np[xx+i][yy+j]-1 ] = 0;

		// �E�\�z�������g���ď���
		// �E�u�����v�̗\�z�������炱�̃u���b�N���̗\�z�����𔲂�
		xx = x/bxn*bxn; yy = y/byn*byn;
		for(int l=0; l<xn; l++){
			n = npe[x][y][l];
			for(j=0; j<byn; j++) for(int i=0; i<bxn; i++)
				if (npf[xx+i][yy+j]) ;

		}

		//�����ɓ��鐔����������̏ꍇ�A���
		j = 0, n = 0;
		for(int i=0; i<xn; i++) if (npe[x][y][i]) { j++; n = npe[x][y][i]; }
		if (j == 1) {
			np[x][y] = n;
			npf[x][y] = true;
			complete++;
		}
	}
	old = true;
	if (!complete) return 4;

	// ����

	complete = 1;
	for(int y=0; y<yn; y++) for(int x=0; x<xn; x++)
		if (!npf[x][y]) complete = 0;
	return complete;
}


// ���`��
void line(HDC hDC, LONG x1, LONG y1, LONG x2, LONG y2, LPPOINT ppt){
	MoveToEx(hDC, x1, y1, ppt);
	LineTo(hDC, x2, y2);
}

// �����`��
void pmes(HDC hDC, LONG px, LONG py, char* text){
	SIZE size;
	GetTextExtentPoint32(hDC, text, lstrlen(text), &size);

	RECT rc = {px, py, px+size.cx, py+size.cy};
	DrawText(hDC, text, lstrlen(text), &rc, DT_WORDBREAK |
		DT_CENTER | DT_TOP | DT_EXPANDTABS);
}

// �����𐔎��ɕϊ�
int toint(char c){
	if (('0' <= c) && (c <= '9')) return c-'0';
	return 0;
}

// �����𕶎��ɕϊ�
char* tostr(char *buf, int i){
	if (i) wsprintf(buf, "%d", i);
	return buf;
}

// �������z��ɕϊ�
void tocharbuf(char *buf, char *i){
	for(int n=0; n<lstrlen(buf); n++) i[n] = toint(buf[n]);
}
