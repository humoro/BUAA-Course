//本文件中主要是字符类型的判断函数的实现
#include<iostream>
using namespace std;
bool isSpace(char ch) {
	return isspace(ch);
}

bool isAlpha(char ch) {
	return isalpha(ch);
}

bool isDigit(char ch) {
	return isdigit(ch);
}

bool isPlus(char ch) {
	return ch == '+';
}

bool isMinus(char ch) {
	return ch == '-';
}

bool isMult(char ch) {
	return ch == '*';
}

bool isDivid(char ch) {
	return ch == '/';
}

bool isAssign(char ch) {
	return ch == '=';
}

bool isSemi(char ch) {
	return ch == ';';
}

bool isComma(char ch) {
	return ch == ',';
}

bool isLparent(char ch) {
	return ch == '(';
}

bool isRparent(char ch) {
	return ch == ')';
}

bool isLbrack(char ch) {
	return ch == '[';
}

bool isRbrack(char ch) {
	return ch == ']';
}

bool isLbrace(char ch) {
	return ch == '{';
}

bool isRbrace(char ch) {
	return ch == '}';
}

bool isLss(char ch) {
	return ch == '<';
}

bool isGre(char ch) {
	return ch == '>';
}

bool isDQuo(char ch) {
    return ch == '\"';
}

bool isSQuo(char ch) {
    return ch == '\'';
}

bool isULine(char ch) {
    return ch == '_';
}

bool isExm(char ch) {
    return ch == '!';
}