


#include <windows.h>
#include <math.h>
#include "app.h"

#include <shlobj.h>

#pragma comment(lib, "msimg32.lib")
#pragma warning(disable:4244)	// �Î��I�^�ϊ� ��:DOUBLE->LONG(�����_�؂�l��)

#define	WT_REDRAW	1100

class DPOINT {
public:
	double	x;
	double	y;
};

class IMAGE : public DPOINT {
	HBITMAP		hBmp;
	HDC			hDC;
	POINT		size;
	POINT		hsize;
	UINT		crClr;
	double		range, degree;

public:
	IMAGE(){ hDC = NULL; hBmp = NULL; crClr = 0; range = 0; degree = 0; }
	~IMAGE(){
		if (hDC == NULL) return;
		DeleteObject(hBmp);
		DeleteDC(hDC);
		hDC = NULL;
	}
	LONG	xget()				{ return x; }
	void	xset(LONG x)		{ this->x = x; }
	LONG	yget()				{ return y; }
	void	yset(LONG y)		{ this->y = y; }
	LONG	sxget()				{ return size.x; }
	LONG	syget()				{ return size.y; }
	LONG	hsxget()			{ return hsize.x; }
	LONG	hsyget()			{ return hsize.y; }
	void	dset(double degree)	{ this->degree = degree; }
	double	dget()				{ return degree; }
	void	rset(double range)	{ this->range = range; }
	double	rget()				{ return range; }

	bool init(HINSTANCE hInst, HDC hDC, char* filename, LONG x = 0L, LONG y = 0L, COLORREF crClr = 0){
		hBmp = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (hBmp == NULL) return false;

		this->hDC = CreateCompatibleDC(hDC);
		if (hDC == NULL) return false;
		SelectObject(this->hDC, hBmp);

		BITMAP bmp;
		if (! GetObject(hBmp, sizeof(BITMAP), &bmp)) return false;
		size.x = bmp.bmWidth;
		size.y = bmp.bmHeight;
		hsize.x = size.x/2;
		hsize.y = size.y/2;
		this->x = x;
		this->y = y;
		this->crClr = crClr;

		return true;
	}
	void draw(HDC hDC){
		TransparentBlt(hDC, x, y, size.x, size.y,
			this->hDC, 0, 0, size.x, size.y, crClr);
	}
	void hdraw(HDC hDC){// ���a�𒆐S�ɂ��ĕ`��
		TransparentBlt(hDC, x-hsize.x, y-hsize.y, size.x, size.y,
			this->hDC, 0, 0, size.x, size.y, crClr);
	}
};

enum SP { SP_SUN = 0, SP_MER, SP_VEN, SP_EAR, SP_MAX };
class SPACE {
	HINSTANCE	hInst;
	HWND		hWnd;
	HDC			hDC, hMDC;
	HBITMAP		hMBmp;
	HFONT		hFont;
	HBRUSH		hBrush;
	HPEN		hPen;
	IMAGE		Img[SP_MAX];

	bool		redraw;		// �ĕ`��r�b�g
	double		magnif;		// �{��

	void inline validback(){ BitBlt(hDC, 0, 0, ginfo(hDC, GI_X), ginfo(hDC, GI_Y), hMDC, 0, 0, SRCCOPY); }
	void draw();

public:
	~SPACE(){ exit(); }
	bool init(HINSTANCE hInst, HWND hWnd);
	void exit();

	void calc();
	void redrawf(){ redraw = true; }
};

namespace {// �O���[�o���ϐ�
	char	AppName[] = "Space";
	POINT	sWin = { 1024, 600 };	// �E�B���h�E�T�C�Y
	//LONG	AU = 149597870700;		// 1�V���P��(m) (�덷3m)
	LONG	AU = 14959;
	SPACE sp;	// �F���N���X
}




bool SPACE::init(HINSTANCE hInst, HWND hWnd){
	bool b = true;
	this->hInst	= hInst;
	this->hWnd	= hWnd;
	hDC		= GetDC(hWnd);

	// ����ʍ쐬
	hMDC	= CreateCompatibleDC(hDC);
	hMBmp	= CreateCompatibleBitmap(hDC, ginfo(hDC, GI_X), ginfo(hDC, GI_Y));
	SelectObject(hMDC, hMBmp);
	// �t�H���g
	hFont	= (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SelectObject(hMDC, hFont);
	// �u���V
	hBrush	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	SelectObject(hMDC, hBrush);
	// �y��
	hPen	= (HPEN)GetStockObject(WHITE_PEN);
	SelectObject(hMDC, hPen);

	// �摜�ǂݍ���
	char pDesk[MAX_PATH];
	SHGetSpecialFolderPath(NULL, pDesk, CSIDL_DESKTOPDIRECTORY, FALSE);
	lstrcat(pDesk, "\\star\\");
	SetCurrentDirectory(pDesk);
	if (!Img[SP_SUN].init(hInst, hDC, "sun.bmp", 0, 0)){
		MessageBox(NULL, "�摜[sun]���ǂݍ��܂�܂���ł���", "�G���[", MB_OK);
		b = false;
	}
	Img[SP_SUN].x = (ginfo(hWnd, GI_CX)-Img[SP_SUN].sxget())/2;
	Img[SP_SUN].y = (ginfo(hWnd, GI_CY)-Img[SP_SUN].syget())/2;

	if (!Img[SP_MER].init(hInst, hDC, "mercury.bmp", 0, 0)){
		MessageBox(NULL, "�摜[mercury]���ǂݍ��܂�܂���ł���", "�G���[", MB_OK);
		b = false;
	}
	Img[SP_MER].rset(5800);

	if (!Img[SP_VEN].init(hInst, hDC, "venus.bmp", 0, 0)){
		MessageBox(NULL, "�摜[venus]���ǂݍ��܂�܂���ł���", "�G���[", MB_OK);
		b = false;
	}
	Img[SP_VEN].rset(10820);

	if (!Img[SP_EAR].init(hInst, hDC, "earth.bmp", 0, 0)){
		MessageBox(NULL, "�摜[earth]���ǂݍ��܂�܂���ł���", "�G���[", MB_OK);
		b = false;
	}
	Img[SP_EAR].rset(AU);

	magnif = 0.02;	// �{��

	return b;
}

void SPACE::exit(){
	if (hDC == NULL) return;
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(hFont);
	DeleteObject(hMBmp);
	DeleteDC(hMDC);
	ReleaseDC(hWnd, hDC);
	hDC = NULL;
}

// �`��
namespace {
	char str_d[128];		// �t���[�̕�����
	char Month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	SYSTEMTIME syst;		// ���ݎ���
}
void SPACE::draw(){
	Img[SP_SUN].hdraw(hMDC);
	Img[SP_MER].hdraw(hMDC);
	Img[SP_VEN].hdraw(hMDC);
	Img[SP_EAR].hdraw(hMDC);

	wsprintf(str_d, "��ʃT�C�Y[%d:%d]", ginfo(hWnd, GI_CX), ginfo(hWnd, GI_CY));
	TextOut(hMDC, 0,  0, str_d, lstrlen(str_d));
	wsprintf(str_d, "���z�̈ʒu[%d:%d]", (LONG)Img[SP_SUN].x, (LONG)Img[SP_SUN].y);
	TextOut(hMDC, 0, 20, str_d, lstrlen(str_d));
	wsprintf(str_d, "�n���̈ʒu[%d:%d (%d)]", (LONG)Img[SP_EAR].x, (LONG)Img[SP_EAR].y, (LONG)Img[SP_EAR].dget());
	TextOut(hMDC, 0, 40, str_d, lstrlen(str_d));


	validback();	// ����ʔ��f
}

// �v�Z
void SPACE::calc(){
	// ���ݎ���
	GetLocalTime(&syst);

	// ���z�̈ʒu
	Img[SP_SUN].x = (ginfo(hWnd, GI_CX))/2;
	Img[SP_SUN].y = (ginfo(hWnd, GI_CY))/2;

	// �����̈ʒu
	Img[SP_MER].dset(
		(Month[syst.wMonth-1] + syst.wDay) * (((double)AU)*2.0*PI/88.0/24.0)
	);
	Img[SP_MER].x = cos(torad(Img[SP_MER].dget()-90))*Img[SP_MER].rget()*magnif
		+Img[SP_SUN].x;
	Img[SP_MER].y = sin(torad(Img[SP_MER].dget()-90))*Img[SP_MER].rget()*magnif
		+Img[SP_SUN].y;

	// �����̈ʒu
	Img[SP_VEN].dset(
		(Month[syst.wMonth-1] + syst.wDay) * (((double)AU)*2.0*PI/225.0/24.0)
	);
	Img[SP_VEN].x = cos(torad(Img[SP_VEN].dget()-90))*Img[SP_VEN].rget()*magnif
		+Img[SP_SUN].x;
	Img[SP_VEN].y = sin(torad(Img[SP_VEN].dget()-90))*Img[SP_VEN].rget()*magnif
		+Img[SP_SUN].y;

	// ��� = ���]�O���̉~�� / 365
	// �n���̌��݊p�x ���]�O���̉~��=AU*2*PI
	Img[SP_EAR].dset(
		(Month[syst.wMonth-1] + syst.wDay) * 
		(( ((double)AU)*2.0*PI )/365.0)
	);
	// �n���̈ʒu
//	Img[SP_EAR].x = cos(torad(Img[SP_EAR].dget()))*Img[SP_EAR].rget()*magnif
	Img[SP_EAR].x = cos(Img[SP_EAR].dget())*Img[SP_EAR].rget()*magnif
		+Img[SP_SUN].x;
//	Img[SP_EAR].y = sin(torad(Img[SP_EAR].dget()))*Img[SP_EAR].rget()*magnif
	Img[SP_EAR].y = cos(Img[SP_EAR].dget())*Img[SP_EAR].rget()*magnif
		+Img[SP_SUN].y;


	if (redraw) draw();
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_TIMER:
		sp.redrawf();
		if (wp == WT_REDRAW) sp.calc();
		break;
	case WM_DESTROY:
		KillTimer(hWnd, WT_REDRAW);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){
	HWND	hWnd;
	MSG		msg;

	WNDCLASSEX wce;
	wce.cbSize		= sizeof(wce);	// �\���̂�size
	wce.style		= CS_HREDRAW | CS_VREDRAW;	// �X�^�C��
	wce.lpfnWndProc	= WindowProc;	// �v���[�W���֐�
	wce.cbClsExtra	= wce.cbWndExtra = 0;
	wce.hInstance	= hInst;		// �v���O�����̃n���h��
	wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// �A�C�R��
	wce.hCursor		= LoadCursor(NULL, IDC_ARROW);		// �}�E�X�J�[�\��
	wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// �u���V
	wce.lpszMenuName	= NULL;		// ���j���[
	wce.lpszClassName	= AppName;	// �N���X��
	wce.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wce)){
		MessageBox(NULL, "Not created class.\n\nexit now.", "Create error", MB_OK | MB_ICONERROR);
		return 1;
	}

	hWnd = CreateWindowEx(0, AppName, AppName, 
		WS_OVERLAPPEDWINDOW-WS_THICKFRAME-WS_MAXIMIZEBOX | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, sWin.x+6, sWin.y+28,
		NULL, NULL, NULL, NULL
	);
	if (hWnd == NULL) return -1;
	ShowWindow(hWnd, SW_SHOW);

	// ������
	if (!sp.init(hInst, hWnd)){
		MessageBox(NULL, "�I�����܂�", "exit now.", MB_OK);
		return 0;
	}

	SetTimer(hWnd, WT_REDRAW, 1000, NULL);
	SendMessage(hWnd, WM_TIMER, WT_REDRAW, 0);

	while(1) {
		Sleep(1);
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			// �V�X�e�����C��
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUIT�����Ă���
			TranslateMessage(&msg);	// �L�[�{�[�h�𗘗p�\�ɂ���
			DispatchMessage(&msg);	// �����Windows�ɖ߂�
		}else {
		}
	}

	sp.exit();
	return 0;
}

