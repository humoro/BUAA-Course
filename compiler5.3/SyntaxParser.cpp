#include<cstdio>
#include<unordered_map>
#include "Type.h"
#include "Parser.h"
#include "Token.h"
using namespace std;
void SyntaxParser::startProgramParser() {
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == CONSTTK) {//处理程序中的全局常量说明
        callBackStreamPos(ps);
        constDeclarationParser();
        flushStreamPos(ps);
    }
    callBackStreamPos(ps);
    type = getNextSym(ans);
    if (type == INTTK || type == CHARTK) { //处理程序的全局变量说明
        getNextSym(ans);
        type = getNextSym(ans);
        if (type != LPARENT) {//向后读两个单词，遇到左括号说明是函数定义
            callBackStreamPos(ps);
            variableDeclarationParser();
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
            returnFuncDefinitionParser();
            flushStreamPos(ps);
        } else {
            type = getNextSym(ans);
            if (type == MAINTK) {
                reachMain = true;
            } else {
                callBackStreamPos(ps);
                voidFuncDefinitionParser();
                flushStreamPos(ps);
            }
        }
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
    mainFuncParser();
}

void SyntaxParser::constDeclarationParser() {//解析常量声明
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    while (type == CONSTTK) {
        constDefinitionParser();//解析常量的定义
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != SEMICN) {
            if (getLineNumber() > ps->getLineNumber()) {//如果是缺少分号而解析到了下一行
                callBackStreamPos(ps);
            }
            errorHandle(k);
        }
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
}

void SyntaxParser::constDefinitionParser() {//解析常量定义
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
            if (type != INTCON) errorHandle(o);//常量定义中的int型变量赋值不是整型
            varValue = stoi(valueStr);//获取常量的数值
        } else  if (identifierType == CHARTK) {
            type = getNextSym(ans);
            if (type != CHARCON) errorHandle(o);
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

void SyntaxParser::variableDeclarationParser() {//解析变量声明
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
            variableDefinitionParser();
            flushStreamPos(ps);
            type = getNextSym(ans);
            if (type != SEMICN) { //没有分号解析到下一行，需要回退
                callBackStreamPos(ps);
                errorHandle(k);
            }
            flushStreamPos(ps);
        }
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
}

void SyntaxParser::variableDefinitionParser() {//解析变量定义
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
        addArrayEntry(identifierType,varName);//添加数组的符号表项
        size = unsignedIntParser();
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != RBRACK) {//缺少右中括号
            callBackStreamPos(ps);
            errorHandle(m);
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

unsigned int SyntaxParser::unsignedIntParser() {
    string ans;
    TokenType type = getNextSym(ans);
    if (type != INTCON) error();
    if (ans.size() > 1 && ans[0] == '0') error();
    return stoi(ans);
}

void SyntaxParser::returnFuncDefinitionParser() {
    string ans;
    TokenType type = getNextSym(ans);
    string functionName;
    if (type != INTTK && type != CHARTK) error();
    TokenType type1 = getNextSym(functionName);
    if (type1 != IDENFR) error();
    functionDefIR(functionName);//中间代码函数标签
    addFunction(type,functionName);//添加有返回值函数符号表项
    funcStatementParser(functionName);
}

void SyntaxParser::voidFuncDefinitionParser() {
    string ans;
    TokenType type = getNextSym(ans);
    string functionName;
    if (type != VOIDTK) error();
    type = getNextSym(functionName);
    if (type != IDENFR) error();
    functionDefIR(functionName);//中间代码函数标签
    addFunction(VOIDTK,functionName);//添加无返回值符号表项
    funcStatementParser(functionName);
}

void SyntaxParser::funcStatementParser(string& functionName) {//主要是解析函数的参数表和执行语句
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != LPARENT) error();
    funcParameterListParser();//解析函数的参数表
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandle(l);//参数表缺少右括号
    }
    type = getNextSym(ans);
    if (type != LBRACE) error();
    compoundStatementParser(functionName);
    type = getNextSym(ans);
    if (type != RBRACE) error();
}

void SyntaxParser::funcParameterListParser() {//解析函数参数表
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

void SyntaxParser::compoundStatementParser(string& functionName) {//复合语句的解析程序
    string ans;auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == CONSTTK) {//常量说明
        callBackStreamPos(ps);
        constDeclarationParser();
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    if (type == INTTK || type == CHARTK) {//变量说明
        callBackStreamPos(ps);
        variableDeclarationParser();
        flushStreamPos(ps);
    }
    callBackStreamPos(ps);
    statementColumnParser();
    checkReturnFunctionReturn();
}

void SyntaxParser::statementColumnParser() {
    string ans;auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    while (type != RBRACE) {
        callBackStreamPos(ps);
        statementParser();
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
}

void SyntaxParser::statementParser() {//语句解析
    string ans;auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == RETURNTK) {//返回语句
        callBackStreamPos(ps);
        returnStatementParser();
    } else if (type == SCANFTK) {//读语句
        callBackStreamPos(ps);
        readStatementParser();
    } else if (type == PRINTFTK) {//写语句
        callBackStreamPos(ps);
        printStatementParser();
    } else if (type == IFTK) {//条件语句
        callBackStreamPos(ps);
        conditionStatementParser();
        return;
    } else if (type == WHILETK || type == FORTK || type == DOTK) {//循环语句
        callBackStreamPos(ps);
        recycleStatementParser();
        return;
    }  else if (type == LBRACE) {//语句列
        statementColumnParser();
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
                returnFuncCallParser(ansSymbol);
            } else {
                voidFuncCallParser();
            }
        } else {//赋值语句
            assignStatementParser();
        }
    } else if (type == SEMICN) {//空语句
        callBackStreamPos(ps);
    } else error();
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != SEMICN || getLineNumber() > ps->getLineNumber()) {
        callBackStreamPos(ps);
        errorHandle(k);
    }
}

void SyntaxParser::readStatementParser() { //scanf语句
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
        errorHandle(l);
    }
}

void SyntaxParser::printStatementParser() {
    string ans;auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != PRINTFTK) error();
    type = getNextSym(ans);
    if (type != LPARENT) error();
    string ansSymbol;
    if (peek() == '\"') {//说明即将要输出的是字符串
        string printStr = stringParser();
        printStr.insert(printStr.begin(),'\"');
        printStr.push_back('\"');
        printStrIR(printStr);//输出字符串中间代码
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type == COMMA) {
            expressionParser(ansSymbol);
            printSymbolIR(ansSymbol);
        } else {
            callBackStreamPos(ps);
        }
    } else {
        expressionParser(ansSymbol);
        printSymbolIR(ansSymbol);
    }
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandle(l);
    }
}

//完成
void SyntaxParser::returnStatementParser() {
    string ans;auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != RETURNTK) error();
    flushStreamPos(ps);
    ExprType exprType = None;
    type = getNextSym(ans);
    string ansSymbol;
    if (type == LPARENT) {
        exprType = expressionParser(ansSymbol);
        returnFuncReturnIR(ansSymbol);//返回值函数的返回语句
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != RPARENT) {
            callBackStreamPos(ps);
            errorHandle(l);
        }
    } else {
        callBackStreamPos(ps);
        voidFuncReturnIR();
    }
    addFunctionReturn(exprType);
}

//完成
//赋值语句
void SyntaxParser::assignStatementParser() {
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
        ExprType expType = expressionParser(indexSymbol);
        left = left + "[" + indexSymbol + "]";
        if (expType == CharExp) errorHandle(i);//数组的下表是字符类型
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != RBRACK) {
            callBackStreamPos(ps);
            errorHandle(m);
        }
        type = getNextSym(ans);
        if (type != ASSIGN) error();
    }
    string right;
    expressionParser(right);
    assignIR(left,right);
}

//完成
//条件语句
void SyntaxParser::conditionStatementParser() {
    string ans;
    string oriCodeLabel = applyCodeLabel();
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != IFTK) error();
    type = getNextSym(ans);
    if (type != LPARENT) error();
    string condition;
    conditionParser(condition);
    reverseCondition(condition);//条件的反
    conditionIR(oriCodeLabel,condition);//if condition goto label
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandle(l);
    }
    statementParser();//语句
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type == ELSETK) {
        string codeLabel = applyCodeLabel();
        gotoIR(codeLabel);
        setLabel(oriCodeLabel);
        statementParser();
        setLabel(codeLabel);
    } else {
        setLabel(oriCodeLabel);
        callBackStreamPos(ps);
    }
}

void SyntaxParser::conditionParser(string& condition) {
    string left;
    string cmpOp;
    string right;
    ExprType exprType0 = expressionParser(left);
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
    exprType1 = expressionParser(right);
    end:
        if (exprType0 == CharExp || exprType1 == CharExp) errorHandle(f);//条件中任意一个表达式返回值是char报错
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
                        cmpOp = ">";
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
void SyntaxParser::whileRecycleStatementParser() {
    string ans;
    string condition;
    string whileLabel = applyWhileCycleLabel();
    string codeLabel = applyCodeLabel();
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != WHILETK) error();
    type = getNextSym(ans);
    if (type != LPARENT) error();
    setLabel(whileLabel);
    conditionParser(condition);
    reverseCondition(condition);
    conditionIR(codeLabel,condition);
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandle(l);
    }
    statementParser();
    gotoIR(whileLabel);
    setLabel(codeLabel);
}

//
void SyntaxParser::doWhileRecycleStatementParser() {
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != DOTK) error();
    string doLabel = applyDoCycleLabel();
    setLabel(doLabel);
    statementParser();
    type = getNextSym(ans);
    if (type != WHILETK) {
        errorHandle(n);
    }//n类错误，do后没有匹配的while语句
    else {
        type = getNextSym(ans);
    }
    if (type != LPARENT) error();
    string condition;
    conditionParser(condition);
    flushStreamPos(ps);//记录当前的分析完条件的位置
    conditionIR(doLabel,condition);
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != RPARENT || getLineNumber() > ps->getLineNumber()) {//解析下一个符号的时候换行了说明没有右括号
        callBackStreamPos(ps);
        errorHandle(l);//l类错误，应该有右括号
    }
}

void SyntaxParser::forRecycleStatementParser() {
    string ans;
    string forCycleLabel = applyForCycleLabel();
    string codeLabel = applyCodeLabel();
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
    expressionParser(ansSymbol);
    assignIR(left,ansSymbol);//初始化语句
    setLabel(forCycleLabel);
    flushStreamPos(ps);
    type = getNextSym(ans);
    string condition;
    if (type != SEMICN) {
        errorHandle(k);//缺少分号
        if (type == LSS || type == LEQ || type == GEQ || type == GRE || type == EQL || type == NEQ) {//解析到下一个字符是比较运算符，说明已经缺少分号并且条件的左边已经被解析完成
            while (!isSemi(peek())) get();//吞到下一个分号位置，出现类似for(i = 0 (缺失分号;)- i  < 1;i = i + 1)
            goto nextSemi;//直接进行步长的解析
        } else {
            callBackStreamPos(ps);//说明当前只是缺少分号其他成分还没有缺少,出现类似for(i = 0 (缺失分号;)<条件>;i = i + 1)
        }
    }
    flushStreamPos(ps);//解析条件之前的文件指针位置
    conditionParser(condition);
    reverseCondition(condition);
    conditionIR(codeLabel,condition);
    nextSemi://下面是步长的解析
    auto* pss = getCurPosition();
    type = getNextSym(ans);
    if (type != SEMICN) {
        errorHandle(k);
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
        errorHandle(l);
    }
    statementParser();
    operationIR(op,opnum,step,opnum);
    gotoIR(forCycleLabel);
    setLabel(codeLabel);
}

//完成
void SyntaxParser::recycleStatementParser() {
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    callBackStreamPos(ps);
    if (type == WHILETK) {//while语句
        whileRecycleStatementParser();
    } else if (type == DOTK) {//do-while语句
        doWhileRecycleStatementParser();
    } else if (type == FORTK) {//for循环语句
        forRecycleStatementParser();
    } else error();
}

//完成
unsigned int SyntaxParser::stepStatementParser() {
    return unsignedIntParser();
}

//完成
void SyntaxParser::mainFuncParser() {
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
        errorHandle(l);
    }
    type = getNextSym(ans);
    if (type != LBRACE) error();
    string mainName = "main";
    addFunction(VOIDTK,mainName);//添加main函数到符号表
    compoundStatementParser(mainName);
    type = getNextSym(ans);
    if (type != RBRACE) error();
    voidFuncReturnIR();
}

//完成
string SyntaxParser::stringParser() {
    string ans;
    TokenType  type = getNextSym(ans);
    if (type != STRCON) error();
    return ans;
}

//完成
int SyntaxParser::integerParser() {
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
ExprType SyntaxParser::expressionParser(string& ansSymbol) {//表达式
    ExprType ansType = CharExp;//默认的表达式的返回值是字符类型的
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if ((type == PLUS || type == MINU)) {//只要有加减运算就是int型
        ansType = IntExp;
    } else {
        callBackStreamPos(ps);
    }
    ansSymbol = applyOperationSymbol();
    string itemSymbol;
    string blankStr;
    ExprType itemType = itemParser(itemSymbol);
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
        itemParser(itemSymbol);
        operationIR(ans, ansSymbol, itemSymbol,ansSymbol);
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
    return ansType;
}

//完成
ExprType SyntaxParser::itemParser(string& ansSymbol) {//项
    ExprType ansType = CharExp;
    ansSymbol = applyOperationSymbol();
    string factorSymbol;
    ExprType factorType = factorParser(factorSymbol);
    if (factorType == IntExp) {//只要有因子返回值是int型
        ansType = IntExp;
    }
    assignIR(ansSymbol,factorSymbol);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    while (type == MULT || type == DIV) {//只要有多个因子就是int型
        ansType = IntExp;
        factorParser(factorSymbol);
        operationIR(ans, ansSymbol, factorSymbol,ansSymbol);
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
    return ansType;
}

//完成
ExprType SyntaxParser::factorParser(string& ansSymbol) {//因子解析
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
            returnFuncCallParser(ansSymbol);//有返回值的函数调用
        } else {
            if (type2 == LBRACK) {//数组
                if (isIntArray(ans)) ansType = IntExp;//只要有数组是int类型
                string arrayIndex;
                ExprType indexType = expressionParser(arrayIndex);
                ansSymbol = ans + "[" + arrayIndex + "]";
                if (indexType == CharExp) errorHandle(i);//数组下表是字符类型
                auto* pss = getCurPosition();
                type = getNextSym(ans);
                if (type != RBRACK) {
                    callBackStreamPos(pss);
                    errorHandle(m);
                }
            } else {
                if (isIntVar(ans)) ansType = IntExp;//只要有变量是int类型
                callBackStreamPos(ps2);
                ansSymbol = ans;
            }
        }
    } else if (type == LPARENT) {//只要是有嵌套表达式
        ansType = IntExp;
        expressionParser(ansSymbol);
        auto* pss = getCurPosition();
        type = getNextSym(ans);
        if (type != RPARENT) {
            callBackStreamPos(pss);
            errorHandle(l);
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
void SyntaxParser::returnFuncCallParser(string& ansSymbol) {
    ansSymbol = funcCallParser(true);
}

//完成
void SyntaxParser::voidFuncCallParser() {
    funcCallParser(false);
}

//完成
string SyntaxParser::funcCallParser(bool isReturn) {
    string ans;
    string ansSymbol;
    string functionName;
    TokenType type = getNextSym(functionName);
    if (type != IDENFR) error();
    checkEntryExist(functionName);//检查函数是否存在
    type = getNextSym(ans);
    if (type != LPARENT) error();
    funcCallValueListParser(functionName);//函数调用的参数表
    if (isReturn) {
        ansSymbol = applyOperationSymbol();
        returnFuncCallIR(ansSymbol,functionName);
    } else {
        voidFuncCallIR(functionName);
    }
    auto* ps = getCurPosition();
    type = getNextSym(ans);
    if (type != RPARENT) {
        callBackStreamPos(ps);
        errorHandle(l);
    }
    return ansSymbol;
}

//完成
void SyntaxParser::funcCallValueListParser(string& functionName) {
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    vector<ExprType> valueList;
    vector<string> paras;
    while (type != RPARENT && type != SEMICN) {//缺少右括号的情况
        string funcPara;
        callBackStreamPos(ps);
        valueList.push_back(expressionParser(funcPara));
        paras.push_back(funcPara);
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type == COMMA) {
            flushStreamPos(ps);
        }
    }
    for (auto para : paras) {
        functionCallParaIR(para);
    }
    checkFunctionParameterList(functionName, valueList);//检查函数参数表
    callBackStreamPos(ps);
}

void error() {
    printf("Sorry, something wrong happened, we will fix it soon...\n");
}
