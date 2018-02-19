
#ifndef BB_H
#define BB_H

#include <windows.h>
#include <deque>

#ifndef DEBUG_
#include <iostream>
using namespace std;
#endif

#define bbassert(text) MessageBox(NULL, text, "Black Board - Error", MB_OK | MB_ICONERROR)
typedef void (*PFUNC)(...);

enum BERROR {
	BE_NOERROR=0,
	BE_MALLOC,
	BE_NULLPOINTER,
	BE_VOIDREALLOC
};
enum BVTYPE {
	BVT_VOID=0,
	BVT_INT,
	BVT_STR,
	BVT_DEC,
	BVT_FUNC
};

class BBVAR{
protected:
	void* contents;
	void* subconts;

public:
	BVTYPE type;
	char* name;

	BBVAR();
	~BBVAR();

	template <class T> bool realloc(T* any, size_t anySize, size_t anyInSize);

	BERROR conrealloc(size_t anyInSize, BVTYPE type);
	void* getcon() const { return contents; }
	template <class T> BERROR setcon(T* any, size_t anySize);

	bool namerealloc(size_t nameInSize);
	BERROR setname(char* name, size_t nameSize);
};
class BlackBoard{
private:
	std::deque<BBVAR*> var;
	BERROR error;

public:
	~BlackBoard();
	bool Error(BERROR error);
	BERROR addvar(char* name, size_t nameLen, BVTYPE type);
	BERROR setcon(unsigned int num, void* any, size_t anySize);

	BERROR compile(char* Buffer);
	void varview();
};


#endif
