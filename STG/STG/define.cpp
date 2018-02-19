
#include "define.h"

// �����Ԃ�
char* strf(char* buffer, char* format, ...){
	va_list argptr;
	
	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);
	
	return buffer;
}

// �����Ԃ� (�o�b�t�@�T�C�Y�w��)
char* strf_s(char* buffer, size_t _SizeInBytes, char* format, ...){
	va_list argptr;
	
	va_start(argptr, format);
	vsprintf_s(buffer, _SizeInBytes, format, argptr);
	va_end(argptr);
	
	return buffer;
}


// BODY �N���X
bool BODY::init(HINSTANCE hInst, HDC hDC, char* filename, LONG x, LONG y, COLORREF crClr, LONG divx, LONG divy){
	hBmp = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp == NULL) return false;
		this->hDC = CreateCompatibleDC(hDC);
	if (hDC == NULL) return false;
	SelectObject(this->hDC, hBmp);

	BITMAP bmp;
	if (! GetObject(hBmp, sizeof(BITMAP), &bmp)) return false;
	size.set(bmp.bmWidth, bmp.bmHeight);
	hsize.set(size.xget()/2, size.yget()/2);
	div.set(size.xget()/divx, size.yget()/divy);
	ndiv.set(0, 0);
	hdiv.set(div.xget()/2, div.yget()/2);
	this->x = x;
	this->y = y;
	this->crClr = crClr;

	return true;
}

void BODY::draw(HDC hDC){
	TransparentBlt(hDC, x, y, size.xget(), size.yget(),
		this->hDC, 0, 0, size.xget(), size.yget(), crClr);
}

void BODY::hdraw(HDC hDC){
	TransparentBlt(hDC, x-hsize.xget(),
		y-hsize.yget(), size.xget(), size.yget(),
		this->hDC, 0, 0, size.xget(), size.yget(), crClr);
}

void BODY::ddraw(HDC hDC){
	TransparentBlt(hDC, x-hdiv.xget(),
		y-hdiv.yget(), div.xget(), size.yget(),
		this->hDC, ndiv.xget(), ndiv.yget(),
		div.xget()+ndiv.xget(), ndiv.yget()+div.yget(),
		crClr);
	if (div.xget() != size.xget() || div.yget() != size.yget()){
		// �摜�ԍ����Z->��]
		ndiv.set(((int)(ndiv.xget()+hdiv.xget()))%size.xget(),
			((int)(ndiv.yget()+hdiv.yget()))%size.yget());
	}
}


// PLAYER �N���X
void PLAYER::calc(){
	// �ړ�
	byte key[256];
	GetKeyboardState(key);
	byte way = 0;
	if (key[VK_LEFT] & 0x80)	{ way += 1; }
	if (key[VK_RIGHT] & 0x80)	{ way += 4; }
	if (key[VK_UP] & 0x80)		{ way += 2; }
	if (key[VK_DOWN] & 0x80)	{ way += 8; }
	if (way){
		if (way & 1) x = x-radius;
		if (way & 4) x = x+radius;
		if (way & 2) y = y-radius;
		if (way & 8) y = y+radius;
	}

}


// STG �N���X
bool STG::init(HINSTANCE hInst, HWND hWnd){
	this->hInst	= hInst;
	this->hWnd	= hWnd;
	hDC		= GetDC(hWnd);

	// ����ʍ쐬
	hMDC	= CreateCompatibleDC(hDC);
	hMBmp	= CreateCompatibleBitmap(hDC, ginfo(hDC, GI_X), ginfo(hDC, GI_Y));
	SelectObject(hMDC, hMBmp);
	// �t�H���g
	hFont	= (HFONT)GetStockObject(OEM_FIXED_FONT);
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
	lstrcat(pDesk, "\\stg\\");
	SetCurrentDirectory(pDesk);

	if (!player.init(hInst, hDC, "player.bmp", 0, 0, 0, 2)){
		MessageBox(NULL, "�摜[player]���ǂݍ��܂�܂���ł���", "�G���[", MB_OK);
		e.seterror(E_IMG_PLAYER);
		return false;
	}
	player.rset(3);	// ���x
	player.DPOINT::set(sWin.x/2, sWin.y/2);

	return true;
}

// stg �I������
void STG::exit(){
	if (hDC == NULL) return;
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(hFont);
	DeleteObject(hMBmp);
	DeleteDC(hMDC);
	ReleaseDC(hWnd, hDC);
	hDC = NULL;
}


// stg �v�Z
void STG::calc(){
	player.calc();
	
	draw();
}

// stg �`��
void STG::draw(){
	GetClientRect(hWnd, (RECT*)g_int);
	FillRect(hMDC, (RECT*)g_int, hBrush);

	player.ddraw(hMDC);

	sbuf(g_str, sizeof(g_str));
	pos(0, 0); bmes("FPS:%d", now_time+FPS);
	bmes("scr:%d x %d", ((RECT&)g_int).right, ((RECT&)g_int).bottom);
	bmes("pos:%d x %d", player.xget(), player.yget());



	validback();	// ����ʍX�V
}

