//本头文件是定义问法中的各种标识符包括非终结符和终结符
//本头文中包含相关字符和字符串保留字类型的判断函数的声明
#pragma once
#ifndef COMPILER7_2_TYPES_H
#define COMPILER7_2_TYPES_H
#include<string>
#include<iostream>
using namespace std;
bool isSpace(char ch);
bool isAlpha(char ch);
bool isDigit(char ch);
bool isPlus(char ch);
bool isMinus(char ch);
bool isMult(char ch);
bool isDivid(char ch);
bool isAssign(char ch);
bool isSemi(char ch);
bool isComma(char ch);
bool isLparent(char ch);
bool isRparent(char ch);
bool isLbrack(char ch);
bool isRbrack(char ch);
bool isLbrace(char ch);
bool isRbrace(char ch);
bool isLss(char ch);
bool isGre(char ch);
bool isDQuo(char ch);
bool isSQuo(char ch);
bool isExm(char ch);
bool isNewLine(char ch);
bool isLegalStringCh(char ch);
bool isLegalCharactor(char ch);
//字符串类型判断函数
bool isConst(string& str);
bool isInt(string& str);
bool isChar(string& str);
bool isVoid(string& str);
bool isMain(string& str);
bool isIf(string& str);
bool isElse(string& str);
bool isDo(string& str);
bool isWhile(string& str);
bool isFor(string& str);
bool isScanf(string& str);
bool isPrintf(string& str);
bool isReturn(string& str);
#endif