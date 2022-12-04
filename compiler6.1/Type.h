#pragma once
#ifndef COMPILER6_1_TYPE_H
#define COMPILER6_1_TYPE_H
//--------------------------词法分析token类型-------------------------//
enum TokenType {
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
    ERROR = -1
};

//--------------------------符号表表项类型-------------------------//
enum SymbolType{
    //variableType
    constIntVar,//const int 变量
    plantIntVar,//普通int变量
    constCharVar,//const char
    plantCharVar,//普通的char变量
    intArray,
    charArray,
    //functionType
    intReturnFunc,
    charReturnFunc,
    voidFunc,
    errType
};

//-------------------------表达式的返回值类型-------------------------//
enum ExprType{
    CharExp,
    IntExp,
    None
};

//--------------------------错误类型-------------------------//
enum ErrorType{
    a,//非法符号或不符合词法
    b,//名字重定义（函数名称重定义和变量名字重定义）
    c,//未定义的名字
    d,//函数参数个数不匹配
    e,//函数参数类型不匹配
    f,//条件判断中出现不合法的类型
    g,//无返回值的函数存在不匹配的return语句
    h,//有返回值的函数缺少return语句或存在不匹配的return语句
    i,//数组元素的下表只能是整型表达式
    j,//不能改变常亮的数值
    k,//应为分号
    l,//应为')'
    m,//应为']'
    n,//do-while应为语句中缺少while
    o,//常量定义中=后面只能是整型或字符型常亮
    p//读取变量应该是普通的变量
};
//-------------------------中间代码类型----------------------//
enum IRType{
    gConstDef,
    pConstDef,
    gVarDef,
    pVarDef,
    //函数相关：
    funcCallBegin,
    funcRetAddrSw,
    funcDefLabel,//函数定义标签
    nonRetFuncCall,//无返回值函数调用
    retFuncCall,//返回值函数调用
    funcDefPara,//函数定义参数
    funcCallPara,//函数调用参数
    valueRet,//有返回值返回
    voidRet,//无返回值返回语句
    statLabel,//除了函数定义之外的各种标签(for do while code)
    greJump,//大于跳转
    geqJump,//大于等于跳转
    lesJump,//小于跳转
    leqJump,//小于等于跳转
    eqlJump,//等于跳转
    neqJmp,//不等于跳转
    jump,//无条件跳转
    readInt,
    readChar,
    printStr,
    printInt,
    printChar,
    printNewLine,
    operation,//四元计算式
    assign,
    empty
};
#endif
