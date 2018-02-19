
// EnumWindows

#include <windows.h>

BOOL WINAPI DllMain( HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved){
	WS_MINIMIZEBOX;
	return true;
}

void __stdcall MessageErrorBox(){
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
