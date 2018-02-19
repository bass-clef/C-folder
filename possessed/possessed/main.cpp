
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

// スレッド作成DLL読み込み 処理させる
// Possessed(スレッドを作成するWindowHandle, 読み込むDLL);
bool Possessed(HWND hWnd, char LibPath[]){
	// プロセスハンドル取得
	DWORD dwPid;
	GetWindowThreadProcessId(hWnd, &dwPid);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == NULL) return false;

	// プロセスの仮想アドレス空間内のメモリ領域の予約
	LPVOID lpvAdd = VirtualAllocEx(hProcess, NULL, sizeof(LibPath), MEM_COMMIT,
		PAGE_READWRITE);
	if (lpvAdd == NULL) return false;

	// プロセスのメモリ領域にデータを書き込み (LibPathを)
	if (WriteProcessMemory(hProcess, lpvAdd, (void*)LibPath, sizeof(LibPath),
		NULL) == 0) return false;

	// DLLを読み込ませるスレッド作成
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
		(LPTHREAD_START_ROUTINE)ThreadLoadProc,
		lpvAdd, 0, NULL);
	if (hThread == NULL) return false;
	// スレッド終了待ち
	WaitForSingleObject(hThread, INFINITE);

	// 終了コード取得
	DWORD dwMod;
	GetExitCodeThread(hThread, &dwMod);

	// 後処理
	CloseHandle(hThread);
	VirtualFreeEx(hProcess, lpvAdd, sizeof(LibPath), MEM_RELEASE);

	// DllMainが実行される

	// DLL開放
	hThread = CreateRemoteThread(hProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)ThreadFreeProc,
		(void*)dwMod, 0, NULL);
	if (hThread == NULL) return false;

	// 後処理
	CloseHandle(hThread);
	CloseHandle(hProcess);

	return true;
}


BOOL WINAPI DllMain(HINSTANCE hDllInst, DWORD fdwReason, LPVOID lpvReserved){
	return TRUE;
}

