
#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

namespace {
	const char table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
}

// APIエラーメッセージ表示
void ErrorMessageBox(){
	LPVOID lpMsgBuf;

	FormatMessage(//エラー表示文字列作成
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR)&lpMsgBuf, 0, NULL
	);

	MessageBox(NULL, (const char*)lpMsgBuf, NULL, MB_OK);	//メッセージ表示
	LocalFree(lpMsgBuf);
}


// 手前にchar追加関数
void addchar(char *buf, char byte){
	int len = strlen(buf);
	for(int i=len; 0<i; i--) buf[i] = buf[i-1];
	buf[0] = byte;
}

// n進数交互変換関数
const char* nBase(char *buf, int base, int prm){
	int dst = prm, rem = 0;
	memset(buf, 0, 128);

	while(1){
		if (dst == 0) break;
		rem = dst % base;
		if (rem >= 128) break;
		addchar(buf, table[rem]);
		dst = dst / base;
	}
	if (strlen(buf) == 0) return "0";
	return buf;
}
long nBase_(int base, char *buf){
	const int len = strlen(buf);
	long dst = 0, i = len-1, n = 0;
	for(long cnt=0; cnt<len; cnt++){
		n = 0;
		for(int cnt2=0; cnt2<base; cnt2++)
			if (table[cnt2] == buf[cnt]){ n = cnt2; break; }
		dst += (long)(n*powl(base, i));
		i--;
	}
	return dst;
}

// ファイル選択ダイアログ
int FileSelectDialog(char *filter, char *fName, DWORD flag, HWND hWnd, size_t fnMax, bool type){
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize	= sizeof(OPENFILENAME);
	ofn.hwndOwner	= hWnd;
	ofn.lpstrFilter	= filter;
	ofn.lpstrFile	= fName;
	ofn.nMaxFile	= fnMax;
	ofn.Flags		= OFN_ENABLESIZING | OFN_EXPLORER | OFN_NONETWORKBUTTON | flag;
	ofn.lpstrDefExt	= "*.*";
	if (type) {
		ofn.lpstrTitle	= "ファイルを開く";
		return GetOpenFileName(&ofn);
	}else{
		ofn.Flags	|= OFN_OVERWRITEPROMPT;
		ofn.lpstrTitle	= "名前をつけて保存";
		return GetSaveFileName(&ofn);
	}
	return 0;
}
int inline OpenDialog(char *filter, char *fName, size_t fnMax, DWORD flag = 0, HWND hWnd = NULL){
	return FileSelectDialog(filter, fName, flag, hWnd, fnMax, 1);
}
int inline SaveDialog(char *filter, char *fName, size_t fnMax, DWORD flag = 0, HWND hWnd = NULL){
	return FileSelectDialog(filter, fName, flag, hWnd, fnMax, 0);
}

int encrypt(char fName[MAX_PATH]){
	HANDLE hFile = CreateFile(fName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return 0;

	DWORD fSize = GetFileSize(hFile, NULL);
	if (fSize != 0xFFFFFFFF){
		char *buf = new char[fSize];
		DWORD rSize, hw = (DWORD)sqrt((double)fSize)+1;

		// ファイル読み込み
		ReadFile(hFile, buf, fSize, &rSize, NULL);
		// メモリDC作成
		HDC hMDC = CreateCompatibleDC(NULL);
		HBITMAP hBitmap = CreateCompatibleBitmap(NULL, hw, hw);

		// 暗号化
		char str[256];
		int j, r;
		DWORD xx = 0, yy = 0;
		for(size_t i=0; i<fSize; i++){
			nBase(str, 2, buf[i]);
			for(j=0; j<lstrlen(str); j++){
				if (str[j] == '0') r = rand()%127; else r = 128+rand()%127;
				SetPixel(hMDC, xx, yy, RGB(r, r, r));
				xx++;
				if (xx >= hw) { xx = 0; yy++; }
			}
			SetPixel(hMDC, xx, yy, RGB(127, 127, 127));
			xx++;
			if (xx >= hw) { xx = 0; yy++; }
		}



		delete[] buf;
	}

	CloseHandle(hFile);
	return fSize;
}


int main(){
	char fName[MAX_PATH*2];
	if (!OpenDialog("*.*", fName, sizeof(fName), 0, FindWindow("ConsoleWindowClass",NULL)))
		ErrorMessageBox();

	//if (encrypt(fName) < 0) cout << "エラー" << endl;
	char buf[256];
	cout << "[" << nBase(buf, 2, 255) << "]" << endl;

	cout << "何か入力してください" << endl;
	cin >> fName;
}
