
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <string>
#include <process.h>

enum SOCKETTYPE{ TCP=0, UDP };
enum SERVERTYPE{ HOST=0, CLIENT };
class Socket{
	SOCKET sock, clsock;
	sockaddr_in addr, dstAddr;
	WSADATA wsaData;
	WSABUF wsabuf;
	int addr_family, len;
	bool boottype, sockettype;
	size_t n;
	std::string ip, gip;
	fd_set fds, rfds, wfds, efds;
	timeval tv;

	// �G���[��
	SOCKET IsError(SOCKET sock, const char *from){
		if (sock == INVALID_SOCKET){
			char str[32];
			int err = WSAGetLastError();
			if (err){
				wsprintf(str, "%s : %d ", from, err);

				err = Error(err);
				if (err) return err;
	
				MessageBox(NULL, str, "socket error", MB_OK);
			}
			Quit();
			return (sock == INVALID_SOCKET);
		}

		return 0;
	}

public:
	void SetIPAddress(const char *ipaddress){
		ip.assign(ipaddress);
	}
	const char* ipget(){
		char buf[1024];
		DWORD d;
		if (!WSAIoctl(sock, SIO_ADDRESS_LIST_QUERY, NULL, 0, buf, 1024,
			&d, NULL, NULL)){
				SOCKET_ADDRESS_LIST *slist = (SOCKET_ADDRESS_LIST*)buf;
				sockaddr *so = NULL;
				for(int i=0; i<slist->iAddressCount; i++){
					so = (sockaddr*)(slist->Address[i].lpSockaddr);
					if (so->sa_family == AF_INET){
						sockaddr_in *s =
							(sockaddr_in*)(slist->Address[i].lpSockaddr);
						
						ip.assign(inet_ntoa(s->sin_addr));
					}
				}
		}
		return ip.c_str();
	}

	const char* getip(){
		return this->ip.c_str();
	}
	int Init(WORD wVarsionRequested, int af, u_short nPort, bool sockettype, bool boottype){
		int err;
		sock = 0;
		clsock = 0;
		addr_family = af;
		this->boottype = boottype;
		this->sockettype = sockettype;

		// winsock ������
		err = WSAStartup(wVarsionRequested, &wsaData);
		if (err){
			MessageBox(NULL, "socket�������ł��܂���ł���", "socket error", MB_OK);
			return -1;
		}

		// �ڑ���̃A�h���X����ݒ�
		memset(&dstAddr, 0, sizeof(sockaddr_in));

		// socket ����
		if (sockettype == TCP) {
			// TCP
			if ( IsError(sock = socket(af, SOCK_STREAM, 0), "socket") ) return -1;
		}else {
			// UDP
			if ( IsError(sock = socket(af, SOCK_DGRAM, 0), "socket") ) return -1;
		}

		// socket �̐ݒ�
		if (boottype) {
			// client
			if (ip.length() == 0) ipget();

			addr.sin_family = af;
			addr.sin_port = htons(nPort);
			addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
			if (addr.sin_addr.S_un.S_addr == 0xFFFFFFFF) {
				hostent *host = gethostbyname(ip.c_str());
				if (host == NULL){
					IsError(INVALID_SOCKET, "hostnull");
					return -1;
				}
			}
		}else {
			// host
			addr.sin_family = af;
			addr.sin_port = htons(nPort);
			addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			if ( IsError(bind(sock, (sockaddr*)&addr, sizeof(addr)), "bind") ) return -1;

			if (sockettype == TCP) {
				// TCP�N���C�A���g����̐ڑ��v����҂Ă��Ԃɂ���
				n = listen(sock, SOMAXCONN);
				if ( IsError(n, "listen") ) return -1;
			}
		}

		return 0;
	}

	~Socket(){
		closesocket(sock);
		WSACleanup();
	}

	// definition for user own
	void Quit();
	int Error(int nError);


	//-- WSA socket functions --//

	int wsaBegin(){
		if (this->boottype == CLIENT) len = sizeof(addr);
		else len = sizeof(dstAddr);

		if (this->sockettype == TCP){
			if (this->boottype == CLIENT) {
				// client
				RETRY:
				n = WSAConnect(sock, (sockaddr*)&addr, len, NULL, NULL, NULL, NULL);
				switch(IsError(n, "WSAConnect")){
				case 0: break;
				case 2: goto RETRY;
				default: return -1;
				}
			}else {
				// host
				clsock = WSAAccept(sock, (sockaddr*)&dstAddr, &len, NULL, 0);
				if (INVALID_SOCKET == IsError(clsock, "WSAAccept")) return -1;
			}
		}

		FD_ZERO(&fds);
		if (this->boottype == CLIENT || this->sockettype == UDP)
			FD_SET(sock, &fds);
		else
			FD_SET(clsock, &fds);

		return 0;
	}

	DWORD wsaSend(char *pt, size_t size, int flags){
		DWORD datalen = 0;
		wsabuf.buf = pt;
		wsabuf.len = size;

		if (this->boottype == CLIENT)
			n = WSASend(sock, &wsabuf, 1, &datalen, flags, NULL, NULL);
		else
			n = WSASend(clsock, &wsabuf, 1, &datalen, flags, NULL, NULL);

		if (IsError(n, "WSASend")) return -1;
		return datalen;
	}

	DWORD wsaRecv(char *pt, size_t size, int flags){
		DWORD datalen = 0, dFlags = flags;
		wsabuf.buf = pt;
		wsabuf.len = size;

		if (this->boottype == CLIENT)
			n = WSARecv(sock, &wsabuf, 1, &datalen, &dFlags, NULL, NULL);
		else
			n = WSARecv(clsock, &wsabuf, 1, &datalen, &dFlags, NULL, NULL);

		if (n == 0) {
			IsError(INVALID_SOCKET, "WSARecv");
			return -1;
		}
		if (IsError(n, "WSARecv")) return -1;
		return datalen;
	}


	//-- normal socket functions --//

	int Begin(){
		if (this->sockettype == TCP) {
			len = sizeof(dstAddr);
			if (this->boottype == CLIENT) {
				// client
				RETRY:
				switch(IsError(connect(sock, (sockaddr*)&addr, len), "connect")){
				case 0:		break;
				case 2:		goto RETRY;
				default:	return -1;
				}
			}else {
				// host
				clsock = accept(sock, (sockaddr*)&dstAddr, &len);
				if (IsError(clsock, "accept")) return -1;
			}
		}else {
			if (this->boottype == CLIENT)	len = sizeof(addr);
			else					len = sizeof(dstAddr);
		}

		FD_ZERO(&fds);
		if (this->boottype == CLIENT || this->sockettype == UDP)
			FD_SET(sock, &fds);
		else
			FD_SET(clsock, &fds);

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
		if (this->boottype || this->sockettype == UDP)
			n = select(sock+1, &rfds, &wfds, &efds, ptv);
		else
			n = select(clsock+1, &rfds, &wfds, &efds, ptv);

		if (0 < n) {
			if (frw) {
				if (this->boottype == CLIENT || this->sockettype == UDP)
					n = FD_ISSET(sock, &wfds);
				else
					n = FD_ISSET(clsock, &wfds);
			} else {
				if (this->boottype == CLIENT || this->sockettype == UDP)
					n = FD_ISSET(sock, &rfds);
				else
					n = FD_ISSET(clsock, &rfds);
			}
			/* socket��O
			if (this->boottype == CLIENT || this->sockettype == UDP){
				if (FD_ISSET(sock, &efds)) ;
			} else
				if (FD_ISSET(clsock, &efds)) ;
			*/
		}else {
			IsError(INVALID_SOCKET, "select");
		}
		return n;
	}

	// for udp.
	int Socket::sendto(char *pt, size_t size, int flags){
		if (size == 0) return -1;
		if (boottype == CLIENT)
			n = ::sendto(sock, pt, size, flags, (sockaddr*)&addr, len);
		else
			n = ::sendto(sock, pt, size, flags, (sockaddr*)&dstAddr, len);
		return n;
	}

	// for udp. result when sockclose is 0
	int Socket::recvfrom(char *pt, size_t size, int flags){
		if (size == 0) return -1;
		if (boottype == CLIENT)
			n = ::recvfrom(sock, pt, size, flags, (sockaddr*)&addr, &len);
		else
			n = ::recvfrom(sock, pt, size, flags, (sockaddr*)&dstAddr, &len);
		return n;
	}

	int Socket::send(char *pt, size_t size, int flags){
		if (size == 0) return -1;
		if (boottype == CLIENT)	n = ::send(sock, pt, size, flags);
		else					n = ::send(clsock, pt, size, flags);

		if (n < size) { IsError(SOCKET_ERROR, "send"); return -1; }
		return n;
	}

	// result when sockclose is 0
	int Socket::recv(char *pt, size_t size, int flags){
		// 0 | MSG_OOB | MSG_PEEK | MSG_WAITALL
		if (this->boottype == CLIENT || this->sockettype == UDP)	
			n = ::recv(sock, pt, size, flags);
		else
			n = ::recv(clsock, pt, size, flags);

		if (n == SOCKET_ERROR) { IsError(SOCKET_ERROR, "receive"); return -1; }
		if (n == WSAEOPNOTSUPP) { IsError(SOCKET_ERROR, "receive"); return WSAEOPNOTSUPP; }

		return n;
	}

	void Socket::shutdown(){
		::shutdown(sock, SD_BOTH);
	}
};

//---

// �N���X�쐬
BOOL ClassInit(WNDCLASSEX &wce, char* AppName, WNDPROC WndProc){

	wce.cbSize		= sizeof(wce);	// �\���̂�size
	wce.style		= CS_HREDRAW | CS_VREDRAW;	// �X�^�C��
	wce.lpfnWndProc	= WndProc;	// �v���[�W���֐�
	wce.cbClsExtra	= wce.cbWndExtra = 0;
	wce.hInstance	= NULL;	// �v���O�����̃n���h��
	wce.hIcon		= LoadIcon(NULL, IDI_APPLICATION);	// �A�C�R��
	wce.hCursor		= LoadCursor(NULL, IDC_ARROW);	// �}�E�X�J�[�\��
	wce.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// �u���V
	wce.lpszMenuName	= NULL;	// ���j���[
	wce.lpszClassName	= AppName;	// �N���X��
	wce.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wce))return FALSE;

	return TRUE;
}

// �E�B���h�E�쐬�E�\��
BOOL WindowInit(HINSTANCE hInstance, HWND &hWnd, int nCmd, char* AppName){
	
	hWnd = CreateWindowEx(
		0, AppName, AppName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
		NULL, NULL, hInstance, NULL
	);
	if (!hWnd) return FALSE;
	ShowWindow(hWnd, nCmd);

	return TRUE;
}

class SELWNDINFO{
public:
	HWND hWnd;
	HDC hDC, hMemDC;
	BITMAPINFO bmi;
	HBITMAP hBmp;
	RECT rc;
	int width, height;

	void CreatehDeskDC(HWND hWnd){
		hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	}

	void GethDC(HWND hWnd){
		hDC = ::GetDC(hWnd);
	}

	void ReleasehDC(HWND hWnd){
		ReleaseDC(hWnd, hDC);
	}

	void CreatehDIBSection(LPBITMAPINFO lpbmi, UINT usage, void** ppvBits){
		hBmp = ::CreateDIBSection(hDC, lpbmi, usage, ppvBits, NULL, 0);
	}

	void CreateCompatiblehhMemDC(){
		hMemDC = CreateCompatibleDC(hDC);
	}

	void SelectObjecthMemDChBmp(){
		SelectObject(hMemDC, hBmp);
	}

	void CopyDC(DWORD rop){
		BitBlt(hMemDC, 0, 0, width, height, hDC, 0, 0, rop);
	}

	void SetWndSize(int width, int height){
		this->width = width;
		this->height = height;
	}

	void SetMainWnd(HWND hWnd){
		this->hWnd = hWnd;
	}

	template <class T> void GetWndSize(T width, T height){
		width = (T)this->width;
		height = (T)this->height;
	}
};

namespace {
	SELWNDINFO wi;
	BITMAPINFO bmi;
}

void SetClientSize(HWND hWnd, int width, int height){
	RECT rc = { 0, 0, width, height };

	::AdjustWindowRectEx(&rc, GetWindowLong(hWnd, GWL_STYLE), false, 0);
	::SetWindowPos(hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top, 
		SWP_NOMOVE | SWP_NOZORDER);

}

//~~~


namespace {
	HWND hWnd;
	HDC hMemDC;
	LPDWORD lpMemory = NULL;
	bool quit = false, redraw = false;
	unsigned int width, height, scsize;
	unsigned int mywdh, myhgt;
	char AppName[] = "Share Window";
	unsigned char bit = 3;

	const bool boottype = HOST;
	unsigned short PORT = 55000;
}

void SetTitle(const char *text){
	SetWindowText(hWnd, text);
}

// Socket����ɃG���[�I������Ƃ��ɌĂ΂��
void Socket::Quit(){
	quit = true;
}
// Socket���G���[���N������ return: 0:�I�� 1:�G���[���� 2:�Ď��s(�ꕔ)
int Socket::Error(int nError){
	switch(nError){
	case WSAECONNREFUSED:
		if ( MessageBox(hWnd,
			"�T�[�o�[�̏����������Ă��܂���B\n�Đڑ����܂����H",
			AppName, MB_RETRYCANCEL) == IDRETRY ){
				return 2;
		}
		break;
	case WSAETIMEDOUT:
		if ( MessageBox(hWnd,
			"�ڑ����^�C���A�E�g���܂����B\n�Đڑ����܂����H",
			AppName, MB_RETRYCANCEL) == IDRETRY ){
				return 2;
		}
		break;
	case WSAEINTR:
		return 1;
	}

	return 0;
}

unsigned int WINAPI SocketMain(void *pParam){
	Socket *sock = (Socket*)pParam;

	// DIB�쐬 : x[�ʏ�] y[����0]
	HWND hTWnd;
	if (::boottype == CLIENT)	hTWnd = GetDesktopWindow();
	else						hTWnd = hWnd;

	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = bit*8;
	bmi.bmiHeader.biCompression = BI_RGB;

	HDC hDC = GetDC(hTWnd);
	HBITMAP hBmp = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS,
		(void**)&lpMemory, NULL, 0);
	if (lpMemory == NULL) {
		MessageBox(NULL, "DIB���쐬����܂���ł����B", AppName, MB_OK);
		quit = true;
		return 1;
	}
	hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBmp);
	StretchBlt(hMemDC, 0, 0, width, height, hDC, 0, 0, mywdh, myhgt, SRCCOPY);

	char str[64] = "";
	short cnt = 0;
	if (boottype == CLIENT) {
		while(1) {
			if (quit) break;
			StretchBlt(hMemDC, 0, 0, width, height, hDC, 0, 0, mywdh, myhgt, SRCCOPY);
			sock->wsaSend((char*)lpMemory, scsize, 0);

//			Sleep(1);
		}
	}else {
		while(1) {
			if (quit) break;
			sock->wsaRecv((char*)lpMemory, scsize, 0);
			BitBlt(hDC, 0, 0, width, height, hMemDC, 0, 0, SRCCOPY);

//			Sleep(1);
		}
	}
	if (!quit){
		wsprintf(str, "�ڑ����ؒf����܂����B\nboottype : %d", ::boottype);
		MessageBox(NULL, str, AppName, MB_OK);
		quit = true;
	}

	if (::boottype == CLIENT) {
		DeleteObject(hBmp);
		DeleteDC(hMemDC);
		ReleaseDC(hTWnd, hDC);
	}

	return 0;
}

LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp){

	switch(uMsg){
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

	// �\�P�b�g �쐬
	Socket sock;
	sock.SetIPAddress(lpCmd);
	if (sock.Init(MAKEWORD(2, 0), AF_INET, PORT, TCP, boottype) == -1) goto QUIT;

	char str[64];
	if (lstrlen(lpCmd)) {
		wsprintf(str, "Set IPAddress : %s", sock.getip());
		MessageBox(hWnd, str, AppName, MB_OK);
	}else {
		wsprintf(str, "My IPAddress : %s", sock.ipget());
		MessageBox(hWnd, str, AppName, MB_OK);
		sock.SetIPAddress("");
	}

	SetTitle("begin prev");
	sock.Begin();


	// �T�C�Y���킹
	SetTitle("size prev");
	{
		RECT rc;
		GetClientRect(GetDesktopWindow(), &rc);
		mywdh = rc.right;
		myhgt = rc.bottom;
	}
	if (::boottype == CLIENT) {
		// client
		sock.wsaRecv(str, 8, 0);
		memcpy_s(&width, 4, str, 4);
		memcpy_s(&height, 4, str+4, 4);
		if (mywdh < width) width = mywdh;
		if (myhgt < height) height = myhgt;

		// �m��𑗐M
		memcpy_s(str, 64, &width, 4);
		memcpy_s(str+4, 64, &height, 4);
		sock.wsaSend(str, 8, 0);
	}else {
		// host
		memcpy_s(str, 64, &mywdh, 4);
		memcpy_s(str+4, 64, &myhgt, 4);
		sock.wsaSend(str, 8, 0);

		// �m�����M
		sock.wsaRecv(str, 8, 0);
		memcpy_s(&width, 4, str, 4);
		memcpy_s(&height, 4, str+4, 4);
	}
	scsize = width*height*bit;
	if (boottype == HOST)
		SetClientSize(hWnd, width, height);
	else
		SetClientSize(hWnd, 256, 60);

	// �T�u�X���b�h
	SetTitle("thread prev");
	HANDLE hThread, hMutex;
	hMutex = CreateMutex(NULL, false, NULL);
	hThread = (HANDLE)_beginthreadex(NULL, 0, SocketMain, &sock, 0, NULL);

	SetTitle(AppName);
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

	// �T�u�X���b�g�I���ҋ@
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hMutex);

	QUIT:
	sock.shutdown();

	return 0;
}
