
#include "definition.h"
#include <process.h>
#include <string>

namespace {
	// ���̃t�@�C����̃O���[�o���ϐ�
	HWND hWnd;
	HDC hMemDC;
	LPDWORD lpMemory = NULL;
	bool quit = false, redraw = false, wait = true;
	unsigned int width, height, scsize;
	static char AppName[] = "Share Window";

	enum SERVERTYPE {HOST=0, CLIENT};
	const bool boottype = HOST;	// 0:host 1:client
	unsigned short PORT = 12900;
}


class Socket{
	WSADATA wsaData;
	SOCKET sock, clsock;
	sockaddr_in addr, dstAddr;
	int addr_family, len, type;
	bool boottype;
	size_t n;
	std::string ip;
	fd_set fds, rfds, wfds, efds;
	timeval tv;

	// �G���[��
	SOCKET IsError(SOCKET sock, const char *from){
		if (sock == INVALID_SOCKET){
			char str[32];
			int err = WSAGetLastError();
			switch(err){
			case WSAEINTR:			return WSAEINTR;
			case WSAECONNREFUSED:	return WSAECONNREFUSED;
			}

			wsprintf(str, "%s : %d \0", from, err);
	
			MessageBox(NULL, str, "socket error", MB_OK);
			quit = true;
			return (sock == INVALID_SOCKET);
		}

		return 0;
	}

public:
	void SetIPAddress(char *addr){
		ip.assign(addr, 0, lstrlen(addr));
	}
	int Init(WORD wVarsionRequested, int af, u_short nPort, bool boottype){
		int err;
		sock = 0;
		clsock = 0;
		addr_family = af;
		this->boottype = boottype;

		// winsock ������
		err = WSAStartup(wVarsionRequested, &wsaData);
		if (err){
			MessageBox(NULL, "socket�������ł��܂���ł���", "socket error", MB_OK);
			return -1;
		}

		// �ڑ���̃A�h���X����ݒ�
		memset(&dstAddr, 0, sizeof(sockaddr_in));

		// socket ���� (TCP)
		if ( IsError(sock = socket(af, SOCK_STREAM, 0), "socket") ) return -1;

		// socket �̐ݒ�
		if (boottype) {
			// client
//			size_t iplen = ip.length();
//			char *dest = new char[iplen+1];
//			memcpy_s(dest, iplen, ip.data(), iplen);
//			dest[iplen] = NULL;

			addr.sin_family = af;
			addr.sin_port = htons(nPort);
			addr.sin_addr.S_un.S_addr = inet_addr(ip.data());
			if (addr.sin_addr.S_un.S_addr == 0xFFFFFFFF) {
				hostent *host = gethostbyname(ip.data());
				if (host == NULL){
					IsError(INVALID_SOCKET, "hostnull");
					return -1;
				}
			}
//			delete[] dest;
		}else {
			// host
			addr.sin_family = af;
			addr.sin_port = htons(nPort);
			addr.sin_addr.S_un.S_addr = INADDR_ANY;
			if ( IsError(bind(sock, (sockaddr*)&addr, sizeof(addr)), "bind") ) return -1;

			// TCP�N���C�A���g����̐ڑ��v����҂Ă��Ԃɂ���
			if ( IsError(listen(sock, 5), "listen") ) return -1;
		}

		return 0;
	}

	~Socket(){
		closesocket(sock);
		WSACleanup();
	}

	int Begin(){
		len = sizeof(dstAddr);
		if (boottype) {
			// client
			RETRY:
			switch(IsError(connect(sock, (sockaddr*)&addr, len), "connect")){
			case 0:
				break;
			case WSAECONNREFUSED:
				switch ( MessageBox(NULL,
					"�T�[�o�[�̏����������Ă��܂���B\n�Đڑ����܂����H",
					AppName, MB_RETRYCANCEL) ){
				case IDRETRY:
					goto RETRY;
				}
				return -1;
			default:
				return -1;
			}
		}else {
			// host
			if ( IsError(clsock = accept(sock, (sockaddr*)&dstAddr, &len), "accept") )
				return -1;
		}

		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		if (this->boottype == CLIENT)	FD_SET(sock, &rfds);
		else					FD_SET(clsock, &rfds);
		if (this->boottype == CLIENT)	FD_SET(sock, &wfds);
		else					FD_SET(clsock, &wfds);
		if (this->boottype == CLIENT)	FD_SET(sock, &efds);
		else					FD_SET(clsock, &efds);

		return 0;
	}

	// false:read  true:write
	int check(bool frw = false, long time = NULL){
		timeval *ptv = NULL;
		if (time) {
			ptv = &tv;
			ptv->tv_sec = time/1000;
			ptv->tv_usec = time%1000;
		}

		memcpy(&rfds, &fds, sizeof(fd_set));
		memcpy(&wfds, &fds, sizeof(fd_set));
		memcpy(&efds, &fds, sizeof(fd_set));
		n = select(0, &rfds, &wfds, &efds, ptv);
		if (0 < n) {
			if (frw)
				if (this->boottype == CLIENT)	n = FD_ISSET(sock, &wfds);
				else							n = FD_ISSET(clsock, &wfds);
			else
				if (this->boottype == CLIENT)	n = FD_ISSET(sock, &rfds);
				else							n = FD_ISSET(clsock, &rfds);
			
			/*
			if (this->boottype == CLIENT){
				if (FD_ISSET(sock, &efds)) ;
			} else
				if (FD_ISSET(clsock, &efds)) ;
			*/
		}
		return n;
	}

	int Socket::send(char *pt, size_t size){
		if (size == 0) return -1;
		if (boottype == CLIENT)	n = ::send(sock, pt, size, 0);
		else					n = ::send(clsock, pt, size, 0);

		if (n < size) { IsError(SOCKET_ERROR, "send"); return -1; }
		return n;
	}
	// result when sockclose is 0
	int Socket::recv(char *pt, size_t size, int flags){
		// 0 | MSG_OOB | MSG_PEEK | MSG_WAITALL
		if (boottype == CLIENT)	
			n = ::recv(sock, pt, size, flags);
		else					n = ::recv(clsock, pt, size, flags);

		if (n == -1) { IsError(SOCKET_ERROR, "receive"); return -1; }
		if (n == WSAEOPNOTSUPP) { IsError(SOCKET_ERROR, "receive"); return WSAEOPNOTSUPP; }

		return n;
	}

	void Socket::close(){
		shutdown(sock, SD_BOTH);
		closesocket(sock);
	}
};

unsigned int WINAPI DrawMain(void* pParam){
	if (boottype == CLIENT) {
		HDC hDC = GetDC(hWnd);
		char str[64];
		wsprintf(str, "�z�X�g�ɑ��M��\0");
		TextOut(hDC, 0, 0, str, lstrlen(str));
		ReleaseDC(hWnd, hDC);
		redraw = true;
		return 0;
	}

	// DIB �̏���ݒ�
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	HDC hDC = GetDC(hWnd);
	HBITMAP hBmp = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS,
		(void**)&lpMemory, NULL, 0);
	if (lpMemory == NULL) {
		MessageBox(NULL, "DIB���쐬����܂���ł����B", AppName, MB_OK);
		quit = true;
		return 1;
	}
	hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBmp);

	// DIB : x[�ʏ�] y[����0]
	for(size_t y=0; y<height; y++) for(size_t x=0; x<width; x++) {
		lpMemory[y*width+x] = 0xFFFFFFFF;
	}

	redraw = true;
	const char str[] = "�N���C�A���g����摜����M��...";
	TextOut(hMemDC, 0, 0, str, lstrlen(str));

	while(1){
		if (quit) break;
		Sleep(1);
		if (redraw) {
			redraw = false;
			InvalidateRect(hWnd, NULL, FALSE);
		}
	}

	quit = true;
	ReleaseDC(hWnd, hDC);
	DeleteObject(hBmp);
	DeleteDC(hMemDC);

	return 0;
}

unsigned int WINAPI SocketMain(void* pParam){
	Socket *sock = (Socket*)pParam;
	if (sock->Init(MAKEWORD(2, 0), AF_INET, PORT, boottype)) {
		quit = true;
		return 1;
	}

	if (sock->Begin()) {
		quit = true;
		return 1;
	}

	char str[64] = "�ʐM�ҋ@��...\0";
	unsigned int cnt=0;
	SetWindowText(hWnd, str);

/*	while(1) {
		Sleep(1);
		if (quit) break;
		if (redraw == true) break;
	}
	/**/
	if (quit) return 1;

	wsprintf(str, "��M��...\0");
	SetWindowText(hWnd, str);
	cnt = 0;
	while(1) {
		Sleep(1);
		if (quit) break;

		SetWindowText(hWnd, str);

		if (boottype == CLIENT) {
			// client
			WatchWindow(hWnd, GetDesktopWindow(), lpMemory);
			sock->send((char*)lpMemory, scsize);
			cnt++;
			wsprintf(str, "���M�� : %d\0", cnt);
		}else {
			// host
			if (!sock->recv((char*)lpMemory, scsize, MSG_WAITALL)) break;
			redraw = true;
			cnt++;
			wsprintf(str, "��M�� : %d\0", cnt);
		}
	}
	MessageBox(hWnd, "�ؒf���܂����B", AppName, MB_OK);
	quit = true;
	return 0;
}

LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){

	switch(uMsg){
	case WM_PAINT:{
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hWnd, &ps);
		BitBlt(hDC, 0, 0, width, height, hMemDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){

	// �E�B���h�E������
	WNDCLASSEX wce;
	if (!ClassInit(wce, AppName, WindowProc)) return -1;
	if (!WindowInit(hInst, hWnd, nCmd, AppName)) return -1;

	// please set default window size here
	width = 1366;
	height = 768;
	scsize = width*height*4;
	if (boottype == HOST)
		SetClientSize(hWnd, width, height);
	else
		SetClientSize(hWnd, 256, 60);


	// �\�P�b�g������ & �T�u�X���b�h�N��
	Socket sock;
	HANDLE hThread[2], hMutex;
	sock.SetIPAddress("192.168.0.5");	// ���㉽���ɕς���
	hMutex = CreateMutex(NULL, false, NULL);
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, SocketMain, &sock, 0, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, DrawMain, hWnd, 0, NULL);

	MSG msg;
	while(1){
		if (quit) break;
		Sleep(1);
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			// �V�X�e�����C��
			if (!GetMessage(&msg, NULL, 0, 0)) break;// WM_QUIT�����Ă���
			TranslateMessage(&msg);	// �L�[�{�[�h�𗘗p�\�ɂ���
			DispatchMessage(&msg);	// �����Windows�ɖ߂�
		}else {
		}
	}

	quit = true;
	Sleep(1);

	// �T�u�X���b�g�I���ҋ@
	WaitForMultipleObjects(2, hThread, true, INFINITE);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hMutex);
	sock.close();

	return 0;
}

