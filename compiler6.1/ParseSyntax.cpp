#include<cstdio>
#include<unordered_map>
#include "Type.h"
#include "Parse.h"
#include "Token.h"
#include "SyntaxTree.h"

using namespace std;
void SyntaxParser::startProgramParser() {
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == CONSTTK) {//处理程序中的全局常量说明
        callBackStreamPos(ps);
        constDeclarationParser(nullptr);
        flushStreamPos(ps);
    }
    callBackStreamPos(ps);
    type = getNextSym(ans);
    if (type == INTTK || type == CHARTK) { //处理程序的全局变量说明
        getNextSym(ans);
        type = getNextSym(ans);
        if (type != LPARENT) {//向后读两个单词，遇到左括号说明是函数定义
            callBackStreamPos(ps);
            variableDeclarationParser(nullptr);
            flushStreamPos(ps);
        }
    }
    this->isglobal = false;
    callBackStreamPos(ps);
    type = getNextSym(ans);
    bool reachMain = false;
    while ((type == INTTK || type == CHARTK || type == VOIDTK) && !reachMain) { //处理程序中的函数说明
        if (type == INTTK || type == CHARTK) {
            callBackStreamPos(ps);
            returnFuncDefinitionParser(nullptr);
            flushStreamPos(ps);
        } else {
            type = getNextSym(ans);
            if (type == MAINTK) {
                reachMain = true;
            } else {
                callBackStreamPos(ps);
                voidFuncDefinitionParser(nullptr);
                flushStreamPos(ps);
            }
        }
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
    mainFuncParser(nullptr);
}

void SyntaxParser::constDeclarationParser(SyntaxNode *parentNode) {//解析常量声明
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    while (type == CONSTTK) {
        constDefinitionParser(nullptr);//解析常量的定义
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != SEMICN) {
            if (getLineNumber() > ps->getLineNumber()) {//如果是缺少分号而解析到了下一行
                callBackStreamPos(ps);
            }
            errorHandler(k);
        }
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
}

void SyntaxParser::constDefinitionParser(SyntaxNode *parentNode) {//解析常量定义
    string ans;
    string varName;
    int varValue  = -1;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    TokenType identifierType = type;
    while (type != SEMICN) {
        flushStreamPos(ps);
        type = getNextSym(ans);
		if (getLineNumber() > ps->getLineNumber()) {//没有分号已经解析到下一行
            callBackStreamPos(ps);
			break;
		}
        if (type != IDENFR) error();
        addVariableEntry(identifierType, ans, true);//添加常量的符号表项
        varName = ans;//常量名
        type = getNextSym(ans);
        if (type != ASSIGN) error();
        if (identifierType == INTTK) {
            type = getNextSym(ans);
            string valueStr = ans;
            if (type == PLUS || type == MINU) {
                type = getNextSym(ans);
                valueStr += ans;
            }
            if (type != INTCON) errorHandler(o);//常量定义中的int型变量赋值不是整型
            varValue = stoi(valueStr);//获取常量的数值
        } else  if (identifierType == CHARTK) {
            type = getNextSym(ans);
            if (type != CHARCON) errorHandler(o);
            varValue = int(ans[0]);//获取常量的数值
        } else error();
        constDefIR(varName, varValue);//生成常量声明的中间代码
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (getLineNumber() > ps->getLineNumber()) {//没有分号已经解析到下一行
            callBackStreamPos(ps);
            break;
        }
    }
    callBackStreamPos(ps);
}

void SyntaxParser::variableDeclarationParser(SyntaxNode *parentNode) {//解析变量声明
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    while (type == INTTK || type == CHARTK) {
        getNextSym(ans);
        type = getNextSym(ans);
        if (type == LPARENT) {//左括号说明碰到的是函数声明
            break;
        } else {
            callBackStreamPos(ps);
            variableDefinitionParser(nullptr);
            flushStreamPos(ps);
            type = getNextSym(ans);
            if (type != SEMICN) { //没有分号解析到下一行，需要回退
                callBackStreamPos(ps);
                errorHandler(k);
            }
            flushStreamPos(ps);
        }
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
}

void SyntaxParser::variableDefinitionParser(SyntaxNode *parentNode) {//解析变量定义
    string ans;
    TokenType type;
    auto* ps = getCurPosition();
    type = getNextSym(ans);//提取类型标识符
    TokenType identifierType = type;
    string varName;
    unsigned int size;//默认变量是一维的变量
    while (type != SEMICN) {
        size = 1;
        type = getNextSym(ans);
        flushStreamPos(ps);
		if (getLineNumber() > ps->getLineNumber()) {//处理分号被替换的情况
            callBackStreamPos(ps);
			break;
		}
        if (type != IDENFR) error();
        varName = ans;//存储变量的标识符
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != LBRACK) {
            addVariableEntry(identifierType,varName,false);//添加常量符号表项
            variableDefIR(varName, size);//一维变量的声明
            continue;
        }
        size = unsignedIntParser();
        addArrayEntry(identifierType,varName,size);//添加数组的符号表项
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != RBRACK) {//缺少右中括号
            callBackStreamPos(ps);
            errorHandler(m);
        }
        variableDefIR(varName, size);//数组的声明
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (getLineNumber() > ps->getLineNumber()) {//没有分号，解析到下一行，需要回退
            callBackStreamPos(ps);
            break;
        }
    }
    callBackStreamPos(ps);
}

unsigned int SyntaxParser::unsignedIntParser() {  // 无符号常数的解析,这是一个叶子函数
    string ans;
    TokenType type = getNextSym(ans);
    if (type != INTCON) error();
    if (ans.size() > 1 && ans[0] == '0') error();
    return stoi(ans);
}

void SyntaxParser::returnFuncDefinitionParser(SyntaxNode *parentNode) {
    string ans;
    TokenType type = getNextSym(ans);
    string functionName;
    if (type != INTTK && type != CHARTK) error();
    TokenType type1 = getNextSym(functionName);
    if (type1 != IDENFR) error();
    functionDefIR(functionName);//中间代码函数标签
    addFunction(type,functionName);//添加有返回值函数符号表项
    funcStatementParser(functionName, nullptr);
}

void SyntaxParser::voidFuncDefinitionParser(SyntaxNode *parentNode) {
    string ans;
    TokenType type = getNextSym(ans);
    string functionName;
    if (type != VOIDTK) error();
    type = getNextSym(functionName);
    if (type != IDENFR) error();
    functionDefIR(functionName);//中间代码函数标签
    addFunction(VOIDTK,functionName);//添加无返回值符号表项
    funcStatementParser(functionName, nullptr);
    voidReturnIR();//无论无返回值函数是否有返回语句都添加一个返回中间代码
}

void SyntaxParser::funcStatementParser(string &functionName, SyntaxNode *parentNode) {//主要是解析函数的参数表和执行语句
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != LPARENT) error();
    funcParameterListParser(nullptr);//解析函数的参数表
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandler(l);//参数表缺少右括号
    }
    type = getNextSym(ans);
    if (type != LBRACE) error();
    compoundStatementParser(functionName, nullptr);
    type = getNextSym(ans);
    if (type != RBRACE) error();
}

void SyntaxParser::funcParameterListParser(SyntaxNode *parentNode) {//解析函数参数表
    string ans;
    auto* ps = getCurPosition();
    TokenType varType = getNextSym(ans);
    TokenType identifierType = varType;//记录函数标识符的类型
    if (varType == RPARENT || varType == LBRACE) {//没有右括号的时候终止需要通过达到大括号来判断
        callBackStreamPos(ps);
    } else {
        for (;;) {
            if (identifierType != INTTK && identifierType != CHARTK) error();
            varType = getNextSym(ans);
            if (varType != IDENFR) error();
            flushStreamPos(ps);
            functionDefParaIR(ans);//参数的中间代码
            addFunctionParameter(identifierType,ans);//添加参数表内容到函数的符号表,ans就是参数的名
            varType = getNextSym(ans);
            if (varType == COMMA) {
                identifierType = getNextSym(ans);
            } else if (varType == RPARENT || varType == LBRACE){ //没有右括号的时候终止需要通过达到大括号来判断
                break;
            } else {
                error();
                break;
            }
        }
        callBackStreamPos(ps);
    }
}

void SyntaxParser::compoundStatementParser(string &functionName, SyntaxNode *parentNode) {//复合语句的解析程序
    string ans;auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == CONSTTK) {//常量说明
        callBackStreamPos(ps);
        constDeclarationParser(nullptr);
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    if (type == INTTK || type == CHARTK) {//变量说明
        callBackStreamPos(ps);
        variableDeclarationParser(nullptr);
        flushStreamPos(ps);
    }
    callBackStreamPos(ps);
    statementColumnParser(nullptr);
    checkReturnFunctionReturn();
}

void SyntaxParser::statementColumnParser(SyntaxNode *parentNode) { // 语句列
    string ans;auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    while (type != RBRACE) {
        callBackStreamPos(ps);
        statementParser(nullptr);
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
}

void SyntaxParser::statementParser(SyntaxNode *parentNode) {//语句解析
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == RETURNTK) {//返回语句
        callBackStreamPos(ps);
        returnStatementParser(nullptr);
    } else if (type == SCANFTK) {//读语句
        callBackStreamPos(ps);
        readStatementParser(nullptr);
    } else if (type == PRINTFTK) {//写语句
        callBackStreamPos(ps);
        printStatementParser(nullptr);
    } else if (type == IFTK) {//条件语句
        callBackStreamPos(ps);
        conditionStatementParser(nullptr);
        return;
    } else if (type == WHILETK || type == FORTK || type == DOTK) {//循环语句
        callBackStreamPos(ps);
        recycleStatementParser(nullptr);
        return;
    }  else if (type == LBRACE) {//语句列
        statementColumnParser(nullptr);
        type = getNextSym(ans);
        if (type != RBRACE) error();
        return;
    } else if (type == IDENFR) {
        string ans2;
        TokenType type2 = getNextSym(ans2);
        callBackStreamPos(ps);
        if (type2 == LPARENT) {//函数调用
            checkEntryExist(ans);//检查标识符是否定义
            if (isReturnFunc(ans)) {//如果是返回函数
                string ansSymbol;
                returnFuncCallParser(ansSymbol, nullptr);
            } else {
                voidFuncCallParser(nullptr);
            }
        } else {//赋值语句
            assignStatementParser(nullptr);
        }
    } else if (type == SEMICN) {//空语句
        callBackStreamPos(ps);
    } else error();
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != SEMICN && getLineNumber() > ps->getLineNumber()) {
        callBackStreamPos(ps);
        errorHandler(k);
    }
}

void SyntaxParser::readStatementParser(SyntaxNode *parentNode) { //scanf语句
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != SCANFTK) error();
    type = getNextSym(ans);
    if (type != LPARENT) error();
    flushStreamPos(ps);
    while (type != RPARENT && type != SEMICN) {
        type = getNextSym(ans);
        if (type != IDENFR) error();
        checkEntryExist(ans);
        checkPlantVariable(ans);
        readIR(ans);//读语句的中间代码
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandler(l);
    }
}

void SyntaxParser::printStatementParser(SyntaxNode *parentNode) {
    string ans;auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != PRINTFTK) error();
    type = getNextSym(ans);
    if (type != LPARENT) error();
    string ansSymbol;
    if (peek() == '\"') {//说明即将要输出的是字符串
        string printStr = stringParser();
        string strName = applyStringName(printStr);
        printStrIR(strName);//输出字符串中间代码
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type == COMMA) {
            ExprType exprType = expressionParser(ansSymbol, nullptr);
            printExprIR(ansSymbol, exprType);
        } else {
            callBackStreamPos(ps);
        }
    } else {
        ExprType exprType = expressionParser(ansSymbol, nullptr);
        printExprIR(ansSymbol, exprType);
    }
    printNewlineIR();
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandler(l);
    }
}

//完成
void SyntaxParser::returnStatementParser(SyntaxNode *parentNode) {
    string ans;auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != RETURNTK) error();
    flushStreamPos(ps);
    ExprType exprType = None;
    type = getNextSym(ans);
    string ansSymbol;
    if (type == LPARENT) {
        exprType = expressionParser(ansSymbol, nullptr);
        valueReturnIR(ansSymbol);//返回值函数的返回语句
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != RPARENT) {
            callBackStreamPos(ps);
            errorHandler(l);
        }
    } else {
        callBackStreamPos(ps);
        voidReturnIR();
    }
    addFunctionReturn(exprType);
}

//完成
//赋值语句
void SyntaxParser::assignStatementParser(SyntaxNode *parentNode) {
    string ans;
    string left;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != IDENFR) error();
    left += ans;
    checkVariableExist(ans);//赋值语句左边的变量是未定义的变量
    checkAssignable(ans);//赋值左端的变量是常量不可改变
    type = getNextSym(ans);
    if (type != LBRACK && type != ASSIGN) error();
    if (type == LBRACK) {//等式左边是数组
        string indexSymbol;
        ExprType expType = expressionParser(indexSymbol, nullptr);
        left = left + "[" + indexSymbol + "]";
        if (expType == CharExp) errorHandler(i);//数组的下表是字符类型
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != RBRACK) {
            callBackStreamPos(ps);
            errorHandler(m);
        }
        type = getNextSym(ans);
        if (type != ASSIGN) error();
    }
    string right;
    expressionParser(right, nullptr);
    assignIR(left,right);
}

//完成
//条件语句
void SyntaxParser::conditionStatementParser(SyntaxNode *parentNode) {
    string ans;
    string oriCodeLabel = applyCodeBlkLabel();
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != IFTK) error();
    type = getNextSym(ans);
    if (type != LPARENT) error();
    string condition;
    conditionParser(condition, nullptr);
    reverseCondition(condition);//条件的反
    conditionalJumpIR(oriCodeLabel, condition);//if condition goto label
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandler(l);
    }
    statementParser(nullptr);//语句
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type == ELSETK) {
        string codeLabel = applyCodeBlkLabel();
        unconditionalJumpIR(codeLabel);
        setDownLabelIR(oriCodeLabel);
        statementParser(nullptr);
        setDownLabelIR(codeLabel);
    } else {
        setDownLabelIR(oriCodeLabel);
        callBackStreamPos(ps);
    }
}

void SyntaxParser::conditionParser(string &condition, SyntaxNode *parentNode) {
    string left;
    string cmpOp;
    string right;
    ExprType exprType0 = expressionParser(left, nullptr);
    ExprType exprType1 = IntExp;
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == RPARENT || type == SEMICN || type == LBRACE || (ps->getLineNumber() > getLineNumber()) || type == IDENFR || type == ASSIGN) {//碰到右括号分号或者在没有右括号的情况下碰到左大括号或者直接解析到了下一行,也有可能在for语句中条件之后缺少分号
        callBackStreamPos(ps);
        goto end;
    }
    if (type != LSS && type != GRE && type != GEQ && type != EQL && type != LEQ && type != NEQ) error();
    cmpOp = ans;
    exprType1 = expressionParser(right, nullptr);
    end:
        if (exprType0 == CharExp || exprType1 == CharExp) errorHandler(f);//条件中任意一个表达式返回值是char报错
        if (cmpOp.empty()) {
            condition = left + " != 0";
            return;
        }
        condition = left + " " + cmpOp + " " + right;

}

void SyntaxParser::reverseCondition(string &condition) {
    stringstream ss(condition);
    string left;
    string right;
    string cmpOp;
    getline(ss,left,' ');
    getline(ss,cmpOp,' ');
    if (cmpOp.empty()) {
        condition = left + " == 0";
        return;
    } else {
        getline(ss,right,' ');
        if (cmpOp == "<") {
            cmpOp = ">=";
        } else {
            if (cmpOp == ">") {
                cmpOp = "<=";
            } else {
                if (cmpOp == "<=") {
                    cmpOp = ">";
                } else {
                    if (cmpOp == ">=") {
                        cmpOp = "<";
                    } else {
                        if (cmpOp == "==") {
                            cmpOp = "!=";
                        } else {
                            cmpOp = "==";
                        }
                    }
                }
            }
        }
        condition = left + " " + cmpOp + " " + right;
    }
}

//完成
//循环语句
void SyntaxParser::whileRecycleStatementParser(SyntaxNode *parentNode) {
    string ans;
    string condition;
    string whileLabel = applyWhileCycleLabel();
    string codeLabel = applyCodeBlkLabel();
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != WHILETK) error();
    type = getNextSym(ans);
    if (type != LPARENT) error();
    setDownLabelIR(whileLabel);
    conditionParser(condition, nullptr);
    reverseCondition(condition);
    conditionalJumpIR(codeLabel, condition);
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandler(l);
    }
    statementParser(nullptr);
    unconditionalJumpIR(whileLabel);
    setDownLabelIR(codeLabel);
}

//
void SyntaxParser::doWhileRecycleStatementParser(SyntaxNode *parentNode) {
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != DOTK) error();
    string doLabel = applyDoCycleLabel();
    setDownLabelIR(doLabel);
    statementParser(nullptr);
    type = getNextSym(ans);
    if (type != WHILETK) {
        errorHandler(n);
    }//n类错误，do后没有匹配的while语句
    else {
        type = getNextSym(ans);
    }
    if (type != LPARENT) error();
    string condition;
    conditionParser(condition, nullptr);
    flushStreamPos(ps);//记录当前的分析完条件的位置
    conditionalJumpIR(doLabel, condition);
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != RPARENT || getLineNumber() > ps->getLineNumber()) {//解析下一个符号的时候换行了说明没有右括号
        callBackStreamPos(ps);
        errorHandler(l);//l类错误，应该有右括号
    }
}

void SyntaxParser::forRecycleStatementParser(SyntaxNode *parentNode) {
    string ans;
    string forCycleLabel = applyForCycleLabel();
    string codeLabel = applyCodeBlkLabel();
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != FORTK) error();
    type = getNextSym(ans);
    if (type != LPARENT) error();
    type = getNextSym(ans);
    string left = ans;
    if (type != IDENFR) error();
    checkVariableExist(ans);//赋值语句左边的变量是未定义的变量
    checkAssignable(ans);//赋值左端的变量是常量不可改变
    type = getNextSym(ans);
    if (type != ASSIGN) error();
    string ansSymbol;
    expressionParser(ansSymbol, nullptr);
    assignIR(left,ansSymbol);//初始化语句
    setDownLabelIR(forCycleLabel);
    flushStreamPos(ps);
    type = getNextSym(ans);
    string condition;
    if (type != SEMICN) {
        errorHandler(k);//缺少分号
        if (type == LSS || type == LEQ || type == GEQ || type == GRE || type == EQL || type == NEQ) {//解析到下一个字符是比较运算符，说明已经缺少分号并且条件的左边已经被解析完成
            while (!isSemi(peek())) get();//吞到下一个分号位置，出现类似for(i = 0 (缺失分号;)- i  < 1;i = i + 1)
            goto nextSemi;//直接进行步长的解析
        } else {
            callBackStreamPos(ps);//说明当前只是缺少分号其他成分还没有缺少,出现类似for(i = 0 (缺失分号;)<条件>;i = i + 1)
        }
    }
    flushStreamPos(ps);//解析条件之前的文件指针位置
    conditionParser(condition, nullptr);
    reverseCondition(condition);
    conditionalJumpIR(codeLabel, condition);
    nextSemi://下面是步长的解析
    auto* pss = getCurPosition();
    type = getNextSym(ans);
    if (type != SEMICN) {
        errorHandler(k);
        if (type == ASSIGN) {//出现类似for(i = 0 (缺失分号;)- i;i = i + 1)
            callBackStreamPos(ps);
        } else {
            callBackStreamPos(pss);// for (i = 0; <条件> i = i + step)
        }
    }
    type = getNextSym(ans);
    if (type != IDENFR) error();
    checkVariableExist(ans);//赋值语句左边的变量是未定义的变量
    checkAssignable(ans);//赋值左端的变量是常量不可改变
    type = getNextSym(ans);
    if (type != ASSIGN) error();
    type = getNextSym(ans);
    if (type != IDENFR) error();
    string opnum = ans;
    checkVariableExist(ans);//赋值语句右边的变量是未定义的变量
    checkAssignable(ans);//赋值右端的变量是常量不可改变
    type = getNextSym(ans);
    if (type != PLUS && type != MINU) error();
    string op = ans;
    string step = to_string(stepStatementParser());
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandler(l);
    }
    statementParser(nullptr);
    operationIR(op,opnum,step,opnum);
    unconditionalJumpIR(forCycleLabel);
    setDownLabelIR(codeLabel);
}

//完成
void SyntaxParser::recycleStatementParser(SyntaxNode *parentNode) {
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    callBackStreamPos(ps);
    if (type == WHILETK) {//while语句
        whileRecycleStatementParser(nullptr);
    } else if (type == DOTK) {//do-while语句
        doWhileRecycleStatementParser(nullptr);
    } else if (type == FORTK) {//for循环语句
        forRecycleStatementParser(nullptr);
    } else error();
}

//完成
unsigned int SyntaxParser::stepStatementParser() {
    return unsignedIntParser();
}

//完成
void SyntaxParser::mainFuncParser(SyntaxNode *parentNode) {
    string ans;
    TokenType type = getNextSym(ans);
    if (type != VOIDTK) error();
    type = getNextSym(ans);
    if (type != MAINTK) error();
    functionDefIR(ans);//main函数中间代码标签
    type = getNextSym(ans);
    if (type != LPARENT) error();
    auto* ps = getCurPosition();
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandler(l);
    }
    type = getNextSym(ans);
    if (type != LBRACE) error();
    string mainName = "main";
    addFunction(VOIDTK,mainName);//添加main函数到符号表
    compoundStatementParser(mainName, nullptr);
    type = getNextSym(ans);
    if (type != RBRACE) error();
    endProgram();
}

//完成
string SyntaxParser::stringParser() { // 字符串的解析函数
    string ans;
    TokenType  type = getNextSym(ans);
    if (type != STRCON) error();
    return ans;
}

//完成
int SyntaxParser::integerParser() { //整数的解析函数
    string ans;
    string intstr;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == PLUS || type == MINU) {
        intstr += ans;
        intstr += to_string(unsignedIntParser());
    } else {
        callBackStreamPos(ps);
        intstr += to_string(unsignedIntParser());
    }
    return stoi(intstr);
}

//完成
ExprType SyntaxParser::expressionParser(string &ansSymbol, SyntaxNode *parentNode) {//表达式
    ExprType ansType = CharExp;//默认的表达式的返回值是字符类型的
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if ((type == PLUS || type == MINU)) {//只要有加减运算就是int型
        ansType = IntExp;
    } else {
        callBackStreamPos(ps);
    }
    ansSymbol = applyVariableName();
    string itemSymbol;
    string blankStr;
    ExprType itemType = itemParser(itemSymbol, nullptr);
    if ((type == PLUS || type == MINU)) {
        operationIR(ans, blankStr, itemSymbol,ansSymbol);
    } else {
        assignIR(ansSymbol, itemSymbol);
    }
    if (itemType == IntExp) {//只要项返回值是int就是int型
        ansType = IntExp;
    }
    flushStreamPos(ps);
    type = getNextSym(ans);
    while (type == PLUS || type == MINU) {//只要有多个项就是int型
        ansType = IntExp;
        itemParser(itemSymbol, nullptr);
        operationIR(ans, ansSymbol, itemSymbol,ansSymbol);
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
    return ansType;
}

//完成
ExprType SyntaxParser::itemParser(string &ansSymbol, SyntaxNode *parentNode) {//项
    ExprType ansType = CharExp;
    ansSymbol = applyVariableName();
    string factorSymbol;
    ExprType factorType = factorParser(factorSymbol, nullptr);
    if (factorType == IntExp) {//只要有因子返回值是int型
        ansType = IntExp;
    }
    assignIR(ansSymbol,factorSymbol);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    while (type == MULT || type == DIV) {//只要有多个因子就是int型
        ansType = IntExp;
        factorParser(factorSymbol, nullptr);
        operationIR(ans, ansSymbol, factorSymbol,ansSymbol);
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
    return ansType;
}

//完成
ExprType SyntaxParser::factorParser(string &ansSymbol, SyntaxNode *parentNode) {//因子解析
    ExprType ansType = CharExp;
    string ans;
    ansSymbol.clear();
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == IDENFR) {
        checkEntryExist(ans);//检查名字是否定义
        string ans2;
        auto* ps2 = getCurPosition();
        TokenType type2 = getNextSym(ans2);
        if (type2 == LPARENT && isReturnFunc(ans)) {//有返回值函数调用的因子
            callBackStreamPos(ps);
            if (isIntReturnFunc(ans)) ansType = IntExp;//返回函数是int型
            returnFuncCallParser(ansSymbol, nullptr);//有返回值的函数调用
        } else {
            if (type2 == LBRACK) {//数组
                if (isIntArray(ans)) ansType = IntExp;//只要有数组是int类型
                string arrayIndex;
                ExprType indexType = expressionParser(arrayIndex, nullptr);
                ansSymbol = applyVariableName();
                string op = "[]";
                operationIR(op,arrayIndex,ans,ansSymbol);
                if (indexType == CharExp) errorHandler(i);//数组下表是字符类型
                auto* pss = getCurPosition();
                type = getNextSym(ans);
                if (type != RBRACK) {
                    callBackStreamPos(pss);
                    errorHandler(m);
                }
            } else {
                if (isIntVar(ans)) ansType = IntExp;//只要有变量是int类型
                callBackStreamPos(ps2);
                ansSymbol = ans;
            }
        }
    } else if (type == LPARENT) {//只要是有嵌套表达式
        ansType = IntExp;
        expressionParser(ansSymbol, nullptr);
        auto* pss = getCurPosition();
        type = getNextSym(ans);
        if (type != RPARENT) {
            callBackStreamPos(pss);
            errorHandler(l);
        }
    } else if (type == PLUS || type == MINU || type == INTCON) {//只要存在数字
        ansType = IntExp;
        callBackStreamPos(ps);
        ansSymbol = to_string(integerParser());
    }  else if (type == CHARCON) {
        ansSymbol = "\'" + ans + "\'";
        return ansType;
    }
    return ansType;
}

//完成
void SyntaxParser::returnFuncCallParser(string &ansSymbol, SyntaxNode *parentNode) {
    ansSymbol = funcCallParser(true, nullptr);
}

//完成
void SyntaxParser::voidFuncCallParser(SyntaxNode *parentNode) {
    funcCallParser(false, nullptr);
}

//完成
string SyntaxParser::funcCallParser(bool isReturn, SyntaxNode *parentNode) {
    string ans;
    string ansSymbol;
    string functionName;
    TokenType type = getNextSym(functionName);
    if (type != IDENFR) error();
    checkEntryExist(functionName);//检查函数是否存在
    type = getNextSym(ans);
    if (type != LPARENT) error();
    funcCallValueListParser(functionName, nullptr);//函数调用的参数表
    if (isReturn) {
        ansSymbol = applyVariableName();//申请一个变量
        retFunctionCallIR(ansSymbol, functionName);
    } else {
        voidFunctionCallIR(functionName);
    }
    auto* ps = getCurPosition();
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandler(l);
    }
    return ansSymbol;
}

//完成
void SyntaxParser::funcCallValueListParser(string &functionName, SyntaxNode *parentNode) {
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    vector<ExprType> valueList;
    vector<string> paras;
    while (type != RPARENT && type != SEMICN) {//缺少右括号的情况
        string funcPara;
        callBackStreamPos(ps);
        valueList.push_back(expressionParser(funcPara, nullptr));
        paras.push_back(funcPara);
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type == COMMA) {
            flushStreamPos(ps);
        }
    }
    spareRetStackAddrIR(functionName);
    for (auto para : paras) {
        functionCallParaIR(para);
    }
    checkFunctionParameterList(functionName, valueList);//检查函数参数表
    callBackStreamPos(ps);
}

void error() {
    printf("Sorry, something wrong happened, we will fix it soon...\n");
}
