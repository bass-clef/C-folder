


#ifndef DEFINE_H
#define DEFINE_H

// �R���p�C���G���[����
#pragma warning(disable:4244)	// �Î��I�^�ϊ� ��:DOUBLE->LONG(�����_�؂�l��)

// �w�b�_�[�t�@�C���ǂݍ���
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <shlobj.h>
#include <stdarg.h>
// STL
#include <string>

// �Q�ƃ��C�u�����ǉ�
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "msimg32.lib")


// �G���[�����p�N���X ���̒萔��
enum STG_ERROR {
	ERROR_IMG = 100,
	E_IMG_PLAYER,
	E_IMG_ENEMY
};
class S_ERROR {
	long number;
	long line;

public:
	void seterror(STG_ERROR error){
		number = error;
		line = __LINE__;
	}
	long nget(){ return number; }
	long lget(){ return line; }
};


// �O���[�o���ϐ��E�O���[�o���֐��E�萔 �̒�`
namespace {
	const int FPS = 60;		// set frames person second
	DWORD now_time = 0;		// now frames person second
	DWORD start_time = timeGetTime();
	char AppName[] = "mono";	// �A�v���P�[�V������
	const double PI = 3.1415926535;	// �~����
	const POINT	sWin = {640, 480};	// �E�B���h�E�T�C�Y
	S_ERROR e;	// �G���[�����p
	// do
	char	g_str[128];	// �O���[�o���ϐ� (char)
	int		g_int[4];	// �O���[�o���ϐ� (int)
};

// �E�B���h�E���擾
enum GINFO_PARAMETER { GI_NOTHING, GI_X, GI_Y, GI_WX, GI_WY, GI_CX, GI_CY };
LONG inline ginfo(HANDLE handle, int gettype){
	switch(gettype){
	case GI_X:	return GetDeviceCaps((HDC)handle, HORZRES);
	case GI_Y:	return GetDeviceCaps((HDC)handle, VERTRES);
	case GI_WX:	break;
	case GI_WY:	break;
	case GI_CX: {
		RECT rc;
		GetClientRect((HWND)handle, &rc);
		return rc.right;
	}
	case GI_CY: {
		RECT rc;
		GetClientRect((HWND)handle, &rc);
		return rc.bottom;
	}
	}
	return GI_NOTHING;
}

// �x�E���W�A�� ���ݕϊ�
double inline torad(double degree){
	return degree * PI / 180.0;
}
double inline todeg(double radian){
	return radian * 180.0 / PI;
}

// �����Ԃ�
char* strf(char* buffer, char* format, ...);
char* strf_s(char* buffer, size_t _Size_In_Bytes, char* format, ...);


#define DPOINT		CPOINT<double>
#define MAX			LONG_MAX
// end define



// �N���X��` (���֐��E�X�g���N�^�̂�)

// ���W
template <class T=int> class CPOINT {
protected:
	T x, y;	// ���W

public:
	CPOINT operator=(DPOINT &dpt) { x = dpt.x; y = dpt.y; }
	CPOINT() { x = 0; y = 0; }
	void	set(T x, T y)		{ this->x = x; this->y = y; }
	T		xget()				{ return x; }
	void	xset(T x)			{ this->x = x; }
	T		yget()				{ return y; }
	void	yset(T y)			{ this->y = y; }
};

// �̗�
class HP {
protected:
	int hp;		// ���݂̒l
	int hpmax;	// �ő�̒l
	int sub;	// ��������l
	int add;	// ���Z����l

public:
	HP() { hp = 0; hpmax = 0; sub = 0; add = 0; }
	void	set(int hp)			{ this->hp = hp; }
	int		get()				{ return hp; }
	void	maxset(int max)		{ this->hpmax = max; }
	int		maxget()			{ return hpmax; }
	void	asub()				{ hp -= sub; }
	void	subset(int sub)		{ this->sub = sub; }
	int		subget()			{ return sub; }
	void	aadd()				{ hp += add; }
	void	addset(int add)		{ this->add = add; }
	int		addget()			{ return add; }

};

// ��N���X (���W, �t���[��, �t���O, �p�x, ���x, �摜ID, �摜�T�C�Yx, y)
class BASE : public DPOINT {
private:
	DPOINT		p;				// ���W

protected:
	int			frame;			// �����Ă���̃t���[��
	char		flag;			// �t���O
	double		radius, degree;	// ���a(���x), ��]
	HBITMAP		hBmp;			// �摜�pBitmap
	HDC			hDC;			// �摜�pDC
	CPOINT<>	size;			// �摜���a
	DPOINT		hsize;			// �摜���a
	CPOINT<>	div;			// �����摜���a
	DPOINT		hdiv;			// �����摜���a
	CPOINT<>	ndiv;			// �����摜�ԍ�xy
	UINT		crClr;			// �摜���ߐF

public:
	// �R���X�g���N�^
	BASE(){
		radius = 0;
		degree = 0;
		hDC = NULL;
		hBmp = NULL;
		crClr = 0;
	}
	// �f�X�g���N�^
	~BASE(){
		if (hDC == NULL) return;
		DeleteObject(hBmp);
		DeleteDC(hDC);
		hDC = NULL;
	}

	DPOINT	dpget()				{ p.set(x, y); return p; }
	void	frameAdd()			{ frame ++; }
	void	frameSet(int frame)	{ this->frame = frame; }
	int		frameGet()			{ return frame; }
	void	flagSet(int flag)	{ this->flag = flag; }
	int		flagGet()			{ return flag; }
	void	dset(double degree)	{ this->degree = degree; }
	double	dget()				{ return degree; }
	void	rset(double radius)	{ this->radius = radius; }
	double	rget()				{ return radius; }
	double	sxget()				{ return size.xget(); }
	double	syget()				{ return size.yget(); }
	double	hsxget()			{ return hsize.xget(); }
	double	hsyget()			{ return hsize.yget(); }
	void	crset(UINT crClr)	{ this->crClr = crClr; }
};

class BODY : public BASE, public HP {
public:
	// �v�Z
	virtual void calc() = 0;
	// �摜�ǂݍ���
	bool init(HINSTANCE hInst, HDC hDC, char* filename, LONG x = 0L, LONG y = 0L, COLORREF crClr = 0, LONG divx = 1, LONG divy = 1);
	// �`�� (���ߐF�͓���)
	void draw(HDC hDC);
	// ���a�𒆐S�Ƃ��ĕ`�� (���ߐF�͓���)
	void hdraw(HDC hDC);
	// �������a�𒆐S�Ƃ��ĕ������ĕ`�� (���ߐF�͓���)
	void ddraw(HDC hDC);
};

class PLAYER : public BODY {
public:
	void calc();
};

class ENEMY : BODY{
};

class ITEM : public BASE{
};

enum SHOTING {S_P, S_E, S_MAX};
class STG{
protected:
	HINSTANCE	hInst;
	HWND		hWnd;
	HDC			hDC, hMDC;
	HBITMAP		hMBmp;
	HFONT		hFont;
	HBRUSH		hBrush;
	HPEN		hPen;

	PLAYER		player;
	ENEMY		*enemy;
	ITEM		*item;

	bool		redraw;		// �ĕ`��r�b�g
	double		magnif;		// �{��
	void draw();
	void inline validback(){
		BitBlt(hDC, 0, 0, ginfo(hDC, GI_X), ginfo(hDC, GI_Y),
			hMDC, 0, 0, SRCCOPY);
	}

	LONG		px, py;		// mes�p���W
	char*		buf;		// mes�p�o�b�t�@�|�C���^
	size_t		_SizeInBytes;
	void inline pos(LONG px, LONG py){
		this->px = px;
		this->py = py;
	}
	void inline sbuf(char* text, size_t _SizeInBytes){
		buf = text;
		this->_SizeInBytes = _SizeInBytes;
	}
	void inline bmes(char* format, ...){
		va_list argptr;
		
		va_start(argptr, format);
		vsprintf_s(buf, _SizeInBytes, format, argptr);
		va_end(argptr);

		mes(buf);
	}
	void inline mes(char* text){
		pmes(px, py, text);
	}
	void inline pmes(LONG px, LONG py, char* text){
		SIZE size;
		GetTextExtentPoint32(hMDC, text, lstrlen(text), &size);

		RECT rc = {px, py, px+size.cx, py+size.cy};
		DrawText(hMDC, text, lstrlen(text), &rc, DT_WORDBREAK |
			DT_LEFT | DT_TOP | DT_EXPANDTABS);
		this->px = rc.left;
		this->py = rc.bottom;
	}

public:
	STG() {
		hInst = NULL;
		hWnd = NULL;
		hDC = NULL;
		hMDC = NULL;
		hMBmp = NULL;
		hFont = NULL;
		hBrush = NULL;
		hPen = NULL;
	}
	~STG(){ exit(); }
	bool init(HINSTANCE hInst, HWND hWnd);
	void exit();

	void calc();
	void redrawf(){ redraw = true; }

};

//end define



#endif
