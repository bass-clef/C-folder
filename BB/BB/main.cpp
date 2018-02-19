


#include "blackboard.h"


BOOL CALLBACK WindowProc(HWND hWnd, UINT uMsg, LPARAM lp, WPARAM wp);

#ifndef DEBUG_
int main(){
	char str[128];

	cout << "•Ï”–¼" << endl;
	cin >> str;

	BlackBoard bb;
	bb.Error(bb.addvar(str, lstrlen(str), BVT_STR));

	char con[128];
	bb.Error(bb.setcon(0, con, lstrlen(con)));
	

	cout << "•Ï”ˆê——:" << endl;
	bb.varview();

	cin >> str;

	return 0;
}
#else
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	return 0;
}
#endif

BOOL CALLBACK WindowProc(HWND hWnd, UINT uMsg, LPARAM lp, WPARAM wp){
	switch(uMsg){
	case WM_DESTROY:
		PostQuitMessage(0);
	}
	return 0;
}
