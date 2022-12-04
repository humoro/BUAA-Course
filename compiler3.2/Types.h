//��ͷ�ļ��Ƕ����ʷ��еĸ��ֱ�ʶ���������ս�����ս��
//��ͷ���а�������ַ����ַ������������͵��жϺ���������
#pragma once
#ifndef COMPILER_3_2_TYPES_H
#define COMPILER_3_2_TYPES_H
#include<iostream>
using namespace std;
//�ս���ͷ��ս��������
enum myType {
    IDENFR,
    INTCON,
    CHARCON,
    STRCON,
    CONSTTK,
    INTTK,
    CHARTK,
    VOIDTK,
    MAINTK,
    IFTK,
    ELSETK,
    DOTK,
    WHILETK,
    FORTK,
    SCANFTK,
    PRINTFTK,
    RETURNTK,
    PLUS,
    MINU,
    MULT,
    DIV,
    LSS,
    LEQ,
    GRE,
    GEQ,
    EQL,
    NEQ,
    ASSIGN,
    SEMICN,
    COMMA,
    LPARENT,
    RPARENT,
    LBRACK,
    RBRACK,
    LBRACE,
    RBRACE,
    Non_String,//�ַ������ս��
    Non_Program,//����...
    Non_Const_Decl,//����˵��...
    Non_Const_Def,//��������...
    Non_Unsigned_int,//�޷�������...
    Non_Integer,//����...
    Non_Func_Head,//����ͷ��...
    Non_Var_Decl,//����˵��
    Non_Var_Def,//��������
    Non_Return_Func,//�з��غ�������
    Non_Void_Func,//�޷���ֵ��������
    Non_Cmpd_Stat,//�������
    Non_Func_ParaList,//����������
    Non_Main_Func,//������
    Non_Expr,//���ʽ
    Non_Item,//����ս��
    Non_Factor,//����
    Non_Stat,//���
    Non_Assign,//��ֵ���
    Non_Condition_Stat,//�������
    Non_Condition,//����
    Non_Recycle_Stat,//ѭ�����
    Non_Step_Length,//ѭ�����Ĳ���
    Non_ReturnFunc_Call,//�з���ֵ�����ĵ���
    Non_VoidFunc_Call,//�޷���ֵ��������
    Non_Value_Table,//���ú�������ֵ������
    Non_Stat_Column,//�����
    Non_Read_Stat,//�����
    Non_Print_Stat,//д���
    Non_Return_Stat,//�������
    SyntaxModel,
    ERROR = -1
};

//�ַ������жϺ���
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
bool isULine(char ch);
bool isExm(char ch);
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
//��ȡ���͵�����
string getTypeName(myType type);
#endif