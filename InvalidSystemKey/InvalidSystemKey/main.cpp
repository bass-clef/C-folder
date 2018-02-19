
#define _WIN32_WINNT 0x0400

#include <windows.h>

#define WM_HOOKKEYDOWN	WM_APP+15

//注意 このDLLを呼び出すアプリケーションはメッセージループ
//が必ず必要です。つまりコンソールアプリケーションからは動作しません

LRESULT CALLBACK HookProc(int nCode, WPARAM wp, LPARAM lp);

class KEY {
public:
	BYTE *data;

	KEY(){	data = NULL;	}
	~KEY(){	del();	}
	void renew(UINT size){
		del();
		data = new BYTE[size];
	}
	void del(){
		if (data) delete[] data;
		data = NULL;
	}
	void operator()(UINT size){
		renew(size);
	}
	BYTE& operator[](UINT size){
		return data[size];
	}
};

class SYSTEMKEYHOOK {
	HINSTANCE hInst;
	HHOOK hhk;
	HWND hWnd;
	bool fSendParent;

public:
	SYSTEMKEYHOOK() {	hhk = NULL;	fSendParent = false;	}
	~SYSTEMKEYHOOK() {	HookEnd();	}
	HHOOK getLPHook(){	return hhk;	}

	void Instance(HINSTANCE hInst){
		this->hInst = hInst;
	}

	void HookEnd() {
		if (hhk) UnhookWindowsHookEx(hhk);
		hhk = NULL;
	}

	void HookStart(HWND hWnd=NULL) {
		if (hWnd){
			this->hWnd = hWnd;
			fSendParent = true;
		}
		HookEnd();
		//低レベルなキーフックを指定
		hhk = SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, hInst, 0);
	}

	bool send(){
		return fSendParent;
	}
	HWND parent(){
		return hWnd;
	}
};

namespace {
	SYSTEMKEYHOOK skh;
	KEY bKey;
}

void __stdcall SystemHookStart(HWND hWnd){
	skh.HookStart(hWnd);
}

void __stdcall SystemHookEnd(){
	skh.HookEnd();
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wp, LPARAM lp) {
	if(nCode==HC_ACTION){
		KBDLLHOOKSTRUCT *pK = (KBDLLHOOKSTRUCT*)lp;

		//91 : 左Windowsキー  92 : 右Windowsキー
		if(pK->vkCode==91 || pK->vkCode==92) {
			bKey(2);
			bKey[0] = 91;
			bKey[1] = 92;
			if (skh.send()) PostMessage(skh.parent(), WM_HOOKKEYDOWN, 2, (WPARAM)bKey.data);
			return TRUE;//100ms以内に0以外を返すと無効になる
		}

		//CTRL + ESC
		if (pK->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1)) {
			bKey(2);
			bKey[0] = VK_ESCAPE;
			bKey[1] = VK_CONTROL;
			if (skh.send()) PostMessage(skh.parent(), WM_HOOKKEYDOWN, 2, (WPARAM)bKey.data);
			return TRUE;
		}

		//ALT + TAB
		if (pK->vkCode == VK_TAB && pK->flags & LLKHF_ALTDOWN) {
			bKey(2);
			bKey[0] = VK_TAB;
			bKey[1] = VK_MENU;
			if (skh.send()) PostMessage(skh.parent(), WM_HOOKKEYDOWN, 2, (WPARAM)bKey.data);
			return TRUE;
		}

		//ALT + ESC
		if (pK->vkCode == VK_ESCAPE && pK->flags & LLKHF_ALTDOWN) {
			bKey(2);
			bKey[0] = VK_ESCAPE;
			bKey[1] = VK_MENU;
			if (skh.send()) PostMessage(skh.parent(), WM_HOOKKEYDOWN, 2, (WPARAM)bKey.data);
			return TRUE;
		}

		//ALT + CTRL + DEL
		if (pK->vkCode == VK_DELETE && pK->flags & LLKHF_ALTDOWN && GetAsyncKeyState(VK_CONTROL)>>((sizeof(SHORT)*8)-1) ) {
			bKey(3);
			bKey[0] = VK_DELETE;
			bKey[1] = VK_CONTROL;
			bKey[2] = VK_MENU;
			if (skh.send()) PostMessage(skh.parent(), WM_HOOKKEYDOWN, 3, (WPARAM)bKey.data);
			return TRUE;
		}
	}
	return CallNextHookEx(skh.getLPHook(), nCode, wp, lp);
}

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
	skh.Instance(hModule);
	return TRUE;
}
