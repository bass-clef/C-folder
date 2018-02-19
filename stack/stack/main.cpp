
#include <vector>

class DESTRUCTOR {
	std::vector<void*> pointer;
	std::vector<void*>::iterator it;

public:
	~DESTRUCTOR(){
		while(pointer.size()){
			delete[] pointer.back();
			pointer.pop_back();
		}
	}

	void PtPush(void *pt){
		bool b = true;
		for (it = pointer.begin(); it != pointer.end(); it++) {
			if (*it == pt) {
				b = false;
				break;
			}
		}

		if (b) pointer.push_back(pt);
	}
};

namespace {
	DESTRUCTOR pp;
	std::vector<void*>::iterator it;
}

void* __stdcall Init(){
	void *pt = new std::vector<void*>;
	pp.PtPush(pt);
	return pt;
}

void __stdcall Push_Back(std::vector<void*> *pt, void *value){
	pt->push_back(value);
}

void __stdcall Pop_Back(std::vector<void*> *pt){
	pt->pop_back();
}


void* __stdcall Back(std::vector<void*> *pt){
	return pt->back();
}

void* __stdcall Front(std::vector<void*> *pt){
	return pt->front();
}

void __stdcall Clear(std::vector<void*> *pt){
	pt->clear();
}

size_t __stdcall Size(std::vector<void*> *pt){
	return pt->size();
}

void* __stdcall At(std::vector<void*> *pt, size_t i){
	return pt->at(i);
}


void* __stdcall Iterator(std::vector<void*> *pt){
	return &(pt->begin());
}

void __stdcall AddIt(std::vector<void*>::iterator *pt){
	*(pt)++;
}

void* __stdcall RefIt(std::vector<void*>::iterator *pt){
	return *(*pt);
}

