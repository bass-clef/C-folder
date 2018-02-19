
#include <Windows.h>
#include <vector>
#include <stack>

typedef std::vector< std::vector<std::string> > VVSTRINGS;
typedef std::vector< std::vector<int> > VVINTS;
typedef std::vector< std::vector<bool> > VVBOOLS;

template <class T> class VVARRAY{
protected:
	// ary[int][int] = stinrg の形式
	T *vvs;

	void resize(size_t first, size_t second, size_t size){
		if (vvs->at(first).at(second).size() <= size)
			vvs->at(first).at(second).resize(size);
	}
	
public:
	VVARRAY(){
		vvs = NULL;
	}

	VVARRAY(T *v){
		vvs = v;
	}

	void operator=(T *v){
		vvs = v;
	}

	void Init(size_t first, size_t second){
		if (vvs->size() <= first) vvs->resize(first+1);
		for(size_t i=0; i<=first; i++)
			if (vvs->at(i).size() <= second) vvs->at(i).resize(second+1);
	}

	size_t Set(size_t first, size_t second, char *str){
		Init(first, second);
		resize(first, second, strlen(str));
		vvs->at(first).at(second).assign(str);
		return vvs->at(first).at(second).size();
	}

	const char* Get(size_t first, size_t second){
		if (vvs->size() <= first) return "";
		if (vvs->at(first).size() <= second) return "";
		return vvs->at(first).at(second).data();
	}

	size_t Copy(size_t first, size_t second, char *str, size_t index, size_t len, size_t index2=0){
		Init(first, second);

		resize(first, second, len+index2);
		memcpy((void*)(vvs->at(first).at(second).data()+index2), str+index, len);
		return vvs->at(first).at(second).size();
	}

	size_t Dest(size_t first, size_t second, char *dest, size_t index, size_t len){
		if (vvs->size() <= first) return 0;
		if (vvs->at(first).size() <=second) return 0;
		memcpy(dest, vvs->at(first).at(second).data()+index, len);
		return vvs->at(first).at(second).size();
	}

	size_t FirstSize(){
		return vvs->size();
	}

	size_t SecondSize(size_t first){
		if (FirstSize()) return vvs->at(first).size();
		return 0;
	}

	size_t DataSize(size_t first, size_t second){
		if (SecondSize(first)) return vvs->at(first).at(second).size();
		return 0;
	}

};

template <class T> class SUBVVARRAY : public VVARRAY<T> {
	std::vector<T*> vpt;
//	std::stack<T*> old;
	unsigned char nold;
	T* old[256];

public:
	SUBVVARRAY(){
		nold = 0;
	}

	~SUBVVARRAY(){
		while(vpt.size()){
			delete vpt.back();
			vpt.pop_back();
		}
	}

	T* Create(){
		vpt.push_back(new T);
		return vpt.back();
	}

	void Setting(T *v){
//		if (vvs != v) old.push(vvs);

		old[nold] = vvs;
		nold = (nold+1)%256;
		vvs = v;
	}

	void BackSet(){
		nold = (nold+255)%256;
		vvs = old[nold];

//		vvs = old.top();
//		old.pop();
	}
};

class VVINT : public SUBVVARRAY<VVINTS> {
public:
	size_t Set(size_t first, size_t second, int value){
		Init(first, second);
		vvs->at(first).at(second) = value;
		return 4;
	}

	const int Get(size_t first, size_t second){
		if (vvs->size() <= first) return 0;
		if (vvs->at(first).size() <= second) return 0;
		return vvs->at(first).at(second);
	}

	size_t Copy(size_t first, size_t second, int *pt, size_t index, size_t len){
		Init(first, second);
		vvs->at(first).at(second) = *pt;
		return vvs->at(first).at(second);
	}

	size_t Dest(size_t first, size_t second, int *dest, size_t index, size_t len){
		if (vvs->size() <= first) return 0;
		if (vvs->at(first).size() <= second) return 0;
		*dest = vvs->at(first).at(second);
		return vvs->at(first).at(second);
	}

	size_t DataSize(size_t first, size_t second){
		return 0;
	}
};

class VVBOOL : public SUBVVARRAY<VVBOOLS> {
public:
	size_t Set(size_t first, size_t second, bool bl){
		Init(first, second);
		vvs->at(first).at(second) = bl;
		return vvs->at(first).at(second);
	}

	const bool Get(size_t first, size_t second){
		if (vvs->size() <= first) return false;
		if (vvs->at(first).size() <= second) return false;
		return vvs->at(first).at(second);
	}

	size_t Copy(size_t first, size_t second, bool bl, size_t index, size_t len){
		return false;
	}

	size_t Dest(size_t first, size_t second, bool bl, size_t index, size_t len){
		return false;
	}

	size_t DataSize(size_t first, size_t second){
		return 1;
	}
};

namespace {
	VVSTRINGS v;
	VVARRAY<VVSTRINGS> vvs(&v);
	SUBVVARRAY<VVSTRINGS> vpt;
	VVINT vipt;
	VVBOOL vbpt;
}

void __stdcall VVSInit(size_t first, size_t second){
	vvs.Init(first, second);
}

size_t __stdcall VVSSet(size_t first, size_t second, char *str){
	return vvs.Set(first, second, str);
}

const char* __stdcall VVSGet(size_t first, size_t second){
	return vvs.Get(first, second);
}

size_t __stdcall VVSCopy(size_t first, size_t second, char *str, size_t index, size_t len, size_t index2){
	return vvs.Copy(first, second, str, index, len, index2);
}

size_t __stdcall VVSDest(size_t first, size_t second, char *dest, size_t index, size_t len){
	return vvs.Dest(first, second, dest, index, len);
}

size_t __stdcall VVSFirstSize(){
	return vvs.FirstSize();
}

size_t __stdcall VVSSecondSize(size_t first){
	return vvs.SecondSize(first);
}

size_t __stdcall VVSDataSize(size_t first, size_t second){
	return vvs.DataSize(first, second);
}


void __stdcall SVVBackSet(SUBVVARRAY<void*> *sv){
	sv->BackSet();
}

void* __stdcall SVVSCreate(){
	return (void*)vpt.Create();
}

void __stdcall SVVSOperate(VVSTRINGS *v){
	vpt.Setting(v);
}

void __stdcall SVVSInit(size_t first, size_t second){
	vpt.Init(first, second);
}

size_t __stdcall SVVSSet(size_t first, size_t second, char *str){
	return vpt.Set(first, second, str);
}

const char* __stdcall SVVSGet(size_t first, size_t second){
	return vpt.Get(first, second);
}

size_t __stdcall SVVSCopy(size_t first, size_t second, char *str, size_t index, size_t len, size_t index2=0){
	return vpt.Copy(first, second, str, index, len, index2);
}

size_t __stdcall SVVSDest(size_t first, size_t second, char *dest, size_t index, size_t len){
	return vpt.Dest(first, second, dest, index, len);
}

size_t __stdcall SVVSFirstSize(){
	return vpt.FirstSize();
}

size_t __stdcall SVVSSecondSize(size_t first){
	return vpt.SecondSize(first);
}

size_t __stdcall SVVSDataSize(size_t first, size_t second){
	return vpt.DataSize(first, second);
}


void* __stdcall SVVBCreate(){
	return (void*)vbpt.Create();
}

void __stdcall SVVBOperate(VVBOOLS *v){
	vbpt.Setting(v);
}

void __stdcall SVVBInit(size_t first, size_t second){
	vbpt.Init(first, second);
}

size_t __stdcall SVVBSet(size_t first, size_t second, bool bl){
	return vbpt.Set(first, second, bl);
}

const bool __stdcall SVVBGet(size_t first, size_t second){
	return vbpt.Get(first, second);
}

size_t __stdcall SVVBFirstSize(){
	return vbpt.FirstSize();
}

size_t __stdcall SVVBSecondSize(size_t first){
	return vbpt.SecondSize(first);
}


void* __stdcall SVVICreate(){
	return (void*)vipt.Create();
}

void __stdcall SVVIOperate(VVINTS *v){
	vipt.Setting(v);
}

void __stdcall SVVIInit(size_t first, size_t second){
	vipt.Init(first, second);
}

size_t __stdcall SVVISet(size_t first, size_t second, int value){
	return vipt.Set(first, second, value);
}

const int __stdcall SVVIGet(size_t first, size_t second){
	return vipt.Get(first, second);
}

size_t __stdcall SVVIFirstSize(){
	return vipt.FirstSize();
}

size_t __stdcall SVVISecondSize(size_t first){
	return vipt.SecondSize(first);
}


void __stdcall SVVIBackSet(){
	vipt.BackSet();
}

void __stdcall SVVSBackSet(){
	vpt.BackSet();
}


#ifndef _DEBUG

BOOL WINAPI DllMain(HINSTANCE hDllInst, DWORD fdwReason, LPVOID lpvReserved){
	return TRUE;
}

#else

#include <iostream>
using namespace std;
int main(char *args){
	char str[64];

	/* 競合テスト２ *
	void *pt = SVVICreate();
	SVVIOperate((VVINTS*)pt);
	SVVIInit(0+1, 3);
	SVVIBackSet();

	SVVIOperate((VVINTS*)pt);
	SVVISet(0, 0, 0);
	SVVISet(0, 1, -1);
	SVVISet(0, 2, -1);
	SVVIBackSet();

	/**/

	/* 競合テスト */
	void *pt = SVVSCreate();
	void *bpt = SVVBCreate();
	SVVSOperate((VVSTRINGS*)pt);
//	VVSInit(3, 1);

	// 代入テスト(Sub)
	wsprintf(str, "Hoge");
	cout << "Sub Copy[" << ::SVVSCopy(3, 0, str, 0, lstrlen(str)) << "]" << endl;

	// 代入テスト
	wsprintf(str, "Hoge");
	cout << "Copy[" << ::VVSCopy(3, 0, str, 0, lstrlen(str), 0) << "]" << endl;

	// 代入テスト２
	wsprintf(str, "Foo");
	cout << "Copy[" << ::VVSCopy(3, 1, str, 0, lstrlen(str), 0) << "]" << endl;

	// 代入テスト２(Sub)
	wsprintf(str, "Foo");
	cout << "Sub Copy[" << ::SVVSCopy(3, 1, str, 0, lstrlen(str)) << "]" << endl;


	// サイズ確認
	cout << "Size:" << SVVSFirstSize() << endl;
	for(size_t i=0; i<SVVSFirstSize(); i++) {
		cout << "Size[" << i << "]:" << SVVSSecondSize(i) << endl;
		for(size_t j=0; j<SVVSSecondSize(i); j++){
			cout << "[" << i << "][" << j << "]" << SVVSDataSize(i, j);
			if (SVVSDataSize(i, j)) cout << "[" << SVVSGet(i, j) << "]";
			cout << endl;
		}
	}
	cout << endl;

	// サイズ確認(Sub)
	cout << "Sub Size:" << SVVSFirstSize() << endl;
	for(size_t i=0; i<SVVSFirstSize(); i++) {
		cout << "Size[" << i << "]:" << SVVSSecondSize(i) << endl;
		for(size_t j=0; j<SVVSSecondSize(i); j++){
			cout << "[" << i << "][" << j << "]" << SVVSDataSize(i, j);
			if (SVVSDataSize(i, j)) cout << "[" << SVVSGet(i, j) << "]";
			cout << endl;
		}
	}
	cout << endl;

	// 取得テスト(Sub)
	cout << "Sub Get[" << ::SVVSGet(3, 0) << "]" << endl;

	// 取得テスト
	cout << "Get[" << ::SVVSGet(3, 0) << "]" << endl;
	/**/

	cin >> str;
}

#endif


