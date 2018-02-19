#include <Windows.h>



const char** GetStrPointer(const char *str){
	return &str;
}

const char* StrReference(const char **str){
	return *(str);
}

char CharReference(char *pt){
	return *(pt);
}

void CharDereference(char *pt, char value){
	*(pt) = value;
}

int IntReference(int *pt){
	return *(pt);
}

void IntDereference(int *pt, int value){
	*(pt) = value;
}


BOOL WINAPI DllMain(HINSTANCE hDllInst, DWORD fdwReason, LPVOID lpvReserved){
	return TRUE;
}
