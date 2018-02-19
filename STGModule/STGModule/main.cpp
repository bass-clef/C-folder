
#include <windows.h>
#include <Mmsystem.h>
#include <math.h>
#include <String>

#pragma comment(lib, "WinMM.Lib")

#define MAX			LONG_MAX
#define	PI			3.141592653589793
#define	MY_SPEED	4

// HSPのシステム情報
#define objkazz 64
struct BMSCR {
    //      Bitmap buffer structure
    //
    int     flag;           // used flag
    int     sx;             // X-size
    int     sy;             // Y-size
    int     palmode;        // palmode
    HDC     hdc;            // buffer HDC
    BYTE    *pBit;          // bitmap pointer
    BITMAPINFOHEADER *pbi;  // infoheader
    HBITMAP dib;            // bitmap handle(DIB)
    HBITMAP old;            // bitmap handle(OLD)
    RGBQUAD *pal;           // palette table
    HPALETTE hpal;          // palette handle
    HPALETTE holdpal;       // palette handle (old)
    int     pals;           // palette entries
    HWND    hwnd;           // window handle
    HANDLE  hInst;          // Instance of program
    int     infsize;        // *pbi alloc memory size
    int     bmpsize;        // *pBit alloc memory size

    //      Window object setting
    //
    int     type;           // setting type
    int     wid;            // window ID
    short   fl_dispw;       // display window flag
    short   fl_udraw;       // update draw window
    int     wx,wy,wchg;     // actual window size x,y
    int     xx,yy;          // buffer view point x,y
    int     lx,ly;          // buffer view size x,y
    int     cx,cy;          // object cursor x,y
    int     ox,oy,py;       // object size x,y,py
    int     texty;          // text Y-axis size
    int     gx,gy,gmode;    // gcopy size

    HBRUSH  hbr;            // BRUSH handle
    HPEN    hpn;            // PEN handle
    HFONT   hfont;          // FONT handle
    HFONT   holdfon;        // FONT handle (old)
    COLORREF color;         // text color code

    HANDLE  hCld[objkazz];  // buttonのhandle
    int     owid[objkazz];  // buttonのjump ID
    int     owb;            // handleのindex

    int     textspeed;      // slow text speed
    int     cx2,cy2;        // slow text cursor x,y
    int     tex,tey;        // slow text limit x,y
    char    *prtmes;        // slow message ptr

};

class SOUND {
protected:
	int nfiles;		// ファイルの数
	char **sound;	// メモリデータ
	char **files;	// ファイル名 配列

public:
	bool loadsound(char **file, int fnLen){
		HANDLE	hFile;
		DWORD	sFile, sRead;

		nfiles = fnLen;
		for(int i=0; i<nfiles; i++){
			hFile = CreateFile(file[i], GENERIC_READ, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) return true;
			sFile = GetFileSize(hFile, NULL);
			sound[i] = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sFile);
			ReadFile(hFile, (LPVOID)sound[i], sFile, &sRead, NULL);
			CloseHandle(hFile);
		}
		return false;
	}
};

// STGのシステム情報
class STG : public SOUND {
protected:
	int frame;		// ゲームが起動してからのフレーム
	int score;		// 点数
	int	enemy;		// 敵の数

public:
	STG(){
		frame = 0;
		score = 0;
		enemy = 0;
	}

	void	frameAdd()			{ frame++; }
	void	frameSet(int frame) { this->frame = frame; }
	int		frameGet()			{ return frame; }
	void	scoreAdd(int add)	{ score += add; }
	void	scoreSub(int sub)	{ score -= sub; if (score < 0) score = 0; }
	void	scoreSet(int score)	{ this->score = score; }
	int		scoreGet()			{ return score; }
	void	enemyAdd(int add)	{ enemy += add; }
	int		enemySub(int sub)	{ enemy -= sub; if (enemy < 0) enemy = 0; }
	void	enemySet(int enemy)	{ this->enemy = enemy; }
	int		enemyGet()			{ return enemy; }
};

// 体力
class HP {
protected:
	int hp;		// 現在の値
	int hpmax;	// 最大の値
	int sub;	// 減少する値
	int add;	// 加算する値

public:
	void set(int hp = MAX, int hpmax = MAX) {
		if (   hp != MAX) this->hp = hp;
		if (hpmax != MAX) this->hpmax = hpmax;
	}
	void	hpSet(int hp)		{ this->hp = hp; }
	int		hpGet()				{ return hp; }
	int		hpxGet()			{ return hpmax; }
	void	subSet(int sub)		{ this->sub = sub; }
	int		subGet()			{ return sub; }
	void	addSet(int add)		{ this->add = add; }
	int		addGet()			{ return add; }

};

// 座標
#define DPOINT CPOINT<double>
template <class T> class CPOINT {
protected:
	T x, y;	// 座標

public:
	void set(T x = MAX, T y = MAX) {
		if (x != MAX) this->x = x;
		if (y != MAX) this->y = y;
	}
	T xGet()		{ return x; }
	void xSet(T x)	{ this->x = x; }
	T yGet()		{ return y; }
	void ySet(T y)	{ this->y = y; }
};

// イベント
#define DEVENT EVENT<double>
template<class T> class EVENT {
public:
	T prm;
	int st;		// 動作開始フレーム
	int et;		// 動作開始フレーム
};

// 基クラス (座標, フレーム, フラグ, 角度, 速度, 画像ID, 画像サイズx, y)
class STGBASE {
protected:
	DPOINT p;		// 座標
	int frame;		// 動いてからのフレーム
	char flag;		// フラグ
	double a, r;	// 角度・速度
	int iix, iiy;	// 画像ID
	int isx, isy;	// 画像の大きさ

public:
	void init(int frame = 0, char flag = 0, int isx = 1, int isy = 1, int iix = 0, int iiy = 0, double a = 0, double r = 0) {
		this->frame	= frame;
		this->flag	= flag;
		this->a		= a;
		this->r		= r;
		this->iix	= iix;
		this->iiy	= iiy;
		this->isx	= isx;
		this->isy	= isy;
	}
	DPOINT	dpget()				{ return p; }
	void	frameAdd()			{ frame ++; }
	void	frameSet(int frame)	{ this->frame = frame; }
	int		frameGet()			{ return frame; }
	void	flagSet(int flag)	{ this->flag = flag; }
	int		flagGet()			{ return flag; }
	void	aSet(double a)		{ this->a = a; }
	double	aGet()				{ return a; }
	void	rSet(double a)		{ this->r = r; }
	double	rGet()				{ return r; }
	void	iixSet(int iix)		{ this->iix = iix; }
	int		iixGet()			{ return iix; }
	void	iiySet(int iiy)		{ this->iiy = iiy; }
	int		iiyGet()			{ return iiy; }
	void	isxSet(int iix)		{ this->isx = isx; }
	int		isxGet()			{ return isx; }
	void	isySet(int iiy)		{ this->isy = isy; }
	int		isyGet()			{ return isy; }
};

// 機体
class BODY : public STGBASE, public HP {
protected:
	int	nshot;		// 弾数
	int cshot;		// 弾カウント
	int csmod;		// 弾カウント剰余
	// イベント
	DEVENT *evm;	// 動き

public:
	BODY*	thisget()			{ return this; }
	void	nshotSet(int nshot)	{ this->nshot = nshot; }
	int		nshotGet()			{ return nshot; }
};
class ENEMY : public BODY {
};
class MY : public BODY {
public:
	MY(int csmod) { nshot = 0; cshot = 0; this->csmod = csmod; }
	// 計算 (自機の移動、当たり判定)
	void calc();
	// 移動 -> 移動している最中にオブジェクトに遭遇するか
	int move(int way, BODY *bd, int dp1Len = 0, double sx = 0, double dy = 0);
	// 当たり判定trueでする処理
	void hit(int nEnemy, BODY *bd);
};

// 弾
class SHOT : STGBASE {
	// イベント
	DEVENT *evap;	// 角度加算
	DEVENT *evam;	// 角度減算
	DEVENT *evrp;	// 速度加算
	DEVENT *evrm;	// 速度減算
};
class MSHOT : public SHOT {
};
class ESHOT : public SHOT {
};

// ラジアン -> 度
inline double deg(double r){
	return (r * 180.0 / PI);
}

// 度 -> ラジアン
inline double rad(double d){
	return (d * PI / 180.0);
}

// 2点間の角度
inline double a2point(DPOINT pt, DPOINT pt2){
	return deg(atan2(pt2.yGet()-pt.yGet(), pt2.xGet()-pt.xGet()));
}

// 2点間の距離 
inline double r2point(DPOINT pt, DPOINT pt2){
	return sqrt(pow(pt2.xGet()-pt.xGet(), 2)+pow(pt2.yGet()-pt.yGet(), 2));
}

void MY::hit(int nEne, BODY *bd) {
	// 敵のHP減少
	bd[nEne].hpSet( bd[nEne].hpGet()-bd[nEne].subGet() );

	// 自機のHP減少
	this->hp -= this->subGet();
}

// 自機移動 自機移動最中に
int MY::move(int way, BODY *bd, int dp1Len, double sx, double dy) {
	int f = 1;
	for(register int j=0; j<r; j++){
		switch(way) {
		case 1: p.xSet(p.xGet()-1); break;
		case 3: p.xSet(p.xGet()+1); break;
		case 2: p.ySet(p.yGet()-1); break;
		case 4: p.ySet(p.yGet()+1); break;
		}

		for(register int i=0; i<dp1Len; i++) {
			if (bd[i].flagGet() > 0)	// 敵が活動中
				if ( (int)r2point(p, bd[i].dpget() ) ) {// あたった
					hit(i, bd);
					f = 1;
				}
		}
	}
	return f;
}


// グローバル変数
BMSCR		sys;
STG			stg;
MY			my(15);
MSHOT		mys;
ENEMY		*ene;
ESHOT		*enes;

// 初期化
void Initialize(BMSCR c_sys, char **soundfiles, int soundmax ){
	sys = c_sys;
	my.init();
	stg.loadsound(soundfiles, soundmax);
}

// 自機計算
void MY::calc(){
	int f = 0;
	BYTE key[256];

	GetKeyboardState(key);	// キー状態取得

	// 自機移動
	if (key[VK_LEFT] & 0x80) {
		f = my.move(1, ene->thisget(), stg.enemyGet());
	}if (key[VK_RIGHT] & 0x80) {
		f = my.move(3, ene->thisget(), stg.enemyGet());
	}if (key[VK_UP] & 0x80) {
		f = my.move(2, ene->thisget(), stg.enemyGet());
	}if (key[VK_DOWN] & 0x80) {
		f = my.move(4, ene->thisget(), stg.enemyGet());
	}

	// 自機弾 (基本ショット)
	if ((key['X'] & 0x80) || (key['Z'] & 0x80)) {
		if (my.rGet() == MY_SPEED) my.rSet(MY_SPEED/2);


	}else if (my.rGet() == MY_SPEED/2) {
		my.rSet(MY_SPEED);
	}
}

void Calculate(){
	my.calc();
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD fdwReason,LPVOID lpvReserved){
	return TRUE;
}
