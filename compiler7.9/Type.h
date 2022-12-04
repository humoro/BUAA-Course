#pragma once
#ifndef COMPILER7_2_TYPE_H
#define COMPILER7_2_TYPE_H
typedef int BlockNumber;
typedef int RegisterNumber;
typedef unsigned int MemoryNumber;
//-------------------------------------------------词法分析token类型---------------------------------------------------//
enum TokenType {
    token_identifier,
    token_intConstant,
    token_charConstant,
    token_strConstant,
    token_constToken,
    token_intToken,
    token_charToken,
    token_voidToken,
    token_mainToken,
    token_ifToken,
    token_elseToken,
    token_doToken,
    token_whileToken,
    token_forToken,
    token_scanfToken,
    token_printfToken,
    token_returnToken,
    token_plus,
    token_minus,
    token_multiply,
    token_divide,
    token_less,
    token_lequal,
    token_greater,
    token_gequal,
    token_equal,
    token_nequal,
    token_assign,
    token_semicolon,
    token_comma,
    token_lparent,
    token_rparent,
    token_lbrack,
    token_rbrack,
    token_lbrace,
    token_rbrace,
    token_error = -1
};
//----------------------------------------------语法分析的非终结符类型---------------------------------------------------//
enum SyntaxType{
    syntax_program, // 程序...
    syntax_constantDecl,//常量说明...
    syntax_ConstantDef,//常量定义...
    syntax_variableDecl,//变量说明
    syntax_variableDef,//变量定义
    syntax_valueReturnFunc,//有返回函数定义
    syntax_voidReturnFunc,//无返回值函数定义
    syntax_cmpdStmt,//复合语句
    syntax_ParameterTable,//函数参数表
    syntax_mainFunction,//主函数
    syntax_expression, //表达式非终结符
    syntax_item, // 表达式项
    syntax_factor, // 表达式因子
    syntax_statement,//语句
    syntax_assign,//赋值语句
    syntax_conditionStmt,//条件语句
    syntax_condition,//条件
    syntax_circulationStmt,//循环语句
    syntax_valueRetFuncCall,//有返回值函数的调用
    syntax_voidRetFuncCall,//无返回值函数调用
    syntax_funcCallValueTable,//调用函数的数值参数表
    syntax_stmtColumn,//语句列
    syntax_readStmt,//读语句
    syntax_printStmt,//写语句
    syntax_returnStmt,//返回语句
    // 带ir后缀的类型是为了满足在语法分析构建语法树的时候只有叶子节点才有中间代码语句而创建的节点类型
    syntax_stmtLabel_ir,
    syntax_functionLabel_ir,
    syntax_endProgram,
    syntax_printStmt_ir,
    syntax_returnStmt_ir,
    syntax_assignStmt_ir,
    syntax_conditionalJump_ir,
    syntax_unconditionalJump_ir,
    syntax_operation_ir,
    syntax_funcCallStmt_ir,
    syntax_funcReturnAddrStore_ir,
    syntax_funcCallValuePass_ir
};
//---------------------------------------------------符号表项类型------------------------------------------------------//
enum SymbolType{
    //variableType
    symbol_intConstant,//const int 变量
    symbol_intVariable,//普通int变量
    symbol_charConstant,//const char
    symbol_charVariable,//普通的char变量
    symbol_intArray,
    symbol_charArray,
    //functionType
    symbol_intReturnFunction,
    symbol_charReturnFunction,
    symbol_voidReturnFunction,
    symbol_error
};
//-------------------------------------------------表达式返回值类型-----------------------------------------------------//
enum ExprType{
    expr_char,
    expr_int,
    expr_none
};
//-------------------------------------------------语法语义错误类型-----------------------------------------------------//
enum ErrorType{
    error_a,//非法符号或不符合词法
    error_b,//名字重定义（函数名称重定义和变量名字重定义）
    error_c,//未定义的名字
    error_d,//函数参数个数不匹配
    error_e,//函数参数类型不匹配
    error_f,//条件判断中出现不合法的类型
    error_g,//无返回值的函数存在不匹配的return语句
    error_h,//有返回值的函数缺少return语句或存在不匹配的return语句
    error_i,//数组元素的下表只能是整型表达式
    error_j,//不能改变常亮的数值
    error_k,//应为分号
    error_l,//应为')'
    error_m,//应为']'
    error_n,//do-while应为语句中缺少while
    error_o,//常量定义中=后面只能是整型或字符型常亮
    error_p//读取变量应该是普通的变量
};
//--------------------------------------------------中间代码类型-------------------------------------------------------//
enum IRType{
    ir_globalConstantDef,
    ir_partialConstantDef,
    ir_globalVariableDef,
    ir_partialVariableDef,
    //函数相关：
    ir_funcCallBegin,
    ir_funcReturnAddrStore,
    ir_funcDefLabel,//函数定义标签
    ir_voidRetutnFuncCall,//无返回值函数调用
    ir_valueReturnFuncCall,//返回值函数调用
    ir_funcParameterDef,//函数定义参数
    ir_funcCallParaPush,//函数调用参数
    ir_valueReturn,//有返回值返回
    ir_voidReturn,//无返回值返回语句
    ir_stmtLabel,//除了函数定义之外的各种标签(for do while code)
    ir_greJump,//大于跳转
    ir_geqJump,//大于等于跳转
    ir_lesJump,//小于跳转
    ir_leqJump,//小于等于跳转
    ir_eqlJump,//等于跳转
    ir_neqJmp,//不等于跳转
    ir_jump,//无条件跳转
    ir_readInt,
    ir_readChar,
    ir_printStr,
    ir_printInt,
    ir_printChar,
    ir_printNewLine,
    ir_operation,//四元计算式
    ir_assign,
    ir_error
};
//--------------------------------------------------目标代码类型-------------------------------------------------------//
enum mipsType{ // 包含所有使用到的指令
    mips_segementSet,
    mips_globalData,
    mips_codeLabel,
    mips_swRegImm, // 利用寄存器和偏移量寻址
    mips_swRegLabel, // 利用寄存器和标签寻址
    mips_swLabelImm, // 利用标签加立即数寻址
    mips_lwRegImm,
    mips_lwRegLabel,
    mips_lwLabelImm,
    mips_loadImm,
    mips_addImm,
    mips_addReg,
    mips_subImm,
    mips_subReg,
    mips_mulImm,
    mips_divImm,
    mips_mulReg,
    mips_divReg,
    mips_moveReg,
    mips_loadAddr,
    mips_shiftLeft,
    mips_branchZero, // 寄存器和0做比较的branch语句
    mips_branchReg, // 两个寄存器或者寄存器和立即数比较的branch
    mips_jalFunction,
    mips_jLabel,
    mips_jReg,
    mips_syscall
};
#endif
