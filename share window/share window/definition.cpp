
#include "definition.h"

// �N���X�쐬
BOOL ClassInit(WNDCLASSEX &wce, char* AppName, WNDPROC WndProc){

	wce.cbSize		= sizeof(wce);	// �\���̂�size
	wce.style		= CS_HREDRAW | CS_VREDRAW;	// �X�^�C��
	wce.lpfnWndProc	= WndProc;	// �v���[�W���֐�
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
	if (!hWnd) return FALSE;
	ShowWindow(hWnd, nCmd);

	return TRUE;
}

// �G���[���b�Z�[�W�\��
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


class SELWNDINFO{
public:
	HWND hWnd;
	HDC hDC, hMemDC;
	BITMAPINFO bmi;
	HBITMAP hBmp;
	RECT rc;
	int width, height;

	void GethDC(HWND hWnd){
		hDC = ::GetDC(hWnd);
	}

	void ReleasehDC(HWND hWnd){
		ReleaseDC(hWnd, hDC);
	}

	void CreatehDIBSection(LPBITMAPINFO lpbmi, UINT usage, void** ppvBits){
		hBmp = ::CreateDIBSection(hDC, lpbmi, usage, ppvBits, NULL, 0);
	}

	void CreateCompatiblehhMemDC(){
		hMemDC = CreateCompatibleDC(hDC);
	}

	void SelectObjecthMemDChBmp(){
		SelectObject(hMemDC, hBmp);
	}

	void CopyDC(DWORD rop){
		BitBlt(hMemDC, 0, 0, width, height, hDC, 0, 0, rop);
	}

	void SetWndSize(int width, int height){
		this->width = width;
		this->height = height;
	}

	void SetMainWnd(HWND hWnd){
		this->hWnd = hWnd;
	}

	template <class T> void GetWndSize(T width, T height){
		width = (T)this->width;
		height = (T)this->height;
	}
};

namespace {
	SELWNDINFO wi;
	BITMAPINFO bmi;
}

// �f�X�N�g�b�vbits�擾
void WatchWindow(HWND hWnd, HWND hTarget, LPDWORD &lpPixel){
	wi.SetMainWnd(hWnd);

	// �X�N���[�����擾
	GetClientRect(hTarget, &wi.rc);
	wi.SetWndSize((int)wi.rc.right, (int)wi.rc.bottom);

	// DIB�̏���ݒ�
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	wi.GetWndSize(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	// DIB Section �̍쐬
	wi.GethDC(hWnd);
	wi.CreatehDIBSection(&bmi, DIB_RGB_COLORS, (void**)&lpPixel);
	wi.CreateCompatiblehhMemDC();
	wi.SelectObjecthMemDChBmp();
	wi.ReleasehDC(hWnd);

	// �X�N���[���� DIB Section �ɃR�s�[
	wi.GethDC(hTarget);
	wi.CopyDC(SRCCOPY);
	wi.ReleasehDC(hTarget);
}

void SetClientSize(HWND hWnd, int width, int height){
	RECT rc = { 0, 0, width, height };

	::AdjustWindowRectEx(&rc, GetWindowLong(hWnd, GWL_STYLE), false, 0);
	::SetWindowPos(hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top, 
		SWP_NOMOVE | SWP_NOZORDER);

}

