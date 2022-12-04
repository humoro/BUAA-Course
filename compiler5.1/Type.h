#ifndef COMPILER4_1_TYPE_H
#define COMPILER4_1_TYPE_H
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
    voidFunc
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
    o//常量定义中=后面只能是整型或字符型常亮
};

#endif //COMPILER4_1_TYPE_H
