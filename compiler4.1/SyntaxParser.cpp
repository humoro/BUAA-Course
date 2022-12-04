#include<cstdio>
#include<unordered_map>
#include "Type.h"
#include "Parser.h"
#include "Token.h"
#include "Signary.h"
#include "Error.h"
using namespace std;
extern long long linenum;

void SyntaxParser() {
    programParser();
}

void callBackStreampos(streampos sp,long long oriLine) {
    cin.seekg(sp);
    linenum = oriLine;
}

streampos getStreampos(long long& oriLine) {
    oriLine = linenum;
    return cin.tellg();
}

void programParser() {
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type == CONSTTK) {//处理程序中的全局常量说明
        callBackStreampos(sp,oriLine);
        constDeclarationParser();
        sp = getStreampos(oriLine);
    }
    callBackStreampos(sp,oriLine);
    type = wordParser(ans);
    if (type == INTTK || type == CHARTK) { //处理程序的全局变量说明
        wordParser(ans);
        type = wordParser(ans);
        if (type != LPARENT) {//向后读两个单词，遇到左括号说明是函数定义
            callBackStreampos(sp,oriLine);
            variableDeclarationParser();
            sp = getStreampos(oriLine);
        }
    }
    callBackStreampos(sp,oriLine);
    type = wordParser(ans);
    bool reachMain = false;
    while ((type == INTTK || type == CHARTK || type == VOIDTK) && !reachMain) { //处理程序中的函数说明
        if (type == INTTK || type == CHARTK) {
            callBackStreampos(sp,oriLine);
            returnFuncDefinitionParser();
            sp = getStreampos(oriLine);
        } else {
            type = wordParser(ans);
            if (type == MAINTK) {
                reachMain = true;
            } else {
                callBackStreampos(sp,oriLine);
                voidFuncDefinitionParser();
                sp = getStreampos(oriLine);
            }
        }
        type = wordParser(ans);
    }
    callBackStreampos(sp,oriLine);
    mainFuncParser();
}

void constDeclarationParser() {//解析常量声明
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    while (type == CONSTTK) {
        constDefinitionParser();//解析常量的定义
        sp = getStreampos(oriLine);
        type = wordParser(ans);
        if (type != SEMICN) {
            error(k,oriLine);
			if (linenum > oriLine) {//如果是缺少分号而解析到了下一行
				callBackStreampos(sp, oriLine);
			}
        }
        sp = getStreampos(oriLine);
        type = wordParser(ans);
    }
    callBackStreampos(sp,oriLine);
}

void constDefinitionParser() {//解析常量定义
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    tokenType identifierType;
    identifierType = type;
    while (type != SEMICN) {
		sp = getStreampos(oriLine);
        type = wordParser(ans);
		if (linenum > oriLine) {//没有分号已经解析到下一行
			callBackStreampos(sp, oriLine);
			break;
		}
        if (type != IDENFR) error();
        addVariableEntry(identifierType, ans, true);//添加常量的符号表项
        type = wordParser(ans);
        if (type != ASSIGN) error();
        if (identifierType == INTTK) {
            type = wordParser(ans);
            if (type == PLUS || type == MINU) {
                type = wordParser(ans);
            }
            if (type != INTCON) error(o,linenum);//常量定义中的int型变量赋值不是整型
        } else  if (identifierType == CHARTK) {
            type = wordParser(ans);
            if (type != CHARCON) error(o,linenum);
        } else error();
        sp = getStreampos(oriLine);
        type = wordParser(ans);
        if (linenum > oriLine) {//没有分号已经解析到下一行
            callBackStreampos(sp,oriLine);
            break;
        }
    }
    callBackStreampos(sp,oriLine);
}

void variableDeclarationParser() {//解析变量声明
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    while (type == INTTK || type == CHARTK) {
        wordParser(ans);
        type = wordParser(ans);
        if (type == LPARENT) {
            break;
        } else {
            callBackStreampos(sp,oriLine);
            variableDefinitionParser();
            sp = getStreampos(oriLine);
            type = wordParser(ans);
            if (type != SEMICN) { //没有分号解析到下一行，需要回退
                error(k,oriLine);
                callBackStreampos(sp,oriLine);
            }
            sp = getStreampos(oriLine);
        }
        type = wordParser(ans);
    }
    callBackStreampos(sp,oriLine);
}

void variableDefinitionParser() {//解析变量定义
    string ans;
    tokenType type;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    type = wordParser(ans);//提取类型标识符
    tokenType identifierType = type;
    string varName;
    while (type != SEMICN) {
        type = wordParser(ans);
		sp = getStreampos(oriLine);
		if (linenum > oriLine) {//处理分号被替换的情况
			callBackStreampos(sp, oriLine);
			break;
		}
        if (type != IDENFR) error();
        varName = ans;//存储变量的标识符
        sp = getStreampos(oriLine);
        type = wordParser(ans);
        if (type != LBRACK) {
            addVariableEntry(identifierType,varName,false);//添加常量符号表项
            continue;
        }
        addArrayEntry(identifierType,varName);//添加数组的符号表项
        unsignedIntParser();
        sp = getStreampos(oriLine);
        type = wordParser(ans);
        if (type != RBRACK) {//缺少右中括号
            callBackStreampos(sp,oriLine);
            error(m,linenum);
        }
        sp = getStreampos(oriLine);
        type = wordParser(ans);
        if (linenum > oriLine) {//没有分号，解析到下一行，需要回退
            callBackStreampos(sp,oriLine);
            break;
        }
    }
    callBackStreampos(sp,oriLine);
}

void unsignedIntParser() {
    string ans;
    tokenType type = wordParser(ans);
    if (type != INTCON) error();
    if (ans.size() > 1 && ans[0] == '0') error();
}

void returnFuncDefinitionParser() {
    string ans;
    tokenType type = wordParser(ans);
    string functionName;
    if (type != INTTK && type != CHARTK) error();
    tokenType type1 = wordParser(functionName);
    if (type1 != IDENFR) error();
    addFunction(type,functionName);//添加有返回值函数符号表项
    funcStatementParser(functionName);
}

void voidFuncDefinitionParser() {
    string ans;
    tokenType type = wordParser(ans);
    string functionName;
    if (type != VOIDTK) error();
    type = wordParser(functionName);
    if (type != IDENFR) error();
    addFunction(VOIDTK,functionName);//添加无返回值符号表项
    funcStatementParser(functionName);
}

void funcStatementParser(string& functionName) {//主要是解析函数的参数表和执行语句
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type != LPARENT) error();
    funcParameterListParser();//解析函数的参数表
    sp = getStreampos(oriLine);
    type = wordParser(ans);
    if (type != RPARENT) {
        callBackStreampos(sp,oriLine);
        error(l,linenum);//参数表缺少右括号
    }
    type = wordParser(ans);
    if (type != LBRACE) error();
    compoundStatementParser(functionName);
    type = wordParser(ans);
    if (type != RBRACE) error();
}

void funcParameterListParser() {//解析函数参数表
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType varType = wordParser(ans);
    tokenType identifierType = varType;//记录函数标识符的类型
    if (varType == RPARENT || varType == LBRACE) {//没有右括号的时候终止需要通过达到大括号来判断
        callBackStreampos(sp,oriLine);
    } else {
        for (;;) {
            if (identifierType != INTTK && identifierType != CHARTK) error();
            varType = wordParser(ans);
            if (varType != IDENFR) error();
            sp = getStreampos(oriLine);
            addFunctionParameter(identifierType,ans);//添加参数表内容到函数的符号表
            varType = wordParser(ans);
            if (varType == COMMA) {
                identifierType = wordParser(ans);
            } else if (varType == RPARENT || varType == LBRACE){ //没有右括号的时候终止需要通过达到大括号来判断
                break;
            } else {
                error();
                break;
            }
        }
        callBackStreampos(sp,oriLine);
    }
}

void compoundStatementParser(string& functionName) {//复合语句的解析程序
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type == CONSTTK) {//常量说明
        callBackStreampos(sp,oriLine);
        constDeclarationParser();
        sp = getStreampos(oriLine);
        type = wordParser(ans);
    }
    if (type == INTTK || type == CHARTK) {//变量说明
        callBackStreampos(sp,oriLine);
        variableDeclarationParser();
        sp = getStreampos(oriLine);
    }
    callBackStreampos(sp,oriLine);
    statementColumnParser();
    checkFuntionReturn();
}

void statementColumnParser() {
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    while (type != RBRACE) {
        callBackStreampos(sp,oriLine);
        statementParser();
        sp = getStreampos(oriLine);
        type = wordParser(ans);
    }
    callBackStreampos(sp,oriLine);
}

void statementParser() {//语句解析
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type == RETURNTK) {//返回语句
        callBackStreampos(sp,oriLine);
        returnStatementParser();
    } else if (type == SCANFTK) {//读语句
        callBackStreampos(sp,oriLine);
        readStatementParser();
    } else if (type == PRINTFTK) {//写语句
        callBackStreampos(sp,oriLine);
        printStatementParser();
    } else if (type == IFTK) {//条件语句
        callBackStreampos(sp,oriLine);
        conditionStatementParser();
        return;
    } else if (type == WHILETK || type == FORTK || type == DOTK) {//循环语句
        callBackStreampos(sp,oriLine);
        recycleStatementParser();
        return;
    }  else if (type == LBRACE) {//语句列
        statementColumnParser();
        type = wordParser(ans);
        if (type != RBRACE) error();
        return;
    } else if (type == IDENFR) {
        string ans2;
        tokenType type2 = wordParser(ans2);
        callBackStreampos(sp,oriLine);
        if (type2 == LPARENT) {//函数调用
            if (!entryExist(ans)) {//名字未定义
                printf("using the unexisting variable %s\n",ans.c_str());
                error(c,linenum);
            }
            if (isReturnFunc(ans)) {//如果是返回函数
                returnFuncCallParser();
            } else {
                voidFuncCallParser();
            }
        } else {//赋值语句
            assignStatementParser();
        }
    } else if (type == SEMICN) {//空语句
        callBackStreampos(sp,oriLine);
    } else error();
    sp = getStreampos(oriLine);
    type = wordParser(ans);
    if (type != SEMICN || linenum > oriLine) {
        error(k,oriLine);
        callBackStreampos(sp,oriLine);
        linenum = oriLine;
    }
}

void readStatementParser() { //scanf语句
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type != SCANFTK) error();
    type = wordParser(ans);
    if (type != LPARENT) error();
    sp = getStreampos(oriLine);
    while (type != RPARENT && type != SEMICN) {
        type = wordParser(ans);
        if (type != IDENFR) error();
        if (!entryExist(ans)) {
            printf("using the unexisting variable %s\n",ans.c_str());
            error(c,linenum);
        }
        sp = getStreampos(oriLine);
        type = wordParser(ans);
    }
    if (type != RPARENT) {
        callBackStreampos(sp,oriLine);
        error(l,linenum);
    }
}

void printStatementParser() {
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type != PRINTFTK) error();
    type = wordParser(ans);
    if (type != LPARENT) error();
    if (cin.peek() == '\"') {
        stringParser();
        sp = getStreampos(oriLine);
        type = wordParser(ans);
        if (type == COMMA) {
            expressionParser();
        } else {
            callBackStreampos(sp,oriLine);
        }
    } else {
        expressionParser();
    }
    sp = getStreampos(oriLine);
    type = wordParser(ans);
    if (type != RPARENT) {
        error(l,oriLine);
        callBackStreampos(sp,oriLine);
    }
}

void returnStatementParser() {
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type != RETURNTK) error();
    sp = getStreampos(oriLine);
    ExprType exprType = None;
    type = wordParser(ans);
    if (type == LPARENT) {
        exprType = expressionParser();
        sp = getStreampos(oriLine);
        type = wordParser(ans);
        if (type != RPARENT) {
            callBackStreampos(sp,oriLine);
            error(l,linenum);
        }
    } else {
        callBackStreampos(sp,oriLine);
    }
    addFunctionReturn(exprType);
}
//赋值语句
void assignStatementParser() {
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type != IDENFR) error();
    if (!isExistVar(ans)) {
        printf("using the unexisting variable %s\n",ans.c_str());
        error(c,linenum);
    }//赋值语句左边的变量是未定义的变量
    if (isConstVar(ans)) error(j,linenum);//赋值左端的变量是常量不可改变
    type = wordParser(ans);
    if (type != LBRACK && type != ASSIGN) error();
    if (type == LBRACK) {//等式左边是数组
        ExprType expType = expressionParser();
        if (expType == CharExp) error(i,linenum);//数组的下表是字符类型
        sp = getStreampos(oriLine);
        type = wordParser(ans);
        if (type != RBRACK) {
            error(m,linenum);
            callBackStreampos(sp,oriLine);
        }
        type = wordParser(ans);
        if (type != ASSIGN) error();
    }
    expressionParser();
}
//条件语句
void conditionStatementParser() {
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type != IFTK) error();
    type = wordParser(ans);
    if (type != LPARENT) error();
    conditionParser();
    sp = getStreampos(oriLine);
    type = wordParser(ans);
    if (type != RPARENT) {
        error(l,oriLine);
        callBackStreampos(sp,oriLine);
    }
    statementParser();
    sp = getStreampos(oriLine);
    type = wordParser(ans);
    if (type == ELSETK) {
        statementParser();
    }
    else callBackStreampos(sp,oriLine);
}

void conditionParser() {
    ExprType expType0 = expressionParser();
    ExprType expType1 = IntExp;
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type == RPARENT || type == SEMICN || type == LBRACE || (oriLine > linenum) || type == IDENFR || type == ASSIGN) {//碰到右括号分号或者在没有右括号的情况下碰到左大括号或者直接解析到了下一行,也有可能在for语句中条件之后缺少分号
        callBackStreampos(sp,oriLine);
        goto end;
    }
    if (type != LSS && type != GRE && type != GEQ && type != EQL && type != LEQ && type != NEQ) error();
    expType1 = expressionParser();
    end:
        if (expType0 == CharExp || expType1 == CharExp) error(f,linenum);//条件中任意一个表达式返回值是char报错
}
//循环语句
//for语句中的分号缺失
void whileRecycleStatementParser() {
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type != WHILETK) error();
    type = wordParser(ans);
    if (type != LPARENT) error();
    conditionParser();
    sp = getStreampos(oriLine);
    type = wordParser(ans);
    if (type != RPARENT) {
        error(l,oriLine);
        callBackStreampos(sp,oriLine);
    }
    statementParser();
}

void doWhileRecycleStatementParser() {
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type != DOTK) error();
    statementParser();
    type = wordParser(ans);
    if (type != WHILETK) {
        error(n,linenum);
    }//n类错误，do后没有匹配的while语句
    else {
        type = wordParser(ans);
    }
    if (type != LPARENT) error();
    conditionParser();
    sp = getStreampos(oriLine);
    type = wordParser(ans);
    if (type != RPARENT || linenum > oriLine) {//解析下一个符号的时候换行了说明没有右括号
        error(f,oriLine);//f类错误，应该有右括号
        callBackStreampos(sp,oriLine);
    }
}

void forRecycleStatementParser() {
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type != FORTK) error();
    type = wordParser(ans);
    if (type != LPARENT) error();
    type = wordParser(ans);
    if (type != IDENFR) error();
    if (!isExistVar(ans)) {
        printf("using the unexisting variable %s\n",ans.c_str());
        error(c,linenum);
    }//赋值语句左边的变量是未定义的变量
    if (isConstVar(ans)) error(j,linenum);//赋值左端的变量是常量不可改变
    type = wordParser(ans);
    if (type != ASSIGN) error();
    expressionParser();
    sp = getStreampos(oriLine);
    type = wordParser(ans);
    if (type != SEMICN) {
        error(k,linenum);//缺少分号
        if (type == LSS || type == LEQ || type == GEQ || type == GRE || type == EQL || type == NEQ) {//解析到下一个字符是比较运算符，说明已经缺少分号并且条件的左边已经被解析完成
            while (!isSemi(cin.peek())) cin.get();//吞到下一个分号位置，出现类似for(i = 0 (缺失分号;)- i  < 1;i = i + 1)
            goto nextSemi;//直接进行步长的解析
        } else {
            callBackStreampos(sp,oriLine);//说明当前只是缺少分号其他成分还没有缺少,出现类似for(i = 0 (缺失分号;)<条件>;i = i + 1)
        }
    }
    sp = getStreampos(oriLine);//解析条件之前的文件指针位置
    conditionParser();
    nextSemi://下面是步长的解析
    streampos spp = cin.tellg();
    long long oriLinee = linenum;
    type = wordParser(ans);
    if (type != SEMICN) {
        error(k,linenum);
        if (type == ASSIGN) {//出现类似for(i = 0 (缺失分号;)- i;i = i + 1)
            callBackStreampos(sp,oriLine);
        } else {
            callBackStreampos(spp,oriLinee);// for (i = 0; <条件> i = i + step)
        }
    }
    type = wordParser(ans);
    if (type != IDENFR) error();
    if (!isExistVar(ans)) {
        printf("using the unexisting variable %s\n",ans.c_str());
        error(c,linenum);
    }//赋值语句左边的变量是未定义的变量
    if (isConstVar(ans)) error(j,linenum);//赋值左端的变量是常量不可改变
    type = wordParser(ans);
    if (type != ASSIGN) error();
    type = wordParser(ans);
    if (type != IDENFR) error();
    if (!isExistVar(ans)) {
        printf("using the unexisting variable %s\n",ans.c_str());
        error(c,linenum);
    }//赋值语句左边的变量是未定义的变量
    if (isConstVar(ans)) error(j,linenum);//赋值左端的变量是常量不可改变
    type = wordParser(ans);
    if (type != PLUS && type != MINU) error();
    stepStatementParser();
    sp = getStreampos(oriLine);
    type = wordParser(ans);
    if (type != RPARENT) {
        error(l,oriLine);
        callBackStreampos(sp,oriLine);
    }
    statementParser();
}

void recycleStatementParser() {
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    callBackStreampos(sp,oriLine);
    if (type == WHILETK) {//while语句
        whileRecycleStatementParser();
    } else if (type == DOTK) {//do-while语句
        doWhileRecycleStatementParser();
    } else if (type == FORTK) {//for循环语句
        forRecycleStatementParser();
    } else error();
}

void stepStatementParser() {
    unsignedIntParser();
}

void mainFuncParser() {
    string ans;
    tokenType type = wordParser(ans);
    if (type != VOIDTK) error();
    type = wordParser(ans);
    if (type != MAINTK) error();
    type = wordParser(ans);
    if (type != LPARENT) error();
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    type = wordParser(ans);
    if (type != RPARENT) {
        error(l,linenum);
        callBackStreampos(sp,oriLine);
    }
    type = wordParser(ans);
    if (type != LBRACE) error();
    string mainName = "main";
    addFunction(VOIDTK,mainName);//添加main函数到符号表
    compoundStatementParser(mainName);
    type = wordParser(ans);
    if (type != RBRACE) error();
}

void stringParser() {
    string ans;
    tokenType  type = wordParser(ans);
    if (type != STRCON) error();
}

void integerParser() {
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type == PLUS || type == MINU) {
        unsignedIntParser();
    } else {
        callBackStreampos(sp,oriLine);
        unsignedIntParser();
    }
}

ExprType expressionParser() {//表达式
    ExprType ansType = CharExp;//默认的表达式的返回值是字符类型的
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if ((type == PLUS || type == MINU)) {//只要有加减运算就是int型
        ansType = IntExp;
    } else {
        callBackStreampos(sp,oriLine);
    }
    ExprType itemType = itemParser();
    if (itemType == IntExp) {//只要项返回值是int就是int型
        ansType = IntExp;
    }
    sp = getStreampos(oriLine);
    type = wordParser(ans);
    while (type == PLUS || type == MINU) {//只要有多个项就是int型
        ansType = IntExp;
        itemParser();
        sp = getStreampos(oriLine);
        type = wordParser(ans);
    }
    callBackStreampos(sp,oriLine);
    return ansType;
}

ExprType itemParser() {//项
    ExprType ansType = CharExp;
    ExprType factorType = factorParser();
    if (factorType == IntExp) {//只要有因子返回值是int型
        ansType = IntExp;
    }
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    while (type == MULT || type == DIV) {//只要有多个因子就是int型
        ansType = IntExp;
        factorParser();
        sp = getStreampos(oriLine);
        type = wordParser(ans);
    }
    callBackStreampos(sp,oriLine);
    return ansType;
}

ExprType factorParser() {
    ExprType ansType = CharExp;
    string ans;
    streampos sp = cin.tellg();
    long long oriLine = linenum;
    tokenType type = wordParser(ans);
    if (type == IDENFR) {
        if (!entryExist(ans)) {
            printf("using the unexisting variable %s\n",ans.c_str());
            error(c,linenum);
        }//名字未定义
        string ans2;
        streampos sp2 = cin.tellg();
        long long oriLine2 = linenum;
        tokenType type2 = wordParser(ans2);
        if (type2 == LPARENT && isReturnFunc(ans)) {//有返回值函数调用的因子
            callBackStreampos(sp,oriLine);
            if (isIntReturnFunc(ans)) ansType = IntExp;//返回函数是int型
            returnFuncCallParser();//有返回值的函数调用
        } else {
            if (type2 == LBRACK) {//数组
                if (isIntArray(ans)) ansType = IntExp;//只要有数组是int类型
                ExprType indexType = expressionParser();
                if (indexType == CharExp) error(i,linenum);//数组下表是字符类型
                streampos spp = cin.tellg();
                type = wordParser(ans);
                if (type != RBRACK) {
                    error(m,linenum);
                    cin.seekg(spp);
                }
            } else {
                if (isIntVar(ans)) ansType = IntExp;//只要有变量是int类型
                callBackStreampos(sp2,oriLine2);
            }
        }
    } else if (type == LPARENT) {//只要是有嵌套表达式
        ansType = IntExp;
        expressionParser();
        streampos spp = cin.tellg();
        long long oriLinee = linenum;
        type = wordParser(ans);
        if (type != RPARENT) {
            error(l,linenum);
            cin.seekg(spp);
            linenum = oriLinee;
        }
    } else if (type == PLUS || type == MINU || type == INTCON) {//只要存在数字
        ansType = IntExp;
        callBackStreampos(sp,oriLine);
        integerParser();
    }  else if (type == CHARCON) {
        return ansType;
    }
    return ansType;
}

void returnFuncCallParser() {
    funcCallParser();
}

void voidFuncCallParser() {
    funcCallParser();
}

void funcCallParser() {
    string ans;
    string functionName;
    tokenType type = wordParser(functionName);
    if (type != IDENFR) error();
    if (!entryExist(functionName)) {
        printf("using the unexisting variable %s\n",functionName.c_str());
        error(c,linenum);
    }
    type = wordParser(ans);
    if (type != LPARENT) error();
    funcCallValueListParser(functionName);//函数调用的参数表
    streampos sp = cin.tellg();
    type = wordParser(ans);
    if (type != RPARENT) {
        error(l,linenum);
        cin.seekg(sp);
    }
}

void funcCallValueListParser(string& functionName) {
    streampos sp = cin.tellg();
    string ans;
    tokenType type = wordParser(ans);
    vector<ExprType> valueList;
    while (type != RPARENT && type != SEMICN) {//缺少右括号的情况
        cin.seekg(sp);
        valueList.push_back(expressionParser());
        sp = cin.tellg();
        type = wordParser(ans);
        if (type == COMMA) {
            sp = cin.tellg();
        }
    }
    matchFunctionParameter(functionName,valueList);//检查函数参数表
    cin.seekg(sp);
}

void error() {
    printf("error\n");
}
