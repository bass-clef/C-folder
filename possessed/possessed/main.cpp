
#include <windows.h>

DWORD ThreadLoadProc(LPVOID lpParameter){
	void (__stdcall *func) (LPVOID);
	func = (void (__stdcall*)(LPVOID))GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");
	func(lpParameter);
	return true;
}

DWORD ThreadFreeProc(LPVOID lpParameter){
	void (__stdcall *func) (LPVOID);
	func = (void (__stdcall*)(LPVOID))GetProcAddress(GetModuleHandle("Kernel32"), "FreeLibrary");
	func(lpParameter);
	return true;
}

// �X���b�h�쐬DLL�ǂݍ��� ����������
// Possessed(�X���b�h���쐬����WindowHandle, �ǂݍ���DLL);
bool Possessed(HWND hWnd, char LibPath[]){
	// �v���Z�X�n���h���擾
	DWORD dwPid;
	GetWindowThreadProcessId(hWnd, &dwPid);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == NULL) return false;

	// �v���Z�X�̉��z�A�h���X��ԓ��̃������̈�̗\��
	LPVOID lpvAdd = VirtualAllocEx(hProcess, NULL, sizeof(LibPath), MEM_COMMIT,
		PAGE_READWRITE);
	if (lpvAdd == NULL) return false;

	// �v���Z�X�̃������̈�Ƀf�[�^���������� (LibPath��)
	if (WriteProcessMemory(hProcess, lpvAdd, (void*)LibPath, sizeof(LibPath),
		NULL) == 0) return false;

	// DLL��ǂݍ��܂���X���b�h�쐬
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
		(LPTHREAD_START_ROUTINE)ThreadLoadProc,
		lpvAdd, 0, NULL);
	if (hThread == NULL) return false;
	// �X���b�h�I���҂�
	WaitForSingleObject(hThread, INFINITE);

	// �I���R�[�h�擾
	DWORD dwMod;
	GetExitCodeThread(hThread, &dwMod);

	// �㏈��
	CloseHandle(hThread);
	VirtualFreeEx(hProcess, lpvAdd, sizeof(LibPath), MEM_RELEASE);

	// DllMain�����s�����

	// DLL�J��
	hThread = CreateRemoteThread(hProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)ThreadFreeProc,
		(void*)dwMod, 0, NULL);
	if (hThread == NULL) return false;

	// �㏈��
	CloseHandle(hThread);
	CloseHandle(hProcess);

	return true;
}


BOOL WINAPI DllMain(HINSTANCE hDllInst, DWORD fdwReason, LPVOID lpvReserved){
	return TRUE;
}

