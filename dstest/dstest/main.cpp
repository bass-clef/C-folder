// 極短DirectSound基盤プログラム

#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "dsound.lib" )
#pragma comment ( lib, "winmm.lib" )

#include <windows.h>
#include <tchar.h>
#include <dsound.h>
#include <mmsystem.h>

TCHAR gName[100] = _T("DirectSound Ultra Short Basical Program");

// Waveファイルオープン関数
bool openWave( TCHAR *filepath, WAVEFORMATEX &waveFormatEx, char** ppData, DWORD &dataSize ) {
        if ( filepath == 0 )
                return false;

        HMMIO hMmio = NULL;
        MMIOINFO mmioInfo;

        // Waveファイルオープン
        memset( &mmioInfo, 0, sizeof(MMIOINFO) );
        hMmio = mmioOpen( filepath, &mmioInfo, MMIO_READ );
        if( !hMmio )
                return false; // ファイルオープン失敗

        // RIFFチャンク検索
        MMRESULT mmRes;
        MMCKINFO riffChunk;
        riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
        mmRes = mmioDescend( hMmio, &riffChunk, NULL, MMIO_FINDRIFF );
        if( mmRes != MMSYSERR_NOERROR ) {
                mmioClose( hMmio, 0 );
                return false;
        }

        // フォーマットチャンク検索
        MMCKINFO formatChunk;
        formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
        mmRes = mmioDescend( hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK );
        if( mmRes != MMSYSERR_NOERROR ) {
                mmioClose( hMmio, 0 );
                return false;
        }
        DWORD fmsize = formatChunk.cksize;
        DWORD size = mmioRead( hMmio, (HPSTR)&waveFormatEx, fmsize );
        if( size != fmsize ) {
                mmioClose( hMmio, 0 );
                return false;
        }

        mmioAscend( hMmio, &formatChunk, 0 );

        // データチャンク検索
        MMCKINFO dataChunk;
        dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
        mmRes = mmioDescend( hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK );
        if( mmRes != MMSYSERR_NOERROR ) {
                mmioClose( hMmio, 0 );
                return false;
        }
        *ppData = new char[ dataChunk.cksize ];
        size = mmioRead( hMmio, (HPSTR)*ppData, dataChunk.cksize );
        if(size != dataChunk.cksize ) {
                delete[] *ppData;
                return false;
        }
        dataSize = size;

        // ハンドルクローズ
        mmioClose( hMmio, 0 );

        return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam){
        if(mes == WM_DESTROY) {PostQuitMessage(0); return 0;}
        return DefWindowProc(hWnd, mes, wParam, lParam);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
        MSG msg; HWND hWnd;
        WNDCLASSEX wcex ={sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, NULL,
                                  (HBRUSH)(COLOR_WINDOW+1), NULL, (TCHAR*)gName, NULL};
        if(!RegisterClassEx(&wcex)) return 0;

        if(!(hWnd = CreateWindow(gName, gName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
                        NULL, NULL, hInstance, NULL)))
                return 0;

        // Waveファイルオープン
        WAVEFORMATEX wFmt;
        char *pWaveData = 0;
        DWORD waveSize = 0;
        if ( !openWave( _T("D:\\Music Files\\flumpool\\ｅｘｐｅｒｉｅｎｃｅ\\01 どんな未来にも愛はある（Ｓｏｎｇ　ｗｉｔｈ　ｍｏｂａｇｅ）.wav"), wFmt, &pWaveData, waveSize ) )
                return 0;

        // サウンドデバイス作成
        IDirectSound8 *pDS8;
        DirectSoundCreate8(NULL, &pDS8, NULL);
        pDS8->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);

        DSBUFFERDESC DSBufferDesc;
        DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
        DSBufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME |
			DSBCAPS_CTRLFREQUENCY;
		// 非アクティブでも | ボリューム制御可 | 周波数制御
        DSBufferDesc.dwBufferBytes = waveSize;
        DSBufferDesc.dwReserved = 0;
        DSBufferDesc.lpwfxFormat = &wFmt;
        DSBufferDesc.guid3DAlgorithm = GUID_NULL;

        IDirectSoundBuffer *ptmpBuf = 0;
        IDirectSoundBuffer8 *pDSBuffer;
        pDS8->CreateSoundBuffer( &DSBufferDesc, &ptmpBuf, NULL );
        ptmpBuf->QueryInterface( IID_IDirectSoundBuffer8 ,(void**)&pDSBuffer );
        ptmpBuf->Release();
        if ( pDSBuffer == 0 ) {
                pDS8->Release();
                return 0;
        }

        // セカンダリバッファにWaveデータ書き込み
        LPVOID lpvWrite = 0;
        DWORD dwLength = 0;
        if ( DS_OK == pDSBuffer->Lock( 0, 0, &lpvWrite, &dwLength, NULL, NULL,
			DSBLOCK_ENTIREBUFFER ) ) {
                memcpy( lpvWrite, pWaveData, dwLength);
                pDSBuffer->Unlock( lpvWrite, dwLength, NULL, 0);
        }
        delete[] pWaveData; // 元音はもういらない

        ShowWindow(hWnd, nCmdShow);

        // 再生
        pDSBuffer->Play(0, 0, 0);

		// 音量
		LONG volume;
		pDSBuffer->GetVolume(&volume);
		pDSBuffer->SetVolume(volume/5);

		// DC・メモリDC作成
		HDC hDC = GetDC(hWnd), hMemDC;
		hMemDC = CreateCompatibleDC(hDC);
		RECT rc;
		GetClientRect(NULL, &rc);
		HBITMAP hbmp = CreateCompatibleBitmap(hDC, rc.right-rc.left, 
			rc.bottom-rc.top);
		SelectObject(hMemDC, hbmp);
		SelectObject(hMemDC, GetStockObject(BLACK_PEN));

		DWORD dwFrequency;// 周波数
		char str[128];

        // メッセージ ループ
		while(1) {
			Sleep(1);
			if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
				// システムメイン
				if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUITが来てたら
				TranslateMessage(&msg);	// キーボードを利用可能にする
				DispatchMessage(&msg);	// 制御をWindowsに戻す
			}else {
				// 周波数描画
				if (pDSBuffer->GetFrequency(&dwFrequency) == DS_OK) {
					// 画面クリア
					GetClientRect(hWnd, &rc);
					FillRect(hMemDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

					wsprintf(str, "dwFrequency = %d", dwFrequency);
					TextOut(hMemDC, 0, 0, str, lstrlen(str));
	
					// メモリDC反映
					BitBlt(hDC, 0, 0, rc.right-rc.left, rc.bottom-rc.top,
						hMemDC, 0, 0, SRCCOPY);
				}
			}
		}

		DeleteObject(hbmp);
		DeleteDC(hMemDC);
		ReleaseDC(hWnd, hDC);

        pDSBuffer->Release();
        pDS8->Release();

   return 0;
}
