

#include <windows.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <tchar.h>

#pragma once
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")

#define AppName	"3D cube"
#define D3DFVF_VERTEX  (D3DFVF_XYZ | D3DFVF_DIFFUSE)	//�g�����X�t�H�[������Ă��Ȃ����_�̈ʒu���W+�f�B�t���[�Y �J���[����
#define TIME_SET 500.0f	//	�����̂���]������X�s�[�h 1/1000�b�P��
#define CUBE_SIZE 100	// �l�p���̃T�C�Y

// structure
typedef struct _D3DVERTEXCOR_ {
	float   x,y,z;	// �g�����X�t�H�[������Ă��Ȃ����_�̈ʒu���W
    DWORD   color;	// �f�B�t���[�Y �J���[����

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
D3DVERTEXCOR v1[] = {	// �O�ʂ̋�`
	{ -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, 0xFFFF0000  },	// ����
	{  CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, v1[0].color },	// �E��
	{ -CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, v1[0].color },	// ����
	{  CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, v1[0].color } };	// �E��
D3DVERTEXCOR v2[] = {	//�E���ʂ̋�`
	{  CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, 0xFF00FF00  },	// �E��
	{  CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, v2[0].color },	// �E�㉜
	{  CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, v2[0].color },	// �E��
	{  CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, v2[0].color } };	// �E����
D3DVERTEXCOR v3[] = {	//��ʂ̋�`
	{ -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, 0xFF0000FF  },	// ���㉜
	{  CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, v3[0].color },	// �E�㉜
	{ -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, v3[0].color },	// ����
	{  CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, v3[0].color } };	// �E��
D3DVERTEXCOR v4[] = {	//�����ʂ̋�`
	{ -CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, 0xFFFFFF00  },	// ����
	{ -CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, v4[0].color },	// ������
	{ -CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, v4[0].color },	// ����
	{ -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, v4[0].color } };	// ���㉜
D3DVERTEXCOR v5[] = {	//���ʂ̋�` 
	{ -CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, 0xFFFF00FF  },	// ������
	{  CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, v5[0].color },	// �E����
	{ -CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, v5[0].color },	// ���㉜
	{  CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, v5[0].color } };	// �E�㉜
D3DVERTEXCOR v6[] = {	//���ʂ̋�`
	{  CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, 0xFFFFFFFF  },	// �E����
	{ -CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, v6[0].color },	// ������
	{  CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, v6[0].color },	// �E��
	{ -CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, v6[0].color } };	// ����

void MakeVertexCube(D3DVERTEXCOR v[6][4], float x, float y, float z, float color, float r){
	// �O��		(����, �E��, ����, �E��)
	v[0][0].x = x-r, y-r, z, color;
	v[0][1].x = x+r, v[0][0].y, z, color;
	v[0][2].x = v[0][0].x, y+r, z, color;
	v[0][3].x = v[0][1].x, v[0][2].y, z, color;
	// ����		(������, �E����, ���㉜, �E�㉜)
	v[4][0].x = v[0][2].x, v[0][2].y, z-r, color;
	v[4][1].x = v[0][3].x, v[0][3].y, v[4][0].z, color;
	v[4][2].x = v[0][0].x, v[0][0].y, v[4][0].z, color;
	v[4][3].x = v[0][1].x, v[0][1].y, v[4][0].z, color;
	// �E����	(�E��, �E�㉜, �E��, �E����)
	v[1][0] = v[0][1], v[4][3], v[0][3], v[4][1];
	// ���		(���㉜, �E�㉜, ����, �E��)
	v[2][0] = v[4][0], v[4][3], v[0][0], v[0][1];
	// ������	(����, ������, ����, ���㉜)
	v[3][0] = v[0][2], v[4][0], v[0][0], v[4][2];
	// ����		(�E����, ������, �E��, ����)
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

// ���C��
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
			// �V�X�e�����C��
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUIT�����Ă���
			TranslateMessage(&msg);	// �L�[�{�[�h�𗘗p�\�ɂ���
			DispatchMessage(&msg);	// �����Windows�ɖ߂�
		}else {
			Draw();
		}
	}

	g_pDEV->Release();
	pD3D->Release();
    UnregisterClass(AppName, wce.hInstance);
	return 0;
}

// �E�B���h�E�v���[�W���[
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){
	switch(uMsg){
	case WM_DESTROY:
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, uMsg, wp, lp);
}

//�f�o�C�X/���[�h���̏�����
HRESULT Init3DDev(HWND hwnd){

	hWnd = hwnd;
    pD3D = NULL;
	HRESULT hr;
	D3DDISPLAYMODE dmode;

	// �f�o�C�X/���[�h���̏�����
	if (!pD3D) pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!pD3D) return E_FAIL;

	//���݂̃f�B�X�v���C���[�h�𓾂�
	if (FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&dmode)))
		return E_FAIL;

	// D3DDevice�I�u�W�F�N�g�̍쐬
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

//�`����̐ݒ�
void  SetupMatrices(float x, float y, float z){
	D3DXMATRIX      matView;
    D3DXMATRIX      matProj;
    D3DXMATRIX      w;

	// �C�ӂ̎�����]���ɂ��ĉ�]����s����쐬����B
	D3DXMatrixRotationYawPitchRoll(&w,
		-1.0f,	// ��
		-1.0f,	// �c
		-1.0f);	// �~
	// timeGetTime()/TIME_SET

	// �I�t�Z�b�g���w�肵�čs����쐬����B
    D3DXMatrixTranslation(&matView, x, y, z);
    matView = w*matView;
    g_pDEV->SetTransform(D3DTS_VIEW, &matView);

	//�����ϊ��̐ݒ�
	// ����Ɋ�Â��āA������W�n�p�[�X�y�N�e�B�u�ˉe�s����쐬����
    D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(45.0f), 1.0f, -300, 300);
    g_pDEV->SetTransform(D3DTS_PROJECTION, &matProj);

    //�`��ݒ�
    g_pDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);//�����v���̖ʂ�����
    g_pDEV->SetRenderState(D3DRS_LIGHTING, FALSE);		//���C�e�B���O���Ȃ�
    g_pDEV->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);	//Z�o�b�t�@�g��Ȃ�
}

//�`�揈��
void Draw(void){
    if (!g_pDEV) return;

    //���œh��Ԃ��ď���
    g_pDEV->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0, 0);

    //�`��̊J�n
    g_pDEV->BeginScene();

	//�`����̐ݒ�
	SetupMatrices(0, 0, 1000);

	//�f�o�C�X�Ɏg�p���钸�_�t�H�[�}�b�g���Z�b�g����
	g_pDEV->SetFVF(D3DFVF_VERTEX);

	//�X�g���[�����g��Ȃ��Œ��ڃf�[�^��n���ĕ`�悷��
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v1, sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v2, sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v3, sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v4, sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v5, sizeof(D3DVERTEXCOR));
	g_pDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v6, sizeof(D3DVERTEXCOR));

	g_pDEV->EndScene();
    g_pDEV->Present(NULL, NULL, NULL, NULL);
}
