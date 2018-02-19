

#include <Windows.h>
#include <string>

using namespace std;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){
	if (strncmp(lpCmd, "execute", 7)) {
		// 権限上げ上げ
		char exepath[MAX_PATH];
		if (!::GetModuleFileNameA(NULL, exepath, MAX_PATH)) {
			MessageBox(NULL, "", "エラー", MB_OK);
		}else {
			while(1) {
				HINSTANCE hInst = ::ShellExecuteA(NULL, "runas", exepath, "execute", "", SW_HIDE);
				if ((int)hInst <= 32) {
					if (IDRETRY != MessageBox(NULL, "管理者権限で起動できませんでした。", "エラー", MB_RETRYCANCEL))
						break;
				} else break;
			}
		}
		return 0;
	}

	// パッチャーの移動
	HWND hTargetWnd = ::FindWindowA(NULL, "Wizet Patcher");
	if (hTargetWnd == NULL)
		MessageBox(NULL, "パッチャーが見つかりませんでした。", "エラー", MB_OK);
	else {
		RECT rc;
		GetWindowRect(hTargetWnd, &rc);

		SetWindowPos(hTargetWnd, NULL, rc.left, rc.top-100, 0, 0, 
			SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);

		MessageBox(NULL, "正常終了", "mpm", MB_OK);
	}

	return 0;
}

