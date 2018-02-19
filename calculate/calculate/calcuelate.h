
#ifndef CALCULATE_H

#define CALCULATE_H

#include <windows.h>

#define	In(prm, min, max) ( ((min) <= (prm)) && ((prm) <= (max)) )
#define	LDOUBLE_MAX	LONG_MAX
#define Ttype	long long
#define	Ttyped	double
#define Tmax	LLONG_MAX
#define	Tmaxd	LDOUBLE_MAX

char** createchar(int byte = 1);
void popchar(void);
void deletechar(void);
char getpeek(char* str, int index);
char* ToRPN(LPSTR FromBuf, LPSTR ToBuf, char rn);
Ttyped Calculate(LPSTR buf, char rn);
Ttyped Calculated(LPSTR buf, char rn);
int GetPriority(LPSTR Priority, char buf);
void Encryption(char* buf, char* lpFromTable, char* lpToTable, char tablemax, int filesize);


#endif
