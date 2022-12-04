#include<iostream>
using namespace std;
bool isConst(string& str) {
	return str == "const";
}

bool isInt(string& str) {
	return str == "int";
}

bool isChar(string& str) {
	return str == "char";
}

bool isVoid(string& str) {
	return str == "void";
}

bool isMain(string& str) {
	return str == "main";
}

bool isIf(string& str) {
	return str == "if";
}

bool isElse(string& str) {
	return str == "else";
}

bool isDo(string& str) {
	return str == "do";
}

bool isWhile(string& str) {
	return str == "while";
}

bool isFor(string& str) {
	return str == "for";
}

bool isScanf(string& str) {
	return str == "scanf";
}

bool isPrintf(string& str) {
	return str == "printf";
}

bool isReturn(string& str) {
	return str == "return";
}