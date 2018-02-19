

#include "blackboard.h"

// BBVAR�R���X�g���N�^
BBVAR::BBVAR(){
	contents = NULL;
	subconts = NULL;
	name = NULL;
	type = BVT_VOID;
}

// BBVAR�f�X�g���N�^
BBVAR::~BBVAR(){
	if (name != NULL) delete[] name;
	if (contents != NULL) delete[] contents;
	if (subconts != NULL) delete[] subconts;
}

template <class T> bool BBVAR::realloc(T* any, size_t anySize, size_t anyInSize){
	if (any == NULL) {
		any = new T[anyInSize];
	}else {
		if (anySize >= anyInSize) return false;

		T *t = new T[anySize];
		if (t == NULL) return true;	// error

		memcpy(t, any, anySize);
		delete[] any;

		any = new T[anyInSize];
		if (any == NULL) { delete[] t; return true; }

		memcpy(any, t, anySize);
		delete[] t;
	}
	return false;
}

// contents �������m��
BERROR BBVAR::conrealloc(size_t anyInSize, BVTYPE type){
	switch(type){
//	case BVT_VOID:realloc((void*)contents, sizeof(contents), anyInSize); break;
	case BVT_INT: realloc((int*)contents, sizeof(contents), anyInSize); break;
	case BVT_DEC: realloc((double*)contents, sizeof(contents), anyInSize); break;
	case BVT_STR: realloc((char*)contents, sizeof(contents), anyInSize); break;
//	case BVT_FUNC:realloc((PFUNC)contents, sizeof(contents), anyInSize); break;
	default: return BE_VOIDREALLOC;
	}

	return BE_NOERROR;
}

template <class T> BERROR BBVAR::setcon(T* any, size_t anySize){
	BERROR error = conrealloc(anySize, type);
	if (error != BE_NOERROR) return error;
	memcpy(contents, any, anySize);
	
	return BE_NOERROR;
}

// name �������m��
bool BBVAR::namerealloc(size_t nameInSize){
	return realloc((char*)name, sizeof(name), nameInSize);
}

// name ���
BERROR BBVAR::setname(char* name, size_t nameSize){
	if (namerealloc(nameSize)) return BE_MALLOC;
	lstrcpyn(this->name, name, nameSize);
	return BE_NOERROR;
}


BlackBoard::~BlackBoard(){
	while(var.size()){
		if (var.front() != NULL) delete var.front();
		var.pop_front();
	}
}

// �ϐ��ǉ�
BERROR BlackBoard::addvar(char* name, size_t nameLen, BVTYPE type){
	var.push_back(new BBVAR);
	if (var.back() == NULL) return BE_MALLOC;

	var.back()->type = type;
	return var.back()->setname(name, nameLen+1);
}

// contents ���
BERROR BlackBoard::setcon(unsigned int num, void* any, size_t anySize){
	error = var[num]->setcon(any, anySize);
	return error;
}

// �ϐ��ꗗ
void BlackBoard::varview(){
	for(unsigned int i=0; i<var.size(); i++){
		cout << "�^:" << var[i]->type << endl;

		if (var[i]->name != NULL)
			cout << "�ϐ���[" << var[i]->name << "]" << endl;
		else cout << "�ϐ���[�s��] ���������[�N�̉\��" << endl;

		if (var[i]->getcon() != NULL)
			cout << "���ept[" << var[i]->getcon() << "]" << endl;
		else cout << "���e���m�ۂ���Ă܂��� �s�����������̉\��" << endl;
	}
}

bool BlackBoard::Error(BERROR error){
	this->error = error;
	switch(error){
	case BE_NOERROR:	return true;
	case BE_MALLOC:		bbassert("�������̈���m�ۂł��܂���ł���"); break;
	case BE_NULLPOINTER:bbassert("NULL�|�C���^���Q�Ƃ��܂���"); break;
	case BE_VOIDREALLOC:bbassert("VOID�^�̍Ċm�ۂł�"); break;
	}
	exit(1);
	return false;
}

BERROR BlackBoard::compile(char* Buffer){
	
	return BE_NOERROR;
}

