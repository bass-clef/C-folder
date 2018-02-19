
// Reverse Polish Notation : RPN

//#include <iostream>
#include <stack>
#include <stdio.h>
#include <math.h>
#include "calcuelate.h"
using namespace std;

namespace {
	class cchar {
		stack<char*>	c;
	public:
		~cchar(){ apop(); }

		void push(int byte = 1){ c.push(new char[byte]); }
		void pop(){ delete[] c.top(); c.pop(); }
		void apop(){ while(c.size()) pop(); }
		char** top(){ return (char**)c.top(); }
		int size(){ return c.size(); }
	};

	cchar			cchar_c;
}

namespace {
	char Priority[] = "-+%/*";
}

// �������H
inline bool IsNum(Ttype any){
	if ('0' <= any && any <= '9') return true;
	return false;
}

// �����𐔎��ɕϊ�
inline Ttype cnvnum(Ttype any){
	if (IsNum(any)) return (any-'0');
	return 0;
}

// char �쐬
char** createchar(int byte){
	cchar_c.push(byte);
	return cchar_c.top();
}

// ��ԍŌ�� pop
void popchar(void){
	if (! cchar_c.size()) cchar_c.pop();
}

// char ���ׂč폜
void deletechar(void){
	cchar_c.apop();
}

// �������index�Ԗڎ擾
char getpeek(char* str, int index){
	return str[index];
}

// double�^�̏�]
double modulo(double left,double right){
    if(right<0) right=-right;

    if(left>0){
        while(left-right>=0){
            left-=right;
        }
    }else if(left<0){
        do{
            left+=right;
        }while(left<0);
    }
    return left;
}

// ���Z�q�̗D�揇�ʂ��擾
int GetPriority(LPSTR Priority, char buf){
	int n = -1;
	for(int i = 0; i < lstrlen(Priority); ++i) {
		if (Priority[i] == buf) {
			n = i;
			break;
		}
	}
	return n;
}

// �����t�|�[�����h�L�@�ɕϊ�
char* ToRPN(LPSTR FromBuf, LPSTR ToBuf, char rn){
	for(int i = 0; i < lstrlen(FromBuf); i++)// ��,���s �폜
		if ((FromBuf[i] == ' ') || (FromBuf[i] == '\r') || (FromBuf[i] == '\n'))
			for(int j = i; j < lstrlen(FromBuf); j++)
				FromBuf[j] = FromBuf[j+1];

	for(int i = 0; i < lstrlen(FromBuf); ++i) ToBuf[i] = 0;

	char tokun;
	stack<char> s;

	int i = 0, c = 0, cnt = 0, tPri, sPri;
	char lc;

	LOOP1:// �t�|�[�����h�֕ϊ�
	tokun = FromBuf[i];
	i++;

	if (tokun <= NULL) {// �X�^�b�N����ɂȂ�܂Ń|�b�v
		while(1) {
			if (s.empty()) break;
			ToBuf[cnt] = s.top();
			s.pop();
			cnt++;
		}
		goto BREAK1;
	}

	if (In(tokun, '0', '9') | (tokun == '.')) {// ����
		ToBuf[cnt] = tokun;// �g�[�N�����o�b�t�@�ɒǉ�
		cnt++;
		c = 1;
		goto LOOP1;
	}

	if (c == 1) {// ��������؂�
		ToBuf[cnt] = rn;
		cnt++;
		c = 0;
	}

	if (tokun == ')') {
		while(1) {
			tokun = s.top();
			s.pop();
			if (tokun == '(') break;
			ToBuf[cnt] = tokun;
			cnt++;
		}
		goto LOOP1;
	}

	if (tokun == '(') {
		s.push(tokun);
		goto LOOP1;
	}

	LOOP2:

	if (s.empty()) {// �X�^�b�N����
		s.push(tokun);
		goto LOOP1;
	} else {
		tPri = GetPriority(Priority, tokun);
		lc = s.top();
		sPri = GetPriority(Priority, lc);
		if (tPri > sPri) {
			// �X�^�b�N�̍ŏ�ʂ̉��Z�q���
			// �g�[�N���̉��Z�q�̗D�揇�ʂ��Ⴂ
			s.push(tokun);
			goto LOOP1;
		}
	}

	// �X�^�b�N����|�b�v������o�b�t�@��
	ToBuf[cnt] = s.top();
	s.pop();
	cnt++;
	goto LOOP2;

	BREAK1:

	for(int i = 0; i < lstrlen(ToBuf); i++) {
		if (ToBuf[i] <= 0) {
			ToBuf[i] = NULL;
			break;
		}
	}

	return ToBuf;
}

// �t�|�[�����h�L�@���v�Z
Ttyped Calculate(LPSTR buf, char rn){
	// �G���[���͖߂�l�� INT_MAX
	// �E����z��Ɋi�[ (�D�揇�ʂ���ɂ���)
	// �E�����͘e��
	// �E���Z�q��������e�̐������v�Z
	// nError	: N0:�قȂ鉉�Z�q���Ȃ��ł���
	//			: N1:�݂̂̎��ɂȂ��Ă��� (=�����������ĂȂ�)
	bool flag = 0;
	stack<short> o;

	for(int i=0; i<lstrlen(buf); i++){
		switch(buf[i]){
		case '+': case '-': case '*': case '/':
		case '%': case '&': case '|': case '^':
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		case '8': case '9':
			break;
		default:
			if (buf[i] != rn) {
				flag = 1;
				o.push(buf[i]);
			}
		}
	}
	if (flag) {// �����ɖ��֌W�̃g�[�N�����܂܂�Ă���
		flag = 0;
		while(o.size()){// �����_���m�F
			if (o.top() != '.') flag = 1;
			o.pop();
		}
		if (flag) return (Ttyped)Tmax;// �v�Z�s�\
		return Calculated(buf, rn);
	}else while(o.size()) o.pop();

	Ttype i1 = 0, i2 = 0, token = 0;
	stack<Ttype> s;

	for(int i=0; i<lstrlen(buf); i++){
		token = (Ttype)buf[i];

		if (IsNum(token)) {	// ��𕶎� (rn) ������܂Ő����ƔF������
			i1 = i1 * 10 + cnvnum(token);	// �������̐����ɑΉ�
		}else if (token == (Ttype)rn) {	// ��𕶎��������琔���̏I��
			s.push(i1);		i1 = 0;		i2 = 0;
		}else {
			if (i1){	// �����̏I��
				s.push(i1);		i1 = 0;		i2 = 0;
			}
			if (! s.size()) {// ���Z�q (�O�u)
				o.push((int)token);
				continue;
			}

			i1 = s.top();
			s.pop();
			if (! s.size()) {// ���Z�q (��u)
				o.push((int)token);
				s.push(i1);
				i1 = 0;
				continue;
			}else {
				i2 = s.top();
				s.pop();
				if (o.size()) {// �X�^�b�N�ɉ��Z�q����
					switch(o.size()){
					case 1:// �P��
						switch(o.top()){
						case '+':	i1 = (+i1);		break;
						case '-':	i1 = (-i1);		break;
						}
						o.pop();
						break;
					case 2:{// ���Z�q���������
						Ttype i0 = o.top(); o.pop();
						if (o.top() == i0){
							switch(i2){
							case '+':	i1++;		break;
							case '-':	i1--;		break;
							}
						}else {
							printf("exception. N0\n");
							//s.push(LLONG_MAX);
							//break;
							i--;
							continue;
						}
						o.pop();
						s.push(i1);
						s.push(i2);
						break;}
					}
				}/*else if (i < lstrlen(buf)-1){
					o.push(token);
					s.push(i2);
					s.push(i1);
					i1 = 0;		i2 = 0;
					continue;
				}*/
			}


			switch(token){
			case '+':	s.push(i2+i1);	break;
			case '-':	s.push(i2-i1);	break;
			case '*':	s.push(i2*i1);	break;
			case '/':	s.push(i2/i1);	break;
			case '%':	s.push(i2%i1);	break;
			case '&':	s.push(i2&i1);	break;
			case '|':	s.push(i2|i1);	break;
			case '^':	s.push(i2^i1);	break;
			}

			i1 = 0;
			i2 = 0;
		}
	}

	if (o.size()) {// �̂� nError.N1
		printf("exception. N1\n");
		printf("The remainder numbers.\n", s.top());
		for(int i=0; s.size(); i++){
			printf("%d\n", s.top());
			s.pop();
		}

		printf("The remainder operatores.\n");
		for(int i=0; o.size(); i++){
			printf("%d\n", o.top());
			o.pop();
		}
		return (Ttyped)s.top();
	}
	if (! s.size()) s.push(0);
	return (Ttyped)s.top();
}

// �t�|�[�����h�L�@���v�Z(������)
Ttyped Calculated(LPSTR buf, char rn){
	// �G���[���͖߂�l�� INT_MAX
	// �E����z��Ɋi�[ (�D�揇�ʂ���ɂ���)
	// �E�����͘e��
	// �E���Z�q��������e�̐������v�Z
	// nError	: N0:�قȂ鉉�Z�q���Ȃ��ł���
	//			: N1:�݂̂̎��ɂȂ��Ă��� (=�����������ĂȂ�)
	Ttyped i1 = 0.0, i2 = 0.0, token = 0.0;
	int flag = 0;
	stack<Ttyped> s;
	stack<short> o;

	for(int i=0; i<lstrlen(buf); i++){
		token = (Ttyped)buf[i];

		if (IsNum((short)token)) {	// ��𕶎� (rn) ������܂Ő����ƔF������
			if (flag & 1) {
				i1 = i1 + cnvnum((short)token) / 10.0;	// �����ϊ�
			}else {
				i1 = i1 * 10.0 + cnvnum((short)token);	// �������̐����ɑΉ�
			}
		}else if (token == '.'){
			flag |= 1;
		}else if (token == (Ttyped)rn) {	// ��𕶎��������琔���̏I��
			s.push(i1);		i1 = 0.0;		i2 = 0.0;
			flag = 0;
		}else {
			if (i1){	// �����̏I��
				s.push(i1);		i1 = 0.0;		i2 = 0.0;
			}
			if (! s.size()) {// ���Z�q (�O�u)
				o.push((short)token);
				continue;
			}

			i1 = s.top();
			s.pop();
			if (! s.size()) {// ���Z�q (��u)
				o.push((short)token);
				s.push(i1);
				i1 = 0.0;
				continue;
			}else {
				i2 = s.top();
				s.pop();
				if (o.size()) {// �X�^�b�N�ɉ��Z�q����
					switch(o.size()){
					case 1:// �P��
						switch((int)o.top()){
						case '+':	i1 = (+i1);		break;
						case '-':	i1 = (-i1);		break;
						}
						o.pop();
						break;
					case 2:{// ���Z�q���������
						Ttyped i0 = o.top(); o.pop();
						if (o.top() == i0){
							switch((int)i2){
							case '+':	i1++;		break;
							case '-':	i1--;		break;
							}
						}else {
							printf("exception. N0\n");
							//s.push(LLONG_MAX);
							//break;
							i--;
							continue;
						}
						o.pop();
						s.push(i1);
						s.push(i2);
						break;}
					}
				}/*else if (i < lstrlen(buf)-1){
					o.push(token);
					s.push(i2);
					s.push(i1);
					i1 = 0.0;		i2 = 0.0;
					continue;
				}*/
			}

			switch((int)token){
			case '+':	s.push(i2+i1);	break;
			case '-':	s.push(i2-i1);	break;
			case '*':	s.push(i2*i1);	break;
			case '/':	s.push(i2/i1);	break;
			case '%':	s.push(modulo(i2, i1));	break;
			}
			i1 = 0.0;
			i2 = 0.0;
		}
	}

	if (o.size()) {// �̂� nError.N1
		printf("exception. N1\n");
		printf("The remainder numbers.\n", s.top());
		for(int i=0; s.size(); i++){
			printf("%d :", s.top());
			s.pop();
		}

		printf("The remainder operatores.\n");
		for(int i=0; o.size(); i++){
			printf("%d :", o.top());
			o.pop();
		}
		return s.top();
	}
	if (! s.size()) s.push(0);
	return s.top();
}

void Encryption(char* buf, char* lpFromTable, char* lpToTable, char tablemax, int filesize){
	for(register int i=0; i<filesize; i++){
		for(register unsigned char j=0; j<tablemax; j++){
			if (lpFromTable[j] == buf[i]) buf[i] = lpToTable[j];
		}
	}
}

