

#include <windows.h>
#include <stdlib.h>

#pragma comment(lib, "winmm.lib")

#define MI_H(x) (x * 65535 / (GetSystemMetrics(SM_CXSCREEN) - 1))
#define MI_V(y) (y * 65535 / (GetSystemMetrics(SM_CYSCREEN) - 1))


// global variables
namespace {
	char AppName[] = "Anti [fruit shop]";
	const int xsize = 8;
	const int ysize = 8;
	const int sizemax = xsize*ysize;
	const int nx = 51, ny = 45;
	const int nxh = nx/2, nyh = ny/2;
	POINT sWin = {nx*xsize, ny*ysize};
}

enum FRUITS {
	FS_NONOR = 0,
	FS_APPLE,
	FS_ORENGE,
	FS_GRAPE,
	FS_KIWI,
	FS_WMELON,
	FS_CHERRY,
	FS_STRAW,
	FS_RANDOM,
	FS_VOID
};

enum WAY{
	WAY_CENTER=0,
	WAY_LEFT=1,
	WAY_UP=2,
	WAY_RIGHT=4,
	WAY_DOWN=8,
	WAY_RANDOM=16
};

class fsinfo{
public:
	unsigned char p[2];	// 位置
	char way;	// 方向

	fsinfo(){
		p[0] = 0, 0;
		way = WAY_CENTER;
	}
	void addway(WAY w){
		if (!(way & w)) way += w;
	}
	void setxy(unsigned char x, unsigned char y){
		p[0] = x;
		p[1] = y;
	}
	unsigned char x(){ return (char)p[0]; }
	unsigned char y(){ return (char)p[1]; }
};

void __stdcall ErrorMessageBox(){
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

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_KEYDOWN:
		RECT rc;
		if ((37 <= wp) && (wp <= 40)) GetWindowRect(hWnd, &rc);
		switch(wp){
		case 37: rc.left--; rc.right--; SetWindowPos(hWnd, 0, rc.left, rc.top, rc.right, rc.bottom, 0x35); break;
		case 39: rc.left++; rc.right++; SetWindowPos(hWnd, 0, rc.left, rc.top, rc.right, rc.bottom, 0x35); break;
		case 38: rc.top--; rc.bottom--; SetWindowPos(hWnd, 0, rc.left, rc.top, rc.right, rc.bottom, 0x35); break;
		case 40: rc.top++; rc.bottom++; SetWindowPos(hWnd, 0, rc.left, rc.top, rc.right, rc.bottom, 0x35); break;
		}
		break;
	case WM_LBUTTONDOWN:{
		if (HIWORD(lp) <= 20) SendMessage(hWnd, WM_NCLBUTTONDOWN, 2, 0);
		break;
	}
	case WM_DESTROY: PostQuitMessage(0); return 0;
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){
	HWND	hWnd;
	char str[64];

	#pragma region /// クラス・ウィンドウ の作成
	WNDCLASSEX wce;
	wce.cbSize		= sizeof(wce);	// 構造体のsize
	wce.style		= CS_HREDRAW | CS_VREDRAW;	// スタイル
	wce.lpfnWndProc	= WindowProc;	// プロージャ関数
	wce.cbClsExtra	= wce.cbWndExtra = 0;
	wce.hInstance	= hInst;		// プログラムのハンドル
	wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// アイコン
	wce.hCursor		= LoadCursor(NULL, IDC_ARROW);		// マウスカーソル
	wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// ブラシ
	wce.lpszMenuName	= NULL;		// メニュー
	wce.lpszClassName	= AppName;	// クラス名
	wce.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	if (! RegisterClassEx(&wce)){ ErrorMessageBox(); return 1; }

	hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST,
		AppName, AppName,
		WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, sWin.x, sWin.y+20,
		NULL, NULL, NULL, NULL
	);
	if (hWnd == NULL) { ErrorMessageBox(); return 1; }
	SetLayeredWindowAttributes(hWnd, 0xFFFFFF, 255, LWA_COLORKEY);
	ShowWindow(hWnd, SW_SHOW);
	#pragma endregion

	// デスクトップウォッチ
	RECT rc;
	GetClientRect(hWnd, &rc);
	HDC hDC = GetDC(hWnd), hDeskDC = GetDC(GetDesktopWindow());
	if (hDC == NULL){ ErrorMessageBox(); return 0; }
	if (hDeskDC == NULL) { ErrorMessageBox(); return 0; }
	HDC hDeskMDC = CreateCompatibleDC(hDeskDC);
	HBITMAP hDeskMBmp = CreateCompatibleBitmap(hDeskDC, rc.right, rc.bottom);
	SelectObject(hDeskMDC, hDeskMBmp);

	#pragma region// fruit shop search
	HWND hIEWnd = FindWindow("IEFrame", NULL);
	if (hIEWnd == NULL) { MessageBox(hWnd, "IE・fruit shopが起動されてません\n終了します", "Anti fruit shop", MB_OK); return 0; }
	{
		COLORREF clr;
		unsigned char r, g, b;
		int hit = 0, xx, yy;

		RETRY:
		MessageBox(hWnd, "fruit shop を検索します。\n全体が見えるようにスクロールしてください。", "Anti fruit shop", MB_OK);
		GetWindowRect(hIEWnd, &rc);
		BitBlt(hDeskMDC, 0, 0, 300, 300, hDeskDC, rc.left, rc.top, SRCCOPY);
		hit = 0; xx = 0; yy = 0;
		for(int y=0; y<rc.bottom; y++)
			for(int x=0; x<rc.right; x++){
			if (hit == 9) break;
			clr = GetPixel(hDeskMDC, x, y);
			r = GetRValue(clr);
			g = GetGValue(clr);
			b = GetBValue(clr);
			switch(hit){// [赤 -> 白 -> 赤 -> 白 -> 赤 -> 白 -> 赤 -> 白 - > 赤] 検知
			case 0:// red
				if ((r > g) && (r > b)) { hit = 1; xx = x; yy = y; }
				break;
			case 1:// white
				if ((r > 220) && (g > 220) && (b > 220)) hit = 2;
				else if (!((r > g) && (r > b))) hit = 0;
				break;
			case 2:// red
				if ((r > g) && (r > b)) hit = 3;
				else if (!((r > 220) && (g > 220) && (b > 220))) hit = 0;
				break;
			case 3:// white
				if ((r > 220) && (g > 220) && (b > 220)) hit = 4;
				else if (!((r > g) && (r > b))) hit = 0;
				break;
			case 4:// red
				if ((r > g) && (r > b)) hit = 5;
				else if (!((r > 220) && (g > 220) && (b > 220))) hit = 0;
				break;
			case 5:// white
				if ((r > 220) && (g > 220) && (b > 220)) hit = 6;
				else if (!((r > g) && (r > b))) hit = 0;
				break;
			case 6:// red
				if ((r > g) && (r > b)) hit = 7;
				else if (!((r > 220) && (g > 220) && (b > 220))) hit = 0;
				break;
			case 7:// white
				if ((r > 220) && (g > 220) && (b > 220)) hit = 8;
				else if (!((r > g) && (r > b))) hit = 0;
				break;
			case 8:// red
				if ((r > g) && (r > b)) hit = 9;
				else if (!((r > 220) && (g > 220) && (b > 220))) hit = 0;
				break;
			}

		}
		if (hit == 9){
			RECT ierc;
			GetWindowRect(hIEWnd, &ierc);
			GetClientRect(hWnd, &rc);
			rc.left = xx+ierc.left+8;
			rc.top = yy+ierc.top+20;
			rc.right = rc.left+rc.right;
			rc.bottom = rc.top+rc.bottom;
			SetWindowPos(hWnd, 0, rc.left, rc.top, rc.right, rc.bottom, 0x35);
		}else {
			if (IDRETRY == MessageBox(hWnd,
				"fruit shop が見つかりませんでした。\n屋根の全体が見えるようにスクロールしてください。",
				"Anti fruit shop", MB_RETRYCANCEL)) {

			}else return 0;
			goto RETRY;
		}
	}
	#pragma endregion

	// メモリDC作成(自画面用)
	HDC hMDC = CreateCompatibleDC(hDC);
	HBITMAP hMBmp = CreateCompatibleBitmap(hDC, rc.right, rc.bottom);
	SelectObject(hMDC, hMBmp);

	SelectObject(hMDC, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
	HBRUSH hBrush = CreateSolidBrush(0);
	SelectObject(hMDC, hBrush);
	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 128, 128));
	SelectObject(hMDC, hPen);

	PatBlt(hMDC, 0, 0, rc.right, rc.bottom, WHITENESS);
	SetTextColor(hMDC, RGB(254, 254, 254));
	SetBkColor(hMDC, RGB(0, 0, 0));

	int nonor = 0;				// 不正常フラグ
	int xx = 0, yy = 0;			// 座標
	int winx = -1, winy = -1;	// ウィンドウ移動検知
	int fsimax = 0, fsimo = 0, fsimo2 = 0;		// fsiの数
	bool fsic = false;
	unsigned char r = 0, g = 0, b = 0, a = 0;	// 赤, 緑, 青, 平均
	COLORREF clr;		// 色
	int count = 0, cm = 10, cmax = 0, calcc = 0;	// 更新カウント
	char center = 0;	// トークン中央値
	char mleft = -1, mup = 0, mright = 0, mdown = 0;	// more -> m
	char left = -2, up = -3, right = -4, down = -5;
	char fruit[xsize][ysize];		// フルーツの種類
	char fruito[xsize][ysize][6];	// ランダム用
	char miss = 0;			// MISSIONフルーツの種類
	fsinfo fsi[sizemax*10];	// 消せる番号
	fsinfo del;

	bool autof = true;
	int num = 0, autoc = 0;

	for(unsigned char y=0; y<ysize; y++) for(unsigned char x=0; x<xsize; x++) {
		fruit[x][y] = 1; fruito[x][y][0] = -1, -1, -1, -1, 0;
	}

	MSG msg;
	while(1) {
		Sleep(1);
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			// システムメイン
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUITが来てたら
			TranslateMessage(&msg);	// キーボードを利用可能にする
			DispatchMessage(&msg);	// 制御をWindowsに戻す
		}else {
			// get quit key
			if (GetAsyncKeyState(27)) PostQuitMessage(0);
			// automatic
			if (GetAsyncKeyState(37)) autoc++;
			if (autoc > 10) {
				if (autof) autof = false; else autof = true;
				autoc = 0;
			}
			if (GetAsyncKeyState(190)) if (cm) cm--;
			if (GetAsyncKeyState(188)) if (cm<100) cm++;

			count++;
			if (count >= cm){
				winx = rc.left; winy = rc.top;
				count = 0; cmax++;

				// title bar
				GetClientRect(hWnd, &rc);
				FillRect(hMDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
				rc.bottom = 20;
				FillRect(hMDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
				if (nonor > 4){
					wsprintf(str, "正常に検知されてません  検査フレーム %3d  mission : ", cm);
				}else wsprintf(str, "消せる数 :%3d  検査フレーム %3d mission : ", fsimax, cm);

				// mission 表示
				#pragma region// missionフルーツの判別
				GetWindowRect(hWnd, &rc);
				BitBlt(hDeskMDC, 0, 0, 1, 1, hDeskDC, rc.left+445, rc.top+260, SRCCOPY);
				clr = GetPixel(hDeskMDC, 0, 0);
				r = GetRValue(clr);
				g = GetGValue(clr);
				b = GetBValue(clr);
				miss = FS_NONOR;
				if ((r == 248) && (g == 238) && (b == 163)) miss = FS_APPLE; else
				if ((r == 255) && (g == 190) && (b ==  52)) miss = FS_ORENGE; else
				if ((r ==  81) && (g ==  56) && (b == 123)) miss = FS_GRAPE; else
				if ((r == 105) && (g == 165) && (b ==  88)) miss = FS_KIWI; else
				if ((r == 250) && (g ==  65) && (b ==  42)) miss = FS_WMELON; else
				if ((r ==  56) && (g ==  10) && (b == 196)) miss = FS_CHERRY; else
					miss = FS_STRAW;
				#pragma endregion
				switch(miss){
				case FS_APPLE:	lstrcat(str, "リンゴ"); break;
				case FS_ORENGE:	lstrcat(str, "オレンジ"); break;
				case FS_GRAPE:	lstrcat(str, "ブドウ"); break;
				case FS_KIWI:	lstrcat(str, "キウイ"); break;
				case FS_WMELON:	lstrcat(str, "スイカ"); break;
				case FS_CHERRY:	lstrcat(str, "チェリー?"); break;
				case FS_STRAW:	lstrcat(str, "イチゴ"); break;
				}
				if (!autof) lstrcat(str, " Auto mode");
				// title ber text
				TextOut(hMDC, 10, 2, str, lstrlen(str));

				#pragma region// 色判定・識別
				// デスクトップをメモリにコピー
				GetWindowRect(hWnd, &rc);
				BitBlt(hDeskMDC, 0, 0, rc.right, rc.bottom, hDeskDC, rc.left, rc.top+20, SRCCOPY);
				nonor = sizemax;
				for(unsigned char y=0; y<ysize; y++)
					for(unsigned char x=0; x<xsize; x++){
					// 色判定
					xx = x*nx; yy = y*ny;
					clr = GetPixel(hDeskMDC, xx+nx/2-3, yy+ny/2+8);
					r = GetRValue(clr);
					g = GetGValue(clr);
					b = GetBValue(clr);
					a = (r+g+b)/3;

					// 果物判別
					fruit[x][y] = FS_NONOR;
					wsprintf(str, "%d:", a);
					if ((r > g) && (r > b)){// 赤
						lstrcat(str, "赤");
						if ((100 <= a) && (a <= 120)) fruit[x][y] = FS_STRAW; else 
						if ((150 <= a) && (a <= 155)) fruit[x][y] = FS_ORENGE; else
						if (223 <= a) fruit[x][y] = FS_VOID;
					}else if ((g > r) && (g > b)){// 緑
						lstrcat(str, "緑");
						if (( 95 <= a) && (a <= 105)) fruit[x][y] = FS_WMELON; else 
						if ((115 <= a) && (a <= 140)) fruit[x][y] = FS_KIWI; else
						if ((155 <= a) && (a <= 180)) fruit[x][y] = FS_APPLE; else
						if (223 <= a) fruit[x][y] = FS_VOID;
					}else if ((b > r) && (b > g)){// 青
						lstrcat(str, "青");
						if (( 65 <= a) && (a <= 100)) fruit[x][y] = FS_GRAPE; else 
						if ((105 <= a) && (a <= 115)) fruit[x][y] = FS_CHERRY; else
						if (223 <= a) fruit[x][y] = FS_VOID;
					}else {// 白
						if (200 <= a) fruit[x][y] = FS_VOID;
					}
					// random search
					left = -5; up = -6; right = -7; down = -8;
					if (x > 0)			left	= fruit[x-1][y  ]; else left	= -5;
					if (x < xsize-1)	right	= fruit[x+1][y  ]; else right	= -6;
					if (y > 0)			up		= fruit[x  ][y-1]; else up		= -7;
					if (y < ysize-1)	down	= fruit[x  ][y+1]; else down	= -8;
					if ((left == fruito[x][y][0]) && (up == fruito[x][y][1]) && (right == fruito[x][y][2]) && (down == fruito[x][y][3]) && (fruit[x][y] != fruito[x][y][4])) {
						if (fruito[x][y][5] > 1){
							fruito[x][y][4] = fruit[x][y];
							fruit[x][y] = FS_RANDOM;
							if (x > 0)		{ fruit[x-1][y  ] = FS_VOID; }
							if (x < xsize-1){ fruit[x+1][y  ] = FS_VOID; }
							if (y > 0)		{ fruit[x  ][y-1] = FS_VOID; }
							if (y < ysize-1){ fruit[x  ][y+1] = FS_VOID; }
							fruito[x][y][0] = FS_VOID;
							fruito[x][y][1] = FS_VOID;
							fruito[x][y][2] = FS_VOID;
							fruito[x][y][3] = FS_VOID;
						}else {
							fruito[x][y][5] ++;
						}
					}else {
						fruito[x][y][0] = left;
						fruito[x][y][1] = up;
						fruito[x][y][2] = right;
						fruito[x][y][3] = down;
						fruito[x][y][4] = fruit[x][y];
						fruito[x][y][5] = 0;
					}
					if (fruit[x][y] != FS_NONOR) nonor--;
					//TextOut(hMDC, xx, yy+ny+7, str, lstrlen(str));

					// 案内表示 do
					wsprintf(str, "");
					switch(fruit[x][y]){
					case FS_APPLE:	lstrcat(str, "リンゴ"); break;
					case FS_ORENGE:	lstrcat(str, "オレンジ"); break;
					case FS_GRAPE:	lstrcat(str, "ブドウ"); break;
					case FS_KIWI:	lstrcat(str, "キウイ"); break;
					case FS_WMELON:	lstrcat(str, "スイカ"); break;
					case FS_CHERRY:	lstrcat(str, "チェリー?"); break;
					case FS_STRAW:	lstrcat(str, "イチゴ"); break;
					case FS_VOID:
						lstrcat(str, "空");
						if (0 < x) fruit[x-1][y] = FS_VOID;
						if (0 < y) fruit[x][y-1] = FS_VOID;
						if (x < xsize-1) fruit[x+1][y] = FS_VOID;
						if (y < ysize-1) fruit[x][y+1] = FS_VOID;
					}
					//TextOut(hMDC, xx, yy+ny+7, str, lstrlen(str));	//do
				}

				#pragma endregion

				#pragma region// 思考ルーチン
				for(int i=0; i<fsimax; i++) {
					fsi[i].setxy(0, 0); fsi[i].way = WAY_CENTER;
				}
				fsimax = 0; fsimo = 0;
				center = 0;
				mleft = -1; mright = -2; mup = -3; mdown = -4;
				left = -5; up = -6; right = -7; down = -8;
				for(unsigned char y=0; y<ysize; y++) for(unsigned char x=0; x<xsize; x++){
					if (fruit[x][y] == FS_NONOR) continue;
					if (fruit[x][y] == FS_VOID) continue;
					if (fruit[x][y] == FS_RANDOM){
						fsi[fsimax].setxy(x, y);
						fsi[fsimax].way = WAY_RANDOM;
						fsimax++;
						break;
					}
					// more -
					if (x > 1)			mleft	= fruit[x-2][y  ]; else mleft	= -1;
					if (x < xsize-2)	mright	= fruit[x+2][y  ]; else mright	= -2;
					if (y > 1)			mup		= fruit[x  ][y-2]; else mup		= -3;
					if (y < ysize-2)	mdown	= fruit[x  ][y+2]; else mdown	= -4;
					// normal
					if (x > 0)			left	= fruit[x-1][y  ]; else left	= -5;
					if (x < xsize-1)	right	= fruit[x+1][y  ]; else right	= -6;
					if (y > 0)			up		= fruit[x  ][y-1]; else up		= -7;
					if (y < ysize-1)	down	= fruit[x  ][y+1]; else down	= -8;

					fsimo = fsimax;

					#pragma region// 横 ver.
					if ((mleft == left) && (left == right))// 最右 □□○=□
						fsi[fsimo].addway(WAY_RIGHT);
					else if ((left == right) && (right == mright))// 最左 □=○□□
						fsi[fsimo].addway(WAY_LEFT);
					if ((mleft == left) && (left == down))// 左左下 --_
						fsi[fsimo].addway(WAY_DOWN);
					else if ((mleft == left) && (left == up))// 左左上 --~
						fsi[fsimo].addway(WAY_UP);
					if ((left == down) && (down == right))// 左下右 -_-
						fsi[fsimo].addway(WAY_DOWN);
					else if ((left == up) && (up == right))// 左上右 -~-
						fsi[fsimo].addway(WAY_UP);
					if ((down == right) && (right == mright))// 下右右 _--
						fsi[fsimo].addway(WAY_DOWN);
					else if ((up == right) && (right == mright))// 上右右 ~--
						fsi[fsimo].addway(WAY_UP);
					#pragma endregion
					#pragma region// 縦 var.
					if ((up == down) && (down == mdown))// 最上 i
						fsi[fsimo].addway(WAY_UP);
					else if ((mup == up) && (up == down))// 最下 !
						fsi[fsimo].addway(WAY_DOWN);
					if ((mup == up) && (up == left))// 上上左 」
						fsi[fsimo].addway(WAY_LEFT);
					else if ((mup == up) && (up == right))// 上上右 no image
						fsi[fsimo].addway(WAY_RIGHT);
					if ((up == left) && (left == down))// 上下左 ＜
						fsi[fsimo].addway(WAY_LEFT);
					else if ((up == right) && (right == down))// 上下右 ＞
						fsi[fsimo].addway(WAY_RIGHT);
					if ((down == mdown) && (mdown == left))// 下下左 no image
						fsi[fsimo].addway(WAY_LEFT);
					else if ((down == mdown) && (mdown == right))// 下下右 「
						fsi[fsimo].addway(WAY_RIGHT);
					#pragma endregion
	
					if (fsi[fsimo].way != WAY_CENTER) {
						fsi[fsimo].setxy(x, y);
						fsimax++;
					}
				}
				#pragma endregion

				#pragma region // 削除できる項目表示
				for(int i=0; i<fsimax; i++){
					// 枠描画
					xx = fsi[i].p[0]*nx; yy = fsi[i].p[1]*ny;
					rc.left = xx;
					rc.top = yy+20;
					rc.right = xx+nx;
					rc.bottom = yy+ny+20;
					FrameRect(hMDC, &rc, hBrush);
	
					// 矢印表示
					wsprintf(str, "");
					if (fsi[i].way & WAY_LEFT)	lstrcat(str, "←");
					if (fsi[i].way & WAY_UP)	lstrcat(str, "↑");
					if (fsi[i].way & WAY_RIGHT)	lstrcat(str, "→");
					if (fsi[i].way & WAY_DOWN)	lstrcat(str, "↓");
					if (fsi[i].way & WAY_RANDOM) wsprintf(str, "◎");
					TextOut(hMDC, xx, yy+20, str, lstrlen(str));
				}
				#pragma endregion

				#pragma region // オート処理
				if (calcc) {
					switch(calcc){
					case 1000:// 移動
						GetWindowRect(hWnd, &rc);
						xx = 0; yy = 0;
						for(int i=0; i<fsimax; i++)
							if ((del.p[0] == fsi[i].p[0]) && (del.p[1] == fsi[i].p[1]) && (del.way == fsi[i].way)) {
								xx = del.p[0] * nx+nx/2 + rc.left + 5;
								yy = del.p[1] * ny+ny/2 + rc.top+20 + 5;
								mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, MI_H(xx), MI_V(yy), 0, NULL);
								break;
							}
						if ((xx == 0) && (yy == 0)) calcc = 2;
						break;
					case 999:// クリック押し下げ
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
						if (del.way & WAY_RANDOM)
							// ランダムは移動しないですぐLEFTUP
							del.way = WAY_CENTER;
						break;
					case 997:// 移動
						GetWindowRect(hWnd, &rc);
						xx = del.p[0] * nx+nx/2 + rc.left + 5;
						yy = del.p[1] * ny+ny/2 + rc.top+20 + 5;
						if (del.way & WAY_LEFT)		xx -= nx; else
						if (del.way & WAY_RIGHT)	xx += nx; else
						if (del.way & WAY_UP)		yy -= ny; else
						if (del.way & WAY_DOWN)		yy += ny;
						mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, MI_H(xx), MI_V(yy), 0, NULL);
						break;
					case 996:// クリック離す
						mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
						break;
					case 995:// 移動
						GetWindowRect(hWnd, &rc);
						mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, MI_H(rc.right), MI_V(rc.bottom), 0, NULL);
						calcc = 2;
						break;
					}
					calcc--;
				}else {
					if (fsimax) {// 優先順位・削除項目決定
						if (fsimo2 != fsimax) { fsimo2 = fsimax; num = 0; }
						if (miss != FS_NONOR){
							for(int i=0; i<fsimax; i++){
								xx = fsi[i].p[0]; yy = fsi[i].p[1];
								if (fsi[i].way & WAY_RANDOM) { num = i-1; break; }
								if (fsi[i].way & WAY_LEFT)	if (fruit[xx-1][yy  ] == miss) {num = i-1; break; }
								if (fsi[i].way & WAY_UP)	if (fruit[xx  ][yy-1] == miss) {num = i-1; break; }
								if (fsi[i].way & WAY_RIGHT)	if (fruit[xx+1][yy  ] == miss) {num = i-1; break; }
								if (fsi[i].way & WAY_DOWN)	if (fruit[xx  ][yy+1] == miss) {num = i-1; break; }
							}
						}
						num++;
						if (num >= fsimax) num = 0;

						// 削除実行
						if (!autof) if (nonor <= 4){
							del.setxy(fsi[num].p[0], fsi[num].p[1]);
							del.way = fsi[num].way;
							calcc = 1003;
						}
					}
				}
				#pragma endregion
			}

			// 外枠
			GetClientRect(hWnd, &rc);
			rc.top += 20;
			FrameRect(hMDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
			// メモリ反映
			BitBlt(hDC, 0, 0, rc.right, rc.bottom, hMDC, 0, 0, SRCCOPY);
		}
	}

	ReleaseDC(GetDesktopWindow(), hDeskDC);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(hMBmp);
	DeleteDC(hMDC);
	ReleaseDC(hWnd, hDC);
	return 0;
}
