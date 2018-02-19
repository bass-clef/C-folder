

//								C-- 言語


// 最終履歴 3/21 1:28
// ・main(stackかなんかによる定義監視(再帰でも可))


#include <stack>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <iostream>

#include <windows.h>

#define inand(min, value, max) ((min <= value) && (value <= max))

using namespace std;

// 指定した文字列が変数の規定に従っているか
bool IsBufferVariable(const char* buffer, size_t length){
	if (inand('0', buffer[0], '9')) return false;
	bool b = true;
	for(size_t i=0; i<length; i++)
		if (!(inand('a', buffer[i], 'z') || inand('A', buffer[i], 'Z') ||
			(inand('0', buffer[i], '9')) || (buffer[i] == '_') || (buffer[i] == '@')))
		{ b = false; break; }
	return b;
}

// 指定した文字列が英字のみかを判断
bool IsBufferAlpha(const char* buffer, size_t length){
	bool b = true;
	for(size_t i=0; i<length; i++)
		if (!(inand('a', buffer[i], 'z') || inand('A', buffer[i], 'Z'))) { b = false; break; }
	return b;
}

// 指定した文字列が数字のみかを判断
bool IsBufferNumber(const char* buffer, size_t length, bool dot = false){
	bool b = true;
	for(size_t i=0; i<length; i++)
		if (!inand('0', buffer[i], '9')) if (dot){
			if (buffer[i] != '.'){ b = false; break; }
		}else { b = false; break; }
	return b;
}

enum { TKN_KEYS=0, TKN_OPER, TKN_IDEN, TKN_MAX };
int likenesscpp(char *from){
	#pragma region // Initialize
	// スルーワード {タブ, 改行, 再帰, 空白}
	const int disregardsmax = 6;
	char disregards[disregardsmax] = {0, 0x09, 0x0A, 0x0D, 0x20, 0x7F};

	// 区切り用文字
	const int extendsmax = 20;
	char extends[extendsmax] = {'{', '}', '(', ')', ' ', '\"', '\'', ';', ',',
	'+', '-', '*', '%', '=', '&', '!', '?', ':', '<', '>'};

	// 演算子
	const int operatorsmax = 47;
	char operators[operatorsmax][17] = {"::", "->", ".", "++", "--", "typeid",
	"~", "!", "+", "-", "*", "/", "%", "&", "|", "new", "delete", "delete[]",
	"sizeof", ".*", "->*", "<<", ">>", "<", ">", "<=", ">=", "==", "!=", "^",
	"&&", "||", "?", ":", "=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=",
	"&=", "|=", "^=", "throw", ","};

	// キーワード
	const int keysmax = 35;
	char keys[keysmax][13] = {
		"struct", "union",  "enum",
		"const",  "volatile",
		"extern",  "register",  "static",  "mutable",  "thread_local",
		"friend",  "typedef",
		"constexpr",  "explicit",  "inline",  "virtual",
		"public", "protected", "private",  "operator",  "this",
		"if", "else",  "for",  "while", "do",  "switch", "case", "default",
		"break",  "continue",  "goto",  "return",  "try", "catch"
	};
	list<pair<string, string>> token;
	list<pair<string, string>>::iterator it, its;
	pair<string, string> temp;
	int flagcnt = 0;
	bool flag = 0, error = 0;
	size_t size;
	#pragma endregion

	#pragma region // 字句解析
	// トークン
	temp.first = "";
	size = strlen(from);
	for(unsigned int i=0; i<size; i++){
		if (error) break;
		/* コメント除去 */
		if ((from[i] == '/') && (i <= size-1)) {
			if (from[i+1] == '*') {
				while(i < size-1)
					if ((from[i] == '*') && (from[i+1] == '/')) break; else i++;
				i+=2;
			}else if (from[i+1] == '/') {
				while(i < size) if (from[i] != 0x0A) i++; else break;
			}
		}

		for(int j=0; j<extendsmax; j++) if (from[i] == extends[j]){ flag = true; break; }
		if (flag) {// 区切り文字がきたら
			for(flagcnt=0; flagcnt<TKN_MAX; flagcnt++){
				switch(flagcnt){// フラグ確定カウント
				case TKN_KEYS:// キーワード検索
					for(int j=0; j<keysmax; j++) if (temp.first == keys[j]) {
						token.push_back(make_pair(temp.first, "KEYWORD"));
						flagcnt = TKN_MAX;
					}
					break;
				case TKN_OPER:// 演算子
					for(int j=0; j<operatorsmax; j++) if (temp.first == operators[j]){
						token.push_back(make_pair(temp.first, "OPERATOR"));
						flagcnt = TKN_MAX;
					}
					break;
				default:// 識別子
					if (temp.first.length() > 0)
						token.push_back(make_pair(temp.first, "IDENTIFIER"));
					flagcnt = TKN_MAX;
					break;
				}
			}
			// 区切り
			for(int j=0; j<disregardsmax; j++)
				if (from[i] == disregards[j]) flag = false;
			if (flag) {
				temp.first = from[i];
				for(int j=0; j<operatorsmax; j++)
					if (temp.first == operators[j]) flag = false;
				if (flag) token.push_back(make_pair(temp.first, "EXTEND"));
				else token.push_back(make_pair(temp.first, "OPERATOR"));
			}

			flag = false;
			temp.first = ""; 
		}else {
			for(int j=0; j<disregardsmax; j++)
				if (from[i] == disregards[j]) flag = true;
			if (!flag) temp.first += from[i];
			flag = false;
		}
	}
	#pragma endregion

	#pragma region // 構文解析
	// 識別子を分類
	it = token.begin();//
	int scopec = 0;
	stack<string> prevscope;
	list<pair<int, pair<string, string>>> global;
	list<pair<int, pair<string, string>>>::iterator git;;

	for(unsigned int i=0; (i<token.size()) && (it != token.end()); i++){
		if (it->first == "\""){// 文字列
			it = token.erase(it);
			its = it; it->second = "STRING"; ++it;
			while((it->first != "\"") && (it != token.end())){
				its->first += it->first;
				it = token.erase(it);
			}
			if (it != token.end()) it = token.erase(it);
			continue;
		}
		if (it->first == "{") {// スコープ
			--it;
			if (it->second == "TYPENAME") prevscope.push("TYPENAME");
			else { prevscope.push("NORMAL"); scopec++; }
			++it;
		}else if (it->first == "}") {
			if (prevscope.top() == "NORMAL") scopec--;
			prevscope.pop();
			if (global.size()){
				git = global.begin();
				while(git != global.end()){
					if (scopec <= git->first) git = global.erase(git);
					else ++git;
				}
			}
		}

		// キーワード
		if (it->first == "class"){ it->second = "TYPE"; }

		if (it->second == "IDENTIFIER"){// 識別子判別
			// 前スコープ定義済み検索
			if (global.size()){
				git = global.begin();
				flag = false;
				while(git != global.end()){
					if (git->first <= scopec) if (it->first == git->second.first) {
						it->second = git->second.second;
						flag = true;
						break;
					}
					++git;
				}
				if (flag) { ++it; continue; }
			}

			if (i < token.size()-1) {
				if (0 < i){ --it; its = it; ++it; }else its = it;
				++it;
				temp.first = it->first;
				temp.second = it->second;
				--it;
				if (its->second == "TYPE") {// 型
					it->second = "TYPENAME";
					global.push_back(make_pair(scopec, make_pair( it->first , it->second )));
				}else if ((temp.first == "(") || ((temp.first == "{") && (scopec == 0))) {
					// 後が括弧
					it->second = "FUNCTION";
					global.push_back(make_pair(scopec, make_pair( it->first , it->second )));
				}else {// 後に括弧がない
					if ((IsBufferVariable(it->first.c_str(), it->first.length())) && ((temp.second == "OPERATOR") || (its->second == "TYPENAME"))){
						it->second = "VARIABLE";
						global.push_back(make_pair(scopec, make_pair( it->first , it->second )));
					}else if (IsBufferNumber(it->first.c_str(), it->first.length()))
						it->second = "NUMBER";
					else if (IsBufferNumber(it->first.c_str(), it->first.length(), true))
						it->second = "DECIMAL";
					else {
						cout << "first[" << it->first << "]second[" << it->second << "]" << endl;
						
						// ・型不確定一行関数
						// ・型暗示的初期化式
						// ・型
						// ・型修飾子
						// ・～指定子
					}
				}
			}

		}
		++it;
	}
	#pragma endregion

	//do
	cout << "size[" << token.size() << "]" << endl;
	while(token.size()){
		cout << "[" << token.front().first;
		cout << "@" << token.front().second << "]" << endl;
		token.pop_front();
	}
	//~do

	return 0;
}

int main(){
	char *initcode = "\
	class bool;\n\
	class char;\n\
	class int;\n\
	class double;\n\
	class void;\n\
	false = 0;\n\
	true = 1;\n\
	";
	// テスト用テキスト (FizzBuzz)
	string buf = "";
	buf += "main { FizzBuzz(); }\n";
	buf += "\n";
	buf += "class test {\n";
	buf += "	FizzBuzz{\n";
	buf += "		tell = false;\n";
	buf += "		for(int i=1; <100; ++){\n";
	buf += "			if (i%3 == 0){ mes(\"Fizz\"); tell = true; }\n";
	buf += "			if (%5 == 0){\n";
	buf += "				mes(\"Buzz\");\n";
	buf += "				tell = true;";
	buf += "			}\n";
	buf += "			if (!tell) mes(\"%di%\");\n";
	buf += "			mes(\"\\n\")";
	buf += "		}\n";
	buf += "	}\n";
	buf += "}\n";

	string frombuf2;
	frombuf2 = "";
	frombuf2 += initcode;
	frombuf2 += buf.data();

	char *frombuf = new char[frombuf2.length()+1];
	memcpy(frombuf, frombuf2.data(), frombuf2.length());

	likenesscpp(frombuf);

	char str[16];
	cin >> str;

	delete[] frombuf;

	

	return 0;
}
