//��ͷ�ļ��Ƕ����ʷ��еĸ��ֱ�ʶ���������ս�����ս��
//��ͷ���а�������ַ����ַ������������͵��жϺ���������
#pragma once
#ifndef COMPILER_5_2_TYPES_H
#define COMPILER_5_2_TYPES_H
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
//�ַ��������жϺ���
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
//token�������жϺ���
bool isIdentifierToken(TokenType type);
bool isIntConToken(TokenType type);
bool isCharConToken(TokenType type);
bool isStrConToken(TokenType type);
bool isConstToken(TokenType type);
bool isIntToken(TokenType type);
bool isCharToken(TokenType type);
bool isTypeIdenfrToken(TokenType type);
bool isVoidToken(TokenType type);
bool isMainToken(TokenType type);
bool isIfToken(TokenType type);
bool isElseToken(TokenType type);
bool isDoToken(TokenType type);
bool isWhile(TokenType type);
bool isForToken(TokenType type);
bool isScanfToken(TokenType type);
bool isPrintfToken(TokenType type);
bool isReturnToken(TokenType type);
bool isPlusToken(TokenType type);//�����Ӽ�
bool isMultipleToken(TokenType type);//�����˳�
bool isCompareToken(TokenType type);//���������ڣ�С�ڣ���С�ڣ������ڣ����ڣ�������
bool isAssignToken(TokenType type);
bool isSemiToken(TokenType type);
bool isCommaToken(TokenType type);
bool isLparentToken(TokenType type);
bool isLbrackToken(TokenType type);
bool isLbrace(TokenType type);
bool isRparentToken(TokenType type);
bool isRbrackToken(TokenType type);
bool isRbraceToken(TokenType type);

string getTokenName(TokenType type);
#endif