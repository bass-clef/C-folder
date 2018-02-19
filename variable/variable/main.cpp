

#include <iostream>
#include <windows.h>

using namespace std;

#define endl2 endl << endl

typedef void*(*func)(...);

class var{
	void *v, *vnew;
	unsigned int s;
	unsigned int t;

	// メモリ開放
	void del(){
		if ((v == NULL) && (s == 0)) return;
		delete[] v;
		s = 0;
	}

	// メモリ確保
	void alc(unsigned int size){
		if ((v == NULL) && (s == 0)) {
			v = new char[size];
			s = size;
		}else {
			if (size < s) return;
			vnew = new char[size];
			memcpy(vnew, v, s);
			delete[] v;
			v = vnew;
			s = size;
		}
	}

	// 型変換
	void typ(int hash){
		// hash:変換先の型 t:変換前の型
		t = hash;
	}

public:
	// コン・デ・ストラクタ
	var()	{ v = NULL; s = 0; alc(8); }
	~var()	{ del(); }

	// 演算子オーバーロード
	template <class T> T operator=(T t)		{
		typ(typeid(T).hash_code()); return (*((T*)v) = t); }
	template <class T> T operator+(T t)		{
		typ(typeid(T).hash_code()); return (*((T*)v) + t); }
	template <class T> T operator+=(T t)	{
		typ(typeid(T).hash_code()); return (*((T*)v) += t); }
	template <class T> T operator-(T t)		{
		typ(typeid(T).hash_code()); return (*((T*)v) - t); }
	template <class T> T operator-=(T t)	{
		typ(typeid(T).hash_code()); return (*((T*)v) -= t); }
	template <class T> operator T() {
		typ(typeid(T).hash_code()); return *((T*)v); }
	// char
	operator char*(){ return (char*)v; }
	char* operator=(const char* c){
		unsigned int csize = strlen(c);
		alc(csize+1);
		lstrcpyn((char*)v, c, csize+1);
		return (char*)v;
	}
	char* operator+=(const char* c){
		alc(strlen((char*)v)+strlen(c)+1);
		strcat_s((char*)v, s, c);
		return (char*)v;
	}
	char* operator=(char* c){
		unsigned int csize = strlen(c);
		alc(csize+1);
		lstrcpyn((char*)v, c, csize+1);
		return (char*)v;
	}
	char* operator+=(char* c){
		alc(strlen((char*)v)+strlen(c)+1);
		strcat_s((char*)v, s, c);
		return (char*)v;
	}
	// func
	operator func(){ return (func)(*((long*)v)); }

	// 参照用
	unsigned int size(){ return (s); }
	int type(){ return t; }
};

int myfunc(int a,...){
	return a;
}

int main(){
	var v;
	char str[128] = "mjk";

	// show 10 -> 16
	cout << std::hex << std::showbase;

	v = str;
	cout << "char[" << (char*)v << "]" << endl;
	cout << "size[" << v.size() << "]" << endl;
	cout << "type[" << v.type() << "]" << endl2;

	v = 10 + 5;
	cout << "int[" << (int)v << "]" << endl;
	cout << "size[" << v.size() << "]" << endl;
	cout << "type[" << v.type() << "]" << endl2;

	v = &myfunc;
	cout << "func[" << v(2, 3) << "]" << endl;
	cout << "func add[" << func(v) << "]" << endl;
	cout << "size[" << v.size() << "]" << endl;
	cout << "type[" << v.type() << "]" << endl2;

	v = "I'm ";
	cout << "char[" << (char*)v << "]" << endl;
	cout << "size[" << v.size() << "]" << endl;
	cout << "type[" << v.type() << "]" << endl2;

	v += "String";
	v += ". What type are you?";
	cout << "char[" << (char*)v << "]" << endl;
	cout << "size[" << v.size() << "]" << endl;
	cout << "type[" << v.type() << "]" << endl2;

	v = 111.1;
	cout << "double[" << (double)v << "]" << endl;
	cout << "size[" << v.size() << "]" << endl;
	cout << "type[" << v.type() << "]" << endl2;


	cin >> str;

	return 0;
}
