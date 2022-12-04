//该文件中是相关的打印函数
#include<unordered_map>
#include "Types.h"
using namespace std;
string getTypeName(myType type)  {
    unordered_map<int, string> typeName {
            {IDENFR,           "IDENFR"},
            {INTCON,           "INTCON" },
            {CHARCON,          "CHARCON" },
            {STRCON,           "STRCON" },
            {CONSTTK,          "CONSTTK" },
            {INTTK,            "INTTK" },
            {CHARTK,           "CHARTK" },
            {VOIDTK,           "VOIDTK"},
            {MAINTK,           "MAINTK"},
            {IFTK,             "IFTK"},
            {ELSETK,           "ELSETK" },
            {DOTK,             "DOTK" },
            {WHILETK,          "WHILETK"},
            {FORTK,            "FORTK"},
            {SCANFTK,          "SCANFTK"},
            {PRINTFTK,         "PRINTFTK"},
            {RETURNTK,         "RETURNTK"},
            {PLUS,             "PLUS" },
            {MINU,             "MINU"},
            {MULT,             "MULT" },
            {DIV,              "DIV"},
            {LSS,              "LSS"},
            {LEQ,              "LEQ"},
            {GRE,              "GRE"},
            {GEQ,              "GEQ"},
            {EQL,              "EQL"},
            {NEQ,              "NEQ"},
            {ASSIGN,           "ASSIGN"},
            {SEMICN,           "SEMICN"},
            {COMMA,            "COMMA"},
            {LPARENT,          "LPARENT"},
            {RPARENT,          "RPARENT"},
            {LBRACK,           "LBRACK"},
            {RBRACK,           "RBRACK"},
            {LBRACE,           "LBRACE"},
            {RBRACE,           "RBRACE"},
            {Non_String,          "<字符串>"},//字符串非终结符
            {Non_Program,         "<程序>"},//程序...
            {Non_Const_Decl,      "<常量说明>"},//常量说明...
            {Non_Const_Def,       "<常量定义>"},//常量定义...
            {Non_Unsigned_int,    "<无符号整数>"},//无符号整数...
            {Non_Integer,         "<整数>"},//整数...
            {Non_Func_Head,       "<声明头部>"},//声明头部...
            {Non_Var_Decl,        "<变量说明>"},//变量说明
            {Non_Var_Def,         "<变量定义>"},//变量定义
            {Non_Return_Func,     "<有返回值函数定义>"},//有返回函数定义
            {Non_Void_Func,       "<无返回值函数定义>"},//无返回值函数定义
            {Non_Cmpd_Stat,       "<复合语句>"},//复合语句
            {Non_Func_ParaList,   "<参数表>"},//函数参数表
            {Non_Main_Func,       "<主函数>"},//主函数
            {Non_Expr,            "<表达式>"},//表达式
            {Non_Item,            "<项>"},//项非终结符
            {Non_Factor,          "<因子>"},//因子
            {Non_Stat,            "<语句>"},//语句
            {Non_Assign,          "<赋值语句>"},//赋值语句
            {Non_Condition_Stat,  "<条件语句>"},//条件语句
            {Non_Condition,       "<条件>"},//条件
            {Non_Recycle_Stat,    "<循环语句>"},//循环语句
            {Non_Step_Length,     "<步长>"},//循环语句的步长
            {Non_ReturnFunc_Call, "<有返回值函数调用语句>"},//有返回值函数的调用
            {Non_VoidFunc_Call,   "<无返回值函数调用语句>"},//无返回值函数调用
            {Non_Value_Table,     "<值参数表>"},//调用函数的数值参数表
            {Non_Stat_Column,     "<语句列>"},//语句列
            {Non_Read_Stat,       "<读语句>"},//读语句
            {Non_Print_Stat,      "<写语句>"},//写语句
            {Non_Return_Stat,     "<返回语句>"},//返回语句
            {SyntaxModel,         ""},
            {ERROR,               "ERROR"}
    };
    return typeName[type];
}