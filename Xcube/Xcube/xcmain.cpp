

#include <windows.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <tchar.h>

#pragma once
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")

#define AppName	"3D cube"
#define D3DFVF_VERTEX  (D3DFVF_XYZ | D3DFVF_DIFFUSE)	//トランスフォームされていない頂点の位置座標+ディフューズ カラー成分
#define TIME_SET 500.0f	//	立方体を回転させるスピード 1/1000秒単位
#define CUBE_SIZE 100	// 四角柱のサイズ

// structure
typedef struct _D3DVERTEXCOR_ {
	float   x,y,z;	// トランスフォームされていない頂点の位置座標
    DWORD   color;	// ディフューズ カラー成分

	void operator=(_D3DVERTEXCOR_& d3d){
		x = d3d.x;
		y = d3d.y;
		z = d3d.z;
		color = d3d.color;
	}
}   D3DVERTEXCOR;

// global variables
HWND                    hWnd;
LPDIRECT3DDEVICE9       g_pDEV = 0;
LPDIRECT3D9             pD3D;
D3DPRESENT_PARAMETERS   D3DPPWin;
D3DVERTEXCOR v1[] = {	// 前面の矩形
	{ -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, 0xFFFF0000  },	// 左上
	{  CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, v1[0].color },	// 右上
	{ -CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, v1[0].color },	// 左下
	{  CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, v1[0].color } };	// 右下
D3DVERTEXCOR v2[] = {	//右側面の矩形
	{  CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, 0xFF00FF00  },	// 右上
	{  CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, v2[0].color },	// 右上奥
	{  CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, v2[0].color },	// 右下
	{  CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, v2[0].color } };	// 右下奥
D3DVERTEXCOR v3[] = {	//上面の矩形
	{ -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, 0xFF0000FF  },	// 左上奥
	{  CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, v3[0].color },	// 右上奥
	{ -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, v3[0].color },	// 左上
	{  CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, v3[0].color } };	// 右上
D3DVERTEXCOR v4[] = {	//左側面の矩形
	{ -CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, 0xFFFFFF00  },	// 左下
	{ -CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, v4[0].color },	// 左下奥
	{ -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, v4[0].color },	// 左上
	{ -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, v4[0].color } };	// 左上奥
D3DVERTEXCOR v5[] = {	//裏面の矩形 
	{ -CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, 0xFFFF00FF  },	// 左下奥
	{  CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, v5[0].color },	// 右下奥
	{ -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, v5[0].color },	// 左上奥
	{  CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, v5[0].color } };	// 右上奥
D3DVERTEXCOR v6[] = {	//下面の矩形
	{  CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, 0xFFFFFFFF  },	// 右下奥
	{ -CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, v6[0].color },	// 左下奥
	{  CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, v6[0].color },	// 右下
	{ -CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, v6[0].color } };	// 左下

void MakeVertexCube(D3DVERTEXCOR v[6][4], float x, float y, float z, float color, float r){
	// 前面		(左上, 右上, 左下, 右下)
	v[0][0].x = x-r, y-r, z, color;
	v[0][1].x = x+r, v[0][0].y, z, color;
	v[0][2].x = v[0][0].x, y+r, z, color;
	v[0][3].x = v[0][1].x, v[0][2].y, z, color;
	// 裏面		(左下奥, 右下奥, 左上奥, 右上奥)
	v[4][0].x = v[0][2].x, v[0][2].y, z-r, color;
	v[4][1].x = v[0][3].x, v[0][3].y, v[4][0].z, color;
	v[4][2].x = v[0][0].x, v[0][0].y, v[4][0].z, color;
	v[4][3].x = v[0][1].x, v[0][1].y, v[4][0].z, color;
	// 右側面	(右上, 右上奥, 右下, 右下奥)
	v[1][0] = v[0][1], v[4][3], v[0][3], v[4][1];
	// 上面		(左上奥, 右上奥, 左上, 右上)
	v[2][0] = v[4][0], v[4][3], v[0][0], v[0][1];
	// 左側面	(左下, 左下奥, 左上, 左上奥)
	v[3][0] = v[0][2], v[4][0], v[0][0], v[4][2];
	// 下面		(右下奥, 左下奥, 右下, 左下)
	v[5][0] = v[4][1], v[4][0], v[0][3], v[0][2];
}

void DrawPrimitiveUPCube(D3DVERTEXCOR v[6][4]){
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v[0], sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v[1], sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v[2], sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v[3], sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v[4], sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v[5], sizeof(D3DVERTEXCOR));
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);
HRESULT Init3DDev(HWND hwnd);
void SetupMatrices(float x, float y, float z=0);
void Draw(void);

// メイン
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	WNDCLASSEX wce = {sizeof(wce), CS_HREDRAW | CS_VREDRAW, WindowProc,
		0, 0, NULL, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, AppName, LoadIcon(NULL, IDI_APPLICATION)
	};
	if (!RegisterClassEx(&wce)) return FALSE;

	HWND hWnd = CreateWindowEx(
		0, AppName, AppName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
		NULL, NULL, hInst, NULL
	);
    if (FAILED(Init3DDev(hWnd))) return FALSE;
	ShowWindow(hWnd, nCmd);

	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while(1){
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			// システムメイン
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUITが来てたら
			TranslateMessage(&msg);	// キーボードを利用可能にする
			DispatchMessage(&msg);	// 制御をWindowsに戻す
		}else {
			Draw();
		}
	}

	g_pDEV->Release();
	pD3D->Release();
    UnregisterClass(AppName, wce.hInstance);
	return 0;
}

// ウィンドウプロージャー
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_DESTROY:
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

//デバイス/モード等の初期化
HRESULT Init3DDev(HWND hwnd){

	hWnd = hwnd;
    pD3D = NULL;
	HRESULT hr;
	D3DDISPLAYMODE dmode;

	// デバイス/モード等の初期化
	if (!pD3D) pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!pD3D) return E_FAIL;

	//現在のディスプレイモードを得る
	if (FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&dmode)))
		return E_FAIL;

	// D3DDeviceオブジェクトの作成
	ZeroMemory(&D3DPPWin, sizeof(D3DPPWin));
	D3DPPWin.BackBufferFormat			= dmode.Format;
	D3DPPWin.BackBufferCount			= 1;
	D3DPPWin.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	D3DPPWin.Windowed					= TRUE;
	D3DPPWin.EnableAutoDepthStencil		= TRUE;
	D3DPPWin.AutoDepthStencilFormat		= D3DFMT_D16;
	D3DPPWin.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &D3DPPWin, &g_pDEV);
	if (FAILED(hr)){
		hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING, &D3DPPWin, &g_pDEV);
		if (FAILED(hr)){
			hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING, &D3DPPWin, &g_pDEV);
			if (FAILED(hr)){
				MessageBox(NULL,TEXT("Create Device Error"),TEXT("Surface Error"),MB_OK);
				return E_FAIL;
			}
        }
    }
	return S_OK;
}

//描画環境の設定
void  SetupMatrices(float x, float y, float z){
	D3DXMATRIX      matView;
    D3DXMATRIX      matProj;
    D3DXMATRIX      w;

	// 任意の軸を回転軸にして回転する行列を作成する。
	D3DXMatrixRotationYawPitchRoll(&w,
		-1.0f,	// 横
		-1.0f,	// 縦
		-1.0f);	// 円
	// timeGetTime()/TIME_SET

	// オフセットを指定して行列を作成する。
    D3DXMatrixTranslation(&matView, x, y, z);
    matView = w*matView;
    g_pDEV->SetTransform(D3DTS_VIEW, &matView);

	//透視変換の設定
	// 視野に基づいて、左手座標系パースペクティブ射影行列を作成する
    D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(45.0f), 1.0f, -300, 300);
    g_pDEV->SetTransform(D3DTS_PROJECTION, &matProj);

    //描画設定
    g_pDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);//反時計回りの面を消去
    g_pDEV->SetRenderState(D3DRS_LIGHTING, FALSE);		//ライティングしない
    g_pDEV->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);	//Zバッファ使わない
}

//描画処理
void Draw(void){
    if (!g_pDEV) return;

    //黒で塗りつぶして消去
    g_pDEV->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0, 0);

    //描画の開始
    g_pDEV->BeginScene();

	//描画環境の設定
	SetupMatrices(0, 0, 1000);

	//デバイスに使用する頂点フォーマットをセットする
	g_pDEV->SetFVF(D3DFVF_VERTEX);

	//ストリームを使わないで直接データを渡して描画する
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v1, sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v2, sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v3, sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v4, sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v5, sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v6, sizeof(D3DVERTEXCOR));

	g_pDEV->EndScene();
    g_pDEV->Present(NULL, NULL, NULL, NULL);
}
