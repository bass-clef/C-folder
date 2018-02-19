
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hDllInst, DWORD fdwReason, LPVOID lpvReserved){
	switch(fdwReason) {
	case DLL_PROCESS_ATTACH:
		PostQuitMessage(0);
	}
	return TRUE;
}

