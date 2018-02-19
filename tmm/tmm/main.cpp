
// trick mansion module

#include <windows.h>
#include <string>
#include <vector>

class vecmap{
	std::vector<std::vector<std::string>*> t;

public:
	void firstinit(size_t first) {
		while(t.size() <= first){
			t.push_back(new std::vector<std::string>);
		}
	}

	void secondinit(size_t first, size_t second){
		try{
			while(t.at(first)->size() <= second){
				t.at(first)->push_back("");
			}
		}catch (std::out_of_range) {
			firstinit(first);
			t.at(first)->push_back("");
		}
	}

	void init(size_t first, size_t second){
		firstinit(first);
		secondinit(first, second);
	}


	void cpy(size_t first, size_t second, const char *str, size_t index, size_t size){
		try{
			t.at(first)->at(second).resize(size, 0);
			memcpy((void*)t.at(first)->at(second).data(), str+index, size);
		} catch (std::out_of_range) {
			secondinit(first, second);
			t.at(first)->at(second).resize(size, 0);
			memcpy((void*)t.at(first)->at(second).data(), str+index, size);
		}
	}

	void set(size_t first, size_t second, char *str){
		try{
			t.at(first)->at(second) = str;
		} catch (std::out_of_range) {
			secondinit(first, second);
			t.at(first)->at(second) = str;
		}
	}

	const char *get(size_t first, size_t second){
		try{
			return t.at(first)->at(second).data();
		} catch (std::out_of_range) {
			secondinit(first, second);
			return NULL;
		}
	}

	size_t f_size(){
		return t.size();
	}

	size_t s_size(size_t first){
		try{
			return t.at(first)->size();
		} catch (std::out_of_range) {
			secondinit(first, 0);
			return 1;
		}
	}

	size_t d_size(size_t first, size_t second){
		try {
			return t.at(first)->at(second).size();
		} catch (std::out_of_range) {
			secondinit(first, second);
			return 0;
		}
	}

	~vecmap(){
		while(t.size()){
			delete t.back();
			t.pop_back();
		}
	}
};

union strsrt {
	char str[2];
	short srt;
};



static vecmap vm;

void vminit(size_t first, size_t second){
	if ((first) || (second)) vm.init(first-1, second-1);
}

void vmset(size_t first, size_t second, char *str){
	vm.set(first, second, str);
}

void vmcpy(size_t first, size_t second, const char *str, size_t index, size_t size){
	vm.cpy(first, second, str, index, size);
}

const char *vmget(size_t first, size_t second){
	return vm.get(first, second);
}

size_t vmf_size(){
	return vm.f_size();
}

size_t vms_size(size_t first){
	return vm.s_size(first);
}

size_t vmd_size(size_t first, size_t second){
	return vm.d_size(first, second);
}

char strpeek(const char *buf, size_t index){
	return buf[index];
}

short strwpeek(const char *buf, size_t index){
	strsrt data = {buf[index], buf[index+1]};
	return data.srt;
}

void strcpy(char *dst, char *src, size_t size, size_t index1 = 0, size_t index2 = 0){
	memcpy(dst+index1, src+index2, size);
}


#define _DEBUG
#ifndef _DEBUG
BOOL WINAPI DllMain(HINSTANCE hDllInst, DWORD fdwReason, LPVOID lpvReserved){
	return TRUE;
}
#else
#include <iostream>

int main(){
	char str[128];
	std::cin >> str;

	const auto d = 1.0;
	const auto dex = d*5;

	return 0;
}
#endif

