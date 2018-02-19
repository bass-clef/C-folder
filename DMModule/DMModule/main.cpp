
#include <Windows.h>
#include <shobjidl.h>
#include <locale.h>
#include <Gdiplus.h>

namespace {
	THUMBBUTTON *buttons;//�\����
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

	// WCHAR�ϊ�
	setlocale(LC_ALL, "japanese");
	size_t retSize;
	mbstowcs_s(&retSize, buttons[ButtonID].szTip, 260, szTip, strlen(szTip));

	buttons[ButtonID].dwFlags = THBF_ENABLED;
	
	return true;
}

bool MakeTaskBar(HWND hWnd, int nLen){
	if ((!nLen) || (!hWnd)) return false;

	// �O���[�o���ϐ�
	hMWnd = hWnd;
	nButton = nLen;

	// �{�^���\���̏�����
	buttons = new THUMBBUTTON[nLen];

	// �^�X�N�o�[�̏�����
	CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList3, (void**)&ptl);

	return true;
}

HRESULT ApplyTaskBarButton(){
	return ptl->ThumbBarAddButtons(hMWnd, nButton, buttons);
}


//������
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

//32�r�b�g�^�r�b�g�t�B�[���h�̐ݒ�
static void InitBitFields32BIT(LPRGBQUAD pRGB)
{
//RGBQUAD��DWORD�͓����T�C�Y�ł��B�r�b�g�t�B�[���h���w�肷��ꍇ��LPDWORD�ɃL���X�g��������y�ł��i16�r�b�g���ł͓��Ɂj
LPDWORD pBF = reinterpret_cast<LPDWORD>(pRGB);
pBF[0] = 0xff0000;
pBF[1] = 0x00ff00;
pBF[2] = 0x0000ff;
}

//DDB����DIB�ւ̌݊��R�s�[
static HBITMAP ConvertToDIB(HBITMAP hDDB)
{
//�ϊ���DDB�̏������
BITMAP bitmap = {0};
::GetObject(hDDB, sizeof(bitmap), &bitmap);
//�w�b�_�̃T�C�Y�iRGBQUAD * 3�́A�r�b�g�t�B�[���h�p�ɕK�v�ł��j
const int nHDRSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 3);
LPBITMAPINFO pbmi = static_cast<LPBITMAPINFO>(::calloc(nHDRSize, 1));
//32�r�b�g�p�̃w�b�_���쐬����
::InitHDR32BIT(pbmi->bmiHeader, bitmap.bmWidth, bitmap.bmHeight);
//32�r�b�g�p�̃t�B�[���h��ݒ肷��
::InitBitFields32BIT(pbmi->bmiColors);

//DIB�Z�N�V�����̍쐬
LPVOID pBits = NULL;
HBITMAP hDIB = ::CreateDIBSection(NULL, pbmi, DIB_RGB_COLORS, &pBits, NULL, NULL);
//���I�Ɋ��蓖�Ă��w�b�_���������
::free(pbmi);

//DIB�pHDC
HDC hDCDIB = ::CreateCompatibleDC(NULL);
//DDB�pHDC
HDC hDCDDB = ::CreateCompatibleDC(NULL);
//���ꂼ���HDC�ɑI��
::SelectObject(hDCDIB, hDIB);
::SelectObject(hDCDDB, hDDB);
//DDB����DIB�֌݊��R�s�[
::BitBlt(hDCDIB, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hDCDDB, 0, 0, SRCCOPY);
//���ꂼ���HDC�����
::DeleteDC(hDCDIB);
::DeleteDC(hDCDDB);
//�o���オ��
return hDIB;
}

HBITMAP CreateImage(HDC hDC, int x, int y, UINT width, UINT height){
	// DIB�̃T�C�Y
	DWORD imgsize = width*height;

	// �r�b�g�}�b�v�I�u�W�F�N�g�̍쐬
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

	// �r�b�g�}�b�v�ɕ`��
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
		// �A�N�e�B�u�ɂ���
		SetActiveWindow(hWnd);
	else {
		// ���C���[�h�E�B���h�E�쐬
		::SetWindowLongA(hWnd, GWL_EXSTYLE,
			GetWindowLongA(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
//		hWnd = CreateWindowEx(WS_EX_LAYERED, "hspwnd0", "",
//			WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
//			x, y, width, height, NULL, NULL, hInst, NULL);
	}

	//�r�b�g�}�b�v�I�u�W�F�N�g���쐬���邩�����̃r�b�g�}�b�v�I�u�W�F�N�g���g�p���邩�ǂ����̃t���O
	HBITMAP hBmp;
	if (flag) hBmp = (HBITMAP)hObj;
	else hBmp = CreateImage((HDC)hObj, ipx, ipy, width, height);

	// DC��BMP��K�p
	HDC hDC = GetDC(hWnd), hMemDC;
	hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBmp);

	// �E�B���h�E�T�C�Y
	SIZE size = {width, height};

	// �f�o�C�X�R���e�L�X�g�ɂ����郌�C���̈ʒu
	POINT pt = {0, 0};

	// ���C���[�E�B���h�E�\���̓����x�̒l
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA};
	
	// ���C���[�E�B���h�E�ɂ���
	::UpdateLayeredWindow(hWnd, hDC, NULL, &size, hMemDC, &pt, 0, &bf, ULW_ALPHA);

	// �\��
	ShowWindow(hWnd, SW_SHOW);

	// �J������
	ReleaseDC(hWnd, hDC);
	if (!flag) DeleteObject(hBmp);
	DeleteDC(hMemDC);
}
*/





BOOL WINAPI DllMain(HINSTANCE hDllInst, DWORD fdwReason, LPVOID lpvReserved){
	
	switch(fdwReason){
	case DLL_PROCESS_ATTACH:  // ���[�h���ꂽ
		hMWnd = NULL;
		buttons = NULL;
		break;
	case DLL_PROCESS_DETACH:  // �A�����[�h���ꂽ
		if (buttons != NULL) delete[] buttons;
		break;
	}
	return TRUE;
}
