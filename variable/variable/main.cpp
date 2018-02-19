

#include <iostream>
#include <windows.h>

using namespace std;

#define endl2 endl << endl

typedef void*(*func)(...);

class var{
	void *v, *vnew;
	unsigned int s;
	unsigned int t;

	// �������J��
	void del(){
		if ((v == NULL) && (s == 0)) return;
		delete[] v;
		s = 0;
	}

	// �������m��
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

	// �^�ϊ�
	void typ(int hash){
		// hash:�ϊ���̌^ t:�ϊ��O�̌^
		t = hash;
	}

public:
	// �R���E�f�E�X�g���N�^
	var()	{ v = NULL; s = 0; alc(8); }
	~var()	{ del(); }

	// ���Z�q�I�[�o�[���[�h
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

	// �Q�Ɨp
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
