//本头文件是定义问法中的各种标识符包括非终结符和终结符
//本头文中包含相关字符和字符串保留字类型的判断函数的声明
#pragma once
#ifndef COMPILER_3_2_TYPES_H
#define COMPILER_3_2_TYPES_H
#include<iostream>
using namespace std;
//终结符和非终结符的类型
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
    Non_String,//字符串非终结符
    Non_Program,//程序...
    Non_Const_Decl,//常量说明...
    Non_Const_Def,//常量定义...
    Non_Unsigned_int,//无符号整数...
    Non_Integer,//整数...
    Non_Func_Head,//声明头部...
    Non_Var_Decl,//变量说明
    Non_Var_Def,//变量定义
    Non_Return_Func,//有返回函数定义
    Non_Void_Func,//无返回值函数定义
    Non_Cmpd_Stat,//复合语句
    Non_Func_ParaList,//函数参数表
    Non_Main_Func,//主函数
    Non_Expr,//表达式
    Non_Item,//项非终结符
    Non_Factor,//因子
    Non_Stat,//语句
    Non_Assign,//赋值语句
    Non_Condition_Stat,//条件语句
    Non_Condition,//条件
    Non_Recycle_Stat,//循环语句
    Non_Step_Length,//循环语句的步长
    Non_ReturnFunc_Call,//有返回值函数的调用
    Non_VoidFunc_Call,//无返回值函数调用
    Non_Value_Table,//调用函数的数值参数表
    Non_Stat_Column,//语句列
    Non_Read_Stat,//读语句
    Non_Print_Stat,//写语句
    Non_Return_Stat,//返回语句
    SyntaxModel,
    ERROR = -1
};

//字符类型判断函数
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
//获取类型的名字
string getTypeName(myType type);
#endif