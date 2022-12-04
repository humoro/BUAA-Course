//本文件中主要是字符类型的判断函数的实现
#include<iostream>
using namespace std;
bool isSpace(char ch) {
	return isspace(ch);
}

bool isAlpha(char ch) {
	return isalpha(ch) || ch == '_';
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

bool isExm(char ch) {
    return ch == '!';
}

bool isNewLine(char ch) {
	return ch == '\n' ;//目前只考虑换行符的情况，不考虑'\r'
}

bool isLegalStringCh(char ch) {//字符串常量的合法字符
    return (ch == 32 || ch == 33 || (ch >= 35 && ch <= 126));
}

bool isLegalCharactor(char ch) {
    return (isPlus(ch) || isMinus(ch) || isAlpha(ch) || isDigit(ch) || isMult(ch) || isDivid(ch));
}
