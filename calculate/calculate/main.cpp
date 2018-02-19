
#include <windows.h>
#include <stdio.h>

#include "calcuelate.h"

#ifndef DEBUG
int main(LPSTR argc[]) {
	printf("逆ポーランド記法による\n数式計算テストプログラム\n作成者 Humi\n\n");

	char Buf[] = "125.5 + 5";
	char *Buf2 = new char[lstrlen(Buf)*2];

	ToRPN(Buf, Buf2, ' ');
	printf("[%s]\n", Buf);
	printf("[%s]\n", Buf2);
	printf("\n\nAnswer = [%f]", Calculate(Buf2, ' '));
	
	delete[] Buf2;

	printf("\n\nPlease push Enter...\n");


	while(1){
		if (GetAsyncKeyState(13)) break;
		Sleep(1);
	}

	return 0;
}

#else
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD fdwReason,LPVOID lpvReserved){
	return TRUE;
}
#endif

