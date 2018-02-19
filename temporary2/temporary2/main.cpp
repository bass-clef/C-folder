
#include <Windows.h>
//#define noquit

#ifdef noquit
class TEST {
public:
	TEST() {
		::MessageBoxA(NULL, "�R���X�g���N�^", "test", MB_OK);
	}

	~TEST() {
		::MessageBoxA(NULL, "�f�X�g���N�^", "test", MB_OK);
	}
};
#endif

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
	MessageBox(NULL, "����", "", MB_OK);
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

// �E�B���h�E�v���[�W���[
LRESULT __stdcall WindowProc(HWND hWnd, UINT uMsg , WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_DESTROY: PostQuitMessage(0); return 0;

	#ifdef noquit
	case WM_KEYDOWN:
		if (wp == 'Q') PostQuitMessage(0);
		return 0;
	case WM_QUIT:
	case WM_CLOSE:
		::MessageBoxA(hWnd, "�I���ł��Ȃ���[�񂗂�", "test", MB_OK);
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

// �N���X�쐬
BOOL ClassInit(HINSTANCE hInst, WNDCLASSEX &wce, char* AppName){

	wce.style		= CS_HREDRAW | CS_VREDRAW;	// �X�^�C��
	wce.lpfnWndProc	= WindowProc;	// �v���[�W���֐�
	wce.cbClsExtra	= wce.cbWndExtra = 0;
	wce.hInstance	= hInst;	// �v���O�����̃n���h��
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
		0, AppName, AppName, WS_OVERLAPPEDWINDOW - WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
		NULL, NULL, hInstance, NULL
	);
	if (!hWnd)return FALSE;
	ShowWindow(hWnd, nCmd);

	return TRUE;
}

// �V�X�e�����C��
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	#ifdef noquit
	TEST test;
	#endif

	WNDCLASSEX wce;
	wce.cbSize		= sizeof(wce);	// �\���̂�size
	if (! ClassInit(hInst, wce, "testing")) return 0;
	
	HWND hWnd;
	if (! WindowInit(hInst, hWnd, nCmd, "testing")) return 0;

	#ifdef noquit
	::MessageBoxA(NULL, "�E�B���h�E�̏�������", "test", MB_OK);
	#else
	setwh(hWnd, 400, 40);
	HDC hDC = GetDC(hWnd);
	SelectObject(hDC, GetStockObject(OEM_FIXED_FONT));
	char str[128];

	wsprintf(str, "�I���������E�B���h�E�ɃJ�[�\�������킹��");
	TextOut(hDC, 5, 0, str, lstrlen(str));
	wsprintf(str, "�d���������L�[�������Ă�������");
	TextOut(hDC, 5, 20, str, lstrlen(str));
	#endif
	
	MSG msg;
	while(1) {
		// �N���X�o�^�ƃE�B���h�E�̍쐬
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			// �V�X�e�����C��
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUIT�����Ă���
			TranslateMessage(&msg);	// �L�[�{�[�h�𗘗p�\�ɂ���
			DispatchMessage(&msg);	// �����Windows�ɖ߂�
		}
		Sleep(1);
	}

	#ifdef noquit
	::MessageBoxA(NULL, "�㏈���̊���", "test", MB_OK);
	#else
	ReleaseDC(hWnd, hDC);
	#endif

	return 0;
}



