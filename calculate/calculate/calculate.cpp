
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

// 数字か？
inline bool IsNum(Ttype any){
	if ('0' <= any && any <= '9') return true;
	return false;
}

// 文字を数字に変換
inline Ttype cnvnum(Ttype any){
	if (IsNum(any)) return (any-'0');
	return 0;
}

// char 作成
char** createchar(int byte){
	cchar_c.push(byte);
	return cchar_c.top();
}

// 一番最後を pop
void popchar(void){
	if (! cchar_c.size()) cchar_c.pop();
}

// char すべて削除
void deletechar(void){
	cchar_c.apop();
}

// 文字列のindex番目取得
char getpeek(char* str, int index){
	return str[index];
}

// double型の剰余
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

// 演算子の優先順位を取得
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

// 式を逆ポーランド記法に変換
char* ToRPN(LPSTR FromBuf, LPSTR ToBuf, char rn){
	for(int i = 0; i < lstrlen(FromBuf); i++)// 空白,改行 削除
		if ((FromBuf[i] == ' ') || (FromBuf[i] == '\r') || (FromBuf[i] == '\n'))
			for(int j = i; j < lstrlen(FromBuf); j++)
				FromBuf[j] = FromBuf[j+1];

	for(int i = 0; i < lstrlen(FromBuf); ++i) ToBuf[i] = 0;

	char tokun;
	stack<char> s;

	int i = 0, c = 0, cnt = 0, tPri, sPri;
	char lc;

	LOOP1:// 逆ポーランドへ変換
	tokun = FromBuf[i];
	i++;

	if (tokun <= NULL) {// スタックが空になるまでポップ
		while(1) {
			if (s.empty()) break;
			ToBuf[cnt] = s.top();
			s.pop();
			cnt++;
		}
		goto BREAK1;
	}

	if (In(tokun, '0', '9') | (tokun == '.')) {// 数字
		ToBuf[cnt] = tokun;// トークンをバッファに追加
		cnt++;
		c = 1;
		goto LOOP1;
	}

	if (c == 1) {// 数字を区切る
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

	if (s.empty()) {// スタックが空
		s.push(tokun);
		goto LOOP1;
	} else {
		tPri = GetPriority(Priority, tokun);
		lc = s.top();
		sPri = GetPriority(Priority, lc);
		if (tPri > sPri) {
			// スタックの最上位の演算子より
			// トークンの演算子の優先順位が低い
			s.push(tokun);
			goto LOOP1;
		}
	}

	// スタックからポップそれをバッファへ
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

// 逆ポーランド記法を計算
Ttyped Calculate(LPSTR buf, char rn){
	// エラー時は戻り値は INT_MAX
	// ・式を配列に格納 (優先順位を基準にして)
	// ・数字は脇に
	// ・演算子が来たら脇の数字を計算
	// nError	: N0:異なる演算子がならんでいる
	//			: N1:のみの式になっている (=式が成立してない)
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
	if (flag) {// 数式に無関係のトークンが含まれている
		flag = 0;
		while(o.size()){// 小数点か確認
			if (o.top() != '.') flag = 1;
			o.pop();
		}
		if (flag) return (Ttyped)Tmax;// 計算不能
		return Calculated(buf, rn);
	}else while(o.size()) o.pop();

	Ttype i1 = 0, i2 = 0, token = 0;
	stack<Ttype> s;

	for(int i=0; i<lstrlen(buf); i++){
		token = (Ttype)buf[i];

		if (IsNum(token)) {	// 回避文字 (rn) がくるまで数字と認識する
			i1 = i1 * 10 + cnvnum(token);	// 複数桁の数字に対応
		}else if (token == (Ttype)rn) {	// 回避文字が来たら数字の終了
			s.push(i1);		i1 = 0;		i2 = 0;
		}else {
			if (i1){	// 数字の終了
				s.push(i1);		i1 = 0;		i2 = 0;
			}
			if (! s.size()) {// 演算子 (前置)
				o.push((int)token);
				continue;
			}

			i1 = s.top();
			s.pop();
			if (! s.size()) {// 演算子 (後置)
				o.push((int)token);
				s.push(i1);
				i1 = 0;
				continue;
			}else {
				i2 = s.top();
				s.pop();
				if (o.size()) {// スタックに演算子あり
					switch(o.size()){
					case 1:// 単項
						switch(o.top()){
						case '+':	i1 = (+i1);		break;
						case '-':	i1 = (-i1);		break;
						}
						o.pop();
						break;
					case 2:{// 演算子が二つ続いた
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

	if (o.size()) {// のみ nError.N1
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

// 逆ポーランド記法を計算(小数可)
Ttyped Calculated(LPSTR buf, char rn){
	// エラー時は戻り値は INT_MAX
	// ・式を配列に格納 (優先順位を基準にして)
	// ・数字は脇に
	// ・演算子が来たら脇の数字を計算
	// nError	: N0:異なる演算子がならんでいる
	//			: N1:のみの式になっている (=式が成立してない)
	Ttyped i1 = 0.0, i2 = 0.0, token = 0.0;
	int flag = 0;
	stack<Ttyped> s;
	stack<short> o;

	for(int i=0; i<lstrlen(buf); i++){
		token = (Ttyped)buf[i];

		if (IsNum((short)token)) {	// 回避文字 (rn) がくるまで数字と認識する
			if (flag & 1) {
				i1 = i1 + cnvnum((short)token) / 10.0;	// 小数変換
			}else {
				i1 = i1 * 10.0 + cnvnum((short)token);	// 複数桁の数字に対応
			}
		}else if (token == '.'){
			flag |= 1;
		}else if (token == (Ttyped)rn) {	// 回避文字が来たら数字の終了
			s.push(i1);		i1 = 0.0;		i2 = 0.0;
			flag = 0;
		}else {
			if (i1){	// 数字の終了
				s.push(i1);		i1 = 0.0;		i2 = 0.0;
			}
			if (! s.size()) {// 演算子 (前置)
				o.push((short)token);
				continue;
			}

			i1 = s.top();
			s.pop();
			if (! s.size()) {// 演算子 (後置)
				o.push((short)token);
				s.push(i1);
				i1 = 0.0;
				continue;
			}else {
				i2 = s.top();
				s.pop();
				if (o.size()) {// スタックに演算子あり
					switch(o.size()){
					case 1:// 単項
						switch((int)o.top()){
						case '+':	i1 = (+i1);		break;
						case '-':	i1 = (-i1);		break;
						}
						o.pop();
						break;
					case 2:{// 演算子が二つ続いた
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

	if (o.size()) {// のみ nError.N1
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

