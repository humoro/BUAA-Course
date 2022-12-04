#include <cstdio>
#include <unordered_map>
#include <sstream>
#include "Type.h"
#include "Parse.h"
using namespace std;
void SyntaxParser::startProgramParser() {
    auto* ps = getCurPosition();
    string ans;
    TokenType type = getNextSym(ans);
    if (type == token_constToken) {//处理程序中的全局常量说明
        callBackStreamPos(ps);
        constDeclarationParser(root);
        flushStreamPos(ps);
    }
    callBackStreamPos(ps);
    type = getNextSym(ans);
    if (type == token_intToken || type == token_charToken) { //处理程序的全局变量说明
        getNextSym(ans);
        type = getNextSym(ans);
        if (type != token_lparent) {//向后读两个单词，遇到左括号说明是函数定义
            callBackStreamPos(ps);
            variableDeclarationParser(root);
            flushStreamPos(ps);
        }
    }
    this->isglobal = false;
    callBackStreamPos(ps);
    type = getNextSym(ans);
    bool reachMain = false;
    while ((type == token_intToken || type == token_charToken || type == token_voidToken) && !reachMain) { //处理程序中的函数说明
        if (type == token_intToken || type == token_charToken) {
            callBackStreamPos(ps);
            returnFuncDefinitionParser(root);
            flushStreamPos(ps);
        } else {
            type = getNextSym(ans);
            if (type == token_mainToken) {
                reachMain = true;
            } else {
                callBackStreamPos(ps);
                voidFuncDefinitionParser(root);
                flushStreamPos(ps);
            }
        }
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
    mainFuncParser(root);
    auto* endNode = new IRSyntaxNode(syntax_endProgram);//程序最终的结束的标签
    root->addKid(endNode);
    endProgram(endNode);
}

void SyntaxParser::constDeclarationParser(IRSyntaxNode *parentNode) {//解析常量声明
    auto* kid = new IRSyntaxNode(syntax_constantDecl);
    parentNode->addKid(kid);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    while (type == token_constToken) {
        constDefinitionParser(kid);//解析常量的定义
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != token_semicolon) {
            if (getLineNumber() > ps->getLineNumber()) {//如果是缺少分号而解析到了下一行
                callBackStreamPos(ps);
            }
            errorHandler(error_k);
        }
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
}

//-----////
void SyntaxParser::constDefinitionParser(IRSyntaxNode *parentNode) {//解析常量定义
    auto* kid = new IRSyntaxNode(syntax_ConstantDef);
    parentNode->addKid(kid);
    string ans;
    string varName;
    int varValue  = -1;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    string typeString = ans;
    TokenType identifierType = type;
    while (type != token_semicolon) {
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != token_identifier) error();
        varName = ans;//常量名
        type = getNextSym(ans);
        if (type != token_assign) error();
        if (identifierType == token_intToken) {
            type = getNextSym(ans);
            string valueStr = ans;
            if (type == token_plus || type == token_minus) {
                type = getNextSym(ans);
                valueStr += ans;
            }
            if (type != token_intConstant) errorHandler(error_o);//常量定义中的int型变量赋值不是整型
            varValue = stoi(valueStr);//获取常量的数值
        } else  if (identifierType == token_charToken) {
            type = getNextSym(ans);
            if (type != token_charConstant) errorHandler(error_o);
            varValue = int(ans[0]);//获取常量的数值
        } else error();
        addVariableEntry(identifierType, varName, true, varValue);//添加常量的符号表项
        constDefIR(varName, typeString, varValue, kid);//生成常量声明的中间代码
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != token_comma && type != token_semicolon && getLineNumber() > ps->getLineNumber()) {//没有分号已经解析到下一行
            callBackStreamPos(ps);
            break;
        }
    }
    callBackStreamPos(ps);
}

void SyntaxParser::variableDeclarationParser(IRSyntaxNode *parentNode) {//解析变量声明
    auto* kid = new IRSyntaxNode(syntax_variableDecl);
    parentNode->addKid(kid);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    while (type == token_intToken || type == token_charToken) {
        getNextSym(ans);
        type = getNextSym(ans);
        if (type == token_lparent) {//左括号说明碰到的是函数声明
            break;
        } else {
            callBackStreamPos(ps);
            variableDefinitionParser(kid);
            flushStreamPos(ps);
            type = getNextSym(ans);
            if (type != token_semicolon) { //没有分号解析到下一行，需要回退
                callBackStreamPos(ps);
                errorHandler(error_k);
            }
            flushStreamPos(ps);
        }
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
}

void SyntaxParser::variableDefinitionParser(IRSyntaxNode *parentNode) {//解析变量定义
    auto* kid = new IRSyntaxNode(syntax_variableDef);
    parentNode->addKid(kid);
    string ans;
    TokenType type;
    auto* ps = getCurPosition();
    type = getNextSym(ans);//提取类型标识符
    string typeString = ans;
    TokenType identifierType = type;
    string varName;
    unsigned int size;//默认变量是一维的变量
    while (type != token_semicolon) {
        size = 1;
        type = getNextSym(ans);
        flushStreamPos(ps);
        if (type != token_identifier) error();
        varName = ans;//存储变量的标识符
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != token_lbrack) {
            addVariableEntry(identifierType,varName,false,0);//添加常量符号表项
            variableDefIR(varName, typeString, size, kid);//一维变量的声明
            continue;
        }
        size = unsignedIntParser();
        addArrayEntry(identifierType,varName,size);//添加数组的符号表项
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != token_rbrack) {//缺少右中括号
            callBackStreamPos(ps);
            errorHandler(error_m);
        }
        variableDefIR(varName, typeString, size, kid);//数组的声明
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != token_comma && type != token_semicolon && getLineNumber() > ps->getLineNumber()) {//没有分号，解析到下一行，需要回退
            callBackStreamPos(ps);
            break;
        }
    }
    callBackStreamPos(ps);
}

unsigned int SyntaxParser::unsignedIntParser() {  // 无符号常数的解析,这是一个叶子函数
    string ans;
    TokenType type = getNextSym(ans);
    if (type != token_intConstant) error();
    if (ans.size() > 1 && ans[0] == '0') error();
    return stoi(ans);
}

void SyntaxParser::returnFuncDefinitionParser(IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_valueReturnFunc);
    parentNode->addKid(kid);
    string ans;
    TokenType type = getNextSym(ans);
    string functionName, typeString;
    typeString = ans;
    if (type != token_intToken && type != token_charToken) error();
    TokenType type1 = getNextSym(functionName);
    if (type1 != token_identifier) error();
    addFunctionNameLabelParser(functionName, typeString, kid);
    addFunction(type,functionName);//添加有返回值函数符号表项
    funcStatementParser(functionName, kid);
}

void SyntaxParser::voidFuncDefinitionParser(IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_voidReturnFunc);
    parentNode->addKid(kid);
    string ans;
    TokenType type = getNextSym(ans);
    string functionName, typeString = "void";
    if (type != token_voidToken) error();
    type = getNextSym(functionName);
    if (type != token_identifier) error();
    addFunctionNameLabelParser(functionName, typeString, kid);
    addFunction(token_voidToken, functionName);//添加无返回值符号表项
    funcStatementParser(functionName, kid);
    addVoidReturnStatementParser(kid);//无论无返回值函数是否有返回语句都添加一个返回中间代码
}

void SyntaxParser::addFunctionNameLabelParser(string &functionName, string &typeString, IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_functionLabel_ir);
    parentNode->addKid(kid);
    functionDefIR(functionName, typeString, kid);//中间代码函数标签
}

void SyntaxParser::funcStatementParser(string &functionName, IRSyntaxNode *parentNode) {//主要是解析函数的参数表和执行语句
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != token_lparent) error();
    funcParameterListParser(parentNode);//解析函数的参数表
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != token_rparent) {
        callBackStreamPos(ps);
        errorHandler(error_l);//参数表缺少右括号
    }
    type = getNextSym(ans);
    if (type != token_lbrace) error();
    compoundStatementParser(functionName, parentNode);
    type = getNextSym(ans);
    if (type != token_rbrace) error();
}

void SyntaxParser::funcParameterListParser(IRSyntaxNode *parentNode) {//解析函数参数表
    auto* kid = new IRSyntaxNode(syntax_ParameterTable);
    parentNode->addKid(kid);
    string ans, typeString;
    auto* ps = getCurPosition();
    TokenType varType = getNextSym(ans);
    TokenType identifierType = varType;//记录函数标识符的类型
    typeString = ans;
    if (varType == token_rparent || varType == token_lbrace) {//没有右括号的时候终止需要通过达到大括号来判断
        callBackStreamPos(ps);
    } else {
        for (;;) {
            if (identifierType != token_intToken && identifierType != token_charToken) error();
            varType = getNextSym(ans);
            if (varType != token_identifier) error();
            flushStreamPos(ps);
            functionDefParaIR(ans, typeString, kid);//参数的中间代码
            addFunctionParameter(identifierType,ans);//添加参数表内容到函数的符号表,ans就是参数的名
            varType = getNextSym(ans);
            if (varType == token_comma) {
                identifierType = getNextSym(ans);
                typeString = ans;
            } else if (varType == token_rparent || varType == token_lbrace){ //没有右括号的时候终止需要通过达到大括号来判断
                break;
            } else {
                error();
                break;
            }
        }
        callBackStreamPos(ps);
    }
}

void SyntaxParser::compoundStatementParser(string &functionName, IRSyntaxNode *parentNode) {//复合语句的解析程序
    auto* kid = new IRSyntaxNode(syntax_cmpdStmt);
    parentNode->addKid(kid);
    string ans;auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == token_constToken) {//常量说明
        callBackStreamPos(ps);
        constDeclarationParser(kid);
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    if (type == token_intToken || type == token_charToken) {//变量说明
        callBackStreamPos(ps);
        variableDeclarationParser(kid);
        flushStreamPos(ps);
    }
    callBackStreamPos(ps);
    statementColumnParser(kid);
    checkReturnFunctionReturn();
}

void SyntaxParser::statementColumnParser(IRSyntaxNode *parentNode) { // 语句列
    auto* kid = new IRSyntaxNode(syntax_stmtColumn);
    parentNode->addKid(kid);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    while (type != token_rbrace) {
        callBackStreamPos(ps);
        statementParser(kid);
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
}

void SyntaxParser::statementParser(IRSyntaxNode *parentNode) {//语句解析
    auto* kid = new IRSyntaxNode(syntax_statement);
    parentNode->addKid(kid);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == token_returnToken) {//返回语句
        callBackStreamPos(ps);
        returnStatementParser(kid);
    } else if (type == token_scanfToken) {//读语句
        callBackStreamPos(ps);
        readStatementParser(kid);
    } else if (type == token_printfToken) {//写语句
        callBackStreamPos(ps);
        printStatementParser(kid);
    } else if (type == token_ifToken) {//条件语句
        callBackStreamPos(ps);
        conditionStatementParser(kid);
        return;
    } else if (type == token_whileToken || type == token_forToken || type == token_doToken) {//循环语句
        callBackStreamPos(ps);
        recycleStatementParser(kid);
        return;
    }  else if (type == token_lbrace) {//语句列
        statementColumnParser(kid);
        type = getNextSym(ans);
        if (type != token_rbrace) error();
        return;
    } else if (type == token_identifier) {
        string ans2;
        TokenType type2 = getNextSym(ans2);
        callBackStreamPos(ps);
        if (type2 == token_lparent) {//函数调用
            checkEntryExist(ans);//检查标识符是否定义
            if (isReturnFunc(ans)) {//如果是返回函数
                string ansSymbol;
                returnFuncCallParser(ansSymbol, kid);
            } else {
                voidFuncCallParser(kid);
            }
        } else {//赋值语句
            assignStatementParser(kid);
        }
    } else if (type == token_semicolon) {//空语句
        callBackStreamPos(ps);
    } else error();
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != token_semicolon && getLineNumber() > ps->getLineNumber()) {
        callBackStreamPos(ps);
        errorHandler(error_k);
    }
}

void SyntaxParser::readStatementParser(IRSyntaxNode *parentNode) { //scanf语句
    auto* kid = new IRSyntaxNode(syntax_readStmt);
    parentNode->addKid(kid);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != token_scanfToken) error();
    type = getNextSym(ans);
    if (type != token_lparent) error();
    flushStreamPos(ps);
    while (type != token_rparent && type != token_semicolon) {
        type = getNextSym(ans);
        if (type != token_identifier) error();
        checkEntryExist(ans);
        checkPlantVariable(ans);
        readIR(ans, kid);//读语句的中间代码
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    if (type != token_rparent) {
        callBackStreamPos(ps);
        errorHandler(error_l);
    }
}

void SyntaxParser::printStatementParser(IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_printStmt);
    parentNode->addKid(kid);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != token_printfToken) error();
    type = getNextSym(ans);
    if (type != token_lparent) error();
    string ansSymbol;
    if (peek() == '\"') {//说明即将要输出的是字符串
        string printStr = stringParser();
        string strName = applyStringName(printStr);
        addPrintNodeParser(strName, printStr, kid);//输出字符串中间代码
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type == token_comma) {
            ExprType exprType = expressionParser(ansSymbol, kid);
            addPrintNodeParser(ansSymbol, exprType, kid);
        } else {
            callBackStreamPos(ps);
        }
    } else {
        ExprType exprType = expressionParser(ansSymbol, kid);
        addPrintNodeParser(ansSymbol, exprType, kid);
    }
    addPrintNewLineParser(kid);
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != token_rparent) {
        callBackStreamPos(ps);
        errorHandler(error_l);
    }
}

void SyntaxParser::addPrintNodeParser(string &strName, string &content, IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_printStmt_ir);
    parentNode->addKid(kid);
    printStrIR(strName, content, parentNode);
}

void SyntaxParser::addPrintNodeParser(string &ansSymbol, ExprType exprType, IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_printStmt_ir);
    parentNode->addKid(kid);
    printExprIR(ansSymbol, exprType, parentNode);
}

void SyntaxParser::addPrintNewLineParser(IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_printStmt_ir);
    parentNode->addKid(kid);
    printNewlineIR(parentNode);
}

//完成
void SyntaxParser::returnStatementParser(IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_returnStmt);
    parentNode->addKid(kid);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != token_returnToken) error();
    flushStreamPos(ps);
    ExprType exprType = expr_none;
    type = getNextSym(ans);
    string ansSymbol;
    if (type == token_lparent) {
        exprType = expressionParser(ansSymbol, kid);
        addValueReturnParser(kid, ansSymbol);//返回值函数的返回语句
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != token_rparent) {
            callBackStreamPos(ps);
            errorHandler(error_l);
        }
    } else {
        callBackStreamPos(ps);
        addVoidReturnStatementParser(kid);
    }
    addFunctionReturn(exprType);
}

void SyntaxParser::addVoidReturnStatementParser(IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_returnStmt_ir);
    parentNode->addKid(kid);
    voidReturnIR(kid);
}

void SyntaxParser::addValueReturnParser(IRSyntaxNode *parentNode, string &retSymbol) {
    auto* kid = new IRSyntaxNode(syntax_returnStmt_ir);
    parentNode->addKid(kid);
    valueReturnIR(retSymbol, kid);
}


//完成
//赋值语句
void SyntaxParser::assignStatementParser(IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_assign);
    parentNode->addKid(kid);
    string ans;
    string left;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != token_identifier) error();
    left += ans;
    checkVariableExist(ans);//赋值语句左边的变量是未定义的变量
    checkAssignable(ans);//赋值左端的变量是常量不可改变
    type = getNextSym(ans);
    if (type != token_lbrack && type != token_assign) error();
    if (type == token_lbrack) {//等式左边是数组
        string indexSymbol;
        ExprType expType = expressionParser(indexSymbol, kid);
        left = left + "[" + indexSymbol + "]";
        if (expType == expr_char) errorHandler(error_i);//数组的下表是字符类型
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type != token_rbrack) {
            callBackStreamPos(ps);
            errorHandler(error_m);
        }
        type = getNextSym(ans);
        if (type != token_assign) error();
    }
    string right;
    expressionParser(right, kid);
    addAssignParser(left, right, kid);
}

void SyntaxParser::addAssignParser(string &left, string &right, IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_assignStmt_ir);
    parentNode->addKid(kid);
    assignIR(left, right, kid);
}

//完成
void SyntaxParser::conditionStatementParser(IRSyntaxNode *parentNode) { //条件语句
    auto* kid = new IRSyntaxNode(syntax_conditionStmt);
    parentNode->addKid(kid);
    string ans;
    string oriCodeLabel = applyCodeBlkLabel();
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != token_ifToken) error();
    type = getNextSym(ans);
    if (type != token_lparent) error();
    string condition;
    conditionParser(condition, kid);
    reverseCondition(condition);//条件的反
    addConditionalJumpParser(oriCodeLabel, condition, kid);//if condition goto labelSet
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != token_rparent) {
        callBackStreamPos(ps);
        errorHandler(error_l);
    }
    statementParser(kid);//语句
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type == token_elseToken) {
        string codeLabel = applyCodeBlkLabel();
        addUnconditionalJumpParser(codeLabel, kid);
        addCodeLabelParser(oriCodeLabel, kid);
        statementParser(kid);
        addCodeLabelParser(codeLabel, kid);
    } else {
        addCodeLabelParser(oriCodeLabel, kid);
        callBackStreamPos(ps);
    }
}

void SyntaxParser::addConditionalJumpParser(string &jumpLabel, string &condition, IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_conditionalJump_ir);
    parentNode->addKid(kid);
    conditionalJumpIR(jumpLabel, condition, kid);
}

void SyntaxParser::addUnconditionalJumpParser(string &jumpLabel, IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_unconditionalJump_ir);
    parentNode->addKid(kid);
    unconditionalJumpIR(jumpLabel, kid);
}

void SyntaxParser::conditionParser(string &condition, IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_condition);
    parentNode->addKid(kid);
    string left;
    string cmpOp;
    string right;
    ExprType exprType0 = expressionParser(left, kid);
    ExprType exprType1 = expr_int;
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == token_rparent || type == token_semicolon) {//碰到右括号分号或者在没有右括号的情况下碰到左大括号或者直接解析到了下一行,也有可能在for语句中条件之后缺少分号
        callBackStreamPos(ps);
        goto end;
    }
    if (type != token_less && type != token_greater && type != token_gequal && type != token_equal && type != token_lequal && type != token_nequal) {
        error();
        goto end;
    }
    cmpOp = ans;
    exprType1 = expressionParser(right, kid);
    end:
        if (exprType0 == expr_char || exprType1 == expr_char) errorHandler(error_f);//条件中任意一个表达式返回值是char报错
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
        condition.clear();
        condition = left + " == 0";
        return;
    } else {
        getline(ss,right,' ');
        condition.clear();
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
void SyntaxParser::whileRecycleStatementParser(IRSyntaxNode *parentNode) {
    string ans;
    string condition;
    string whileLabel = applyWhileCycleLabel();
    string codeLabel = applyCodeBlkLabel();
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != token_whileToken) error();
    type = getNextSym(ans);
    if (type != token_lparent) error();
    addCodeLabelParser(whileLabel, parentNode); //添加while循环的标签
    conditionParser(condition, parentNode);
    reverseCondition(condition);
    addConditionalJumpParser(codeLabel, condition, parentNode);
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != token_rparent) {
        callBackStreamPos(ps);
        errorHandler(error_l);
    }
    statementParser(parentNode);
    addUnconditionalJumpParser(whileLabel, parentNode);
    addCodeLabelParser(codeLabel, parentNode);
}

//
void SyntaxParser::doWhileRecycleStatementParser(IRSyntaxNode *parentNode) {
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != token_doToken) error();
    string doLabel = applyDoCycleLabel();
    addCodeLabelParser(doLabel, parentNode);
    statementParser(parentNode);
    type = getNextSym(ans);
    if (type != token_whileToken) {
        errorHandler(error_n);
    }//n类错误，do后没有匹配的while语句
    else {
        type = getNextSym(ans);
    }
    if (type != token_lparent) error();
    string condition;
    conditionParser(condition, parentNode);
    flushStreamPos(ps);//记录当前的分析完条件的位置
    addConditionalJumpParser(doLabel, condition, parentNode);
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != token_rparent) {//解析下一个符号的时候换行了说明没有右括号
        callBackStreamPos(ps);
        errorHandler(error_l);//l类错误，应该有右括号
    }
}

void SyntaxParser::forRecycleStatementParser(IRSyntaxNode *parentNode) {
    string ans;
    string forCycleLabel = applyForCycleLabel();
    string codeLabel = applyCodeBlkLabel();
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type != token_forToken) error();
    type = getNextSym(ans);
    if (type != token_lparent) error();
    type = getNextSym(ans);
    string left = ans;
    if (type != token_identifier) error();
    checkVariableExist(ans);//赋值语句左边的变量是未定义的变量
    checkAssignable(ans);//赋值左端的变量是常量不可改变
    type = getNextSym(ans);
    if (type != token_assign) error();
    string right;
    expressionParser(right, parentNode);
    addAssignParser(left, right, parentNode);//初始化语句
    addCodeLabelParser(forCycleLabel, parentNode); // 添加循环块的开始位置标签
    type = getNextSym(ans);
    if (type != token_semicolon) error();
    string condition;
    flushStreamPos(ps);//解析条件之前的文件指针位置
    conditionParser(condition, parentNode);
    reverseCondition(condition);
    addConditionalJumpParser(codeLabel, condition, parentNode); // 增加条件判断跳转语句
    type = getNextSym(ans);
    if (type != token_semicolon) error();
    type = getNextSym(ans);
    if (type != token_identifier) error();
    checkVariableExist(ans);//赋值语句左边的变量是未定义的变量
    checkAssignable(ans);//赋值左端的变量是常量不可改变
    type = getNextSym(ans);
    if (type != token_assign) error();
    type = getNextSym(ans);
    if (type != token_identifier) error();
    string opnum = ans;
    checkVariableExist(ans);//赋值语句右边的变量是未定义的变量
    checkAssignable(ans);//赋值右端的变量是常量不可改变
    type = getNextSym(ans);
    if (type != token_plus && type != token_minus) error();
    string op = ans;
    string step = to_string(stepStatementParser());
    flushStreamPos(ps);
    type = getNextSym(ans);
    if (type != token_rparent) {
        callBackStreamPos(ps);
        errorHandler(error_l);
    }
    statementParser(parentNode);
    addOperationParser(op, opnum, step, opnum, parentNode); // 步长增大语句
    addUnconditionalJumpParser(forCycleLabel, parentNode);
    addCodeLabelParser(codeLabel, parentNode);
}

void SyntaxParser::addOperationParser(string &op, string &opnum1, string &opnum2, string &ans, IRSyntaxNode *parentNode) {
    auto * kid = new IRSyntaxNode(syntax_operation_ir);
    parentNode->addKid(kid);
    operationIR(op, opnum1, opnum2, ans, kid);
}

//完成
void SyntaxParser::recycleStatementParser(IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_circulationStmt);
    parentNode->addKid(kid);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    callBackStreamPos(ps);
    if (type == token_whileToken) {//while语句
        whileRecycleStatementParser(kid);
    } else if (type == token_doToken) {//do-while语句
        doWhileRecycleStatementParser(kid);
    } else if (type == token_forToken) {//for循环语句
        forRecycleStatementParser(kid);
    } else error();
}

//完成
unsigned int SyntaxParser::stepStatementParser() {
    return unsignedIntParser();
}

//完成
void SyntaxParser::mainFuncParser(IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_mainFunction);
    parentNode->addKid(kid);
    string ans;
    TokenType type = getNextSym(ans);
    if (type != token_voidToken) error();
    type = getNextSym(ans);
    if (type != token_mainToken) error();
    string typeString = "void";
    addFunctionNameLabelParser(ans, typeString, kid);//main函数中间代码标签
    type = getNextSym(ans);
    if (type != token_lparent) error();
    auto* ps = getCurPosition();
    type = getNextSym(ans);
    if (type != token_rparent) {
        callBackStreamPos(ps);
        errorHandler(error_l);
    }
    type = getNextSym(ans);
    if (type != token_lbrace) error();
    string mainName = "main";
    addFunction(token_voidToken, mainName);//添加main函数到符号表
    compoundStatementParser(mainName, kid);
    type = getNextSym(ans);
    if (type != token_rbrace) error();
}

//完成
string SyntaxParser::stringParser() { // 字符串的解析函数
    string ans;
    TokenType  type = getNextSym(ans);
    if (type != token_strConstant) error();
    return ans;
}

//完成
int SyntaxParser::integerParser() { //整数的解析函数
    string ans;
    string intstr;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == token_plus || type == token_minus) {
        intstr += ans;
        intstr += to_string(unsignedIntParser());
    } else {
        callBackStreamPos(ps);
        intstr += to_string(unsignedIntParser());
    }
    return stoi(intstr);
}

//完成
ExprType SyntaxParser::expressionParser(string &ansSymbol, IRSyntaxNode *parentNode) {//表达式
    auto* kid = new IRSyntaxNode(syntax_expression);
    parentNode->addKid(kid);
    ExprType ansType = expr_char;//默认的表达式的返回值是字符类型的
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if ((type == token_plus || type == token_minus)) {//只要有加减运算就是int型
        ansType = expr_int;
    } else {
        callBackStreamPos(ps);
    }
    ansSymbol = applyVariableName();
    string itemSymbol;
    string blankStr;
    ExprType itemType = itemParser(itemSymbol, kid);
    if ((type == token_plus || type == token_minus)) {
        addOperationParser(ans, blankStr, itemSymbol, ansSymbol, kid);
    } else {
        addAssignParser(ansSymbol, itemSymbol, kid);
    }
    if (itemType == expr_int) {//只要项返回值是int就是int型
        ansType = expr_int;
    }
    flushStreamPos(ps);
    type = getNextSym(ans);
    while (type == token_plus || type == token_minus) {//只要有多个项就是int型
        ansType = expr_int;
        itemParser(itemSymbol, kid);
        addOperationParser(ans, ansSymbol, itemSymbol, ansSymbol, kid);
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
    return ansType;
}

//完成
ExprType SyntaxParser::itemParser(string &ansSymbol, IRSyntaxNode *parentNode) {//项
    auto* kid = new IRSyntaxNode(syntax_item);
    parentNode->addKid(kid);
    ExprType ansType = expr_char;
    ansSymbol = applyVariableName();
    string factorSymbol;
    ExprType factorType = factorParser(factorSymbol, kid);
    if (factorType == expr_int) {//只要有因子返回值是int型
        ansType = expr_int;
    }
    addAssignParser(ansSymbol, factorSymbol, kid);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    while (type == token_multiply || type == token_divide) {//只要有多个因子就是int型
        ansType = expr_int;
        factorParser(factorSymbol, kid);
        addOperationParser(ans, ansSymbol, factorSymbol, ansSymbol, kid);
        flushStreamPos(ps);
        type = getNextSym(ans);
    }
    callBackStreamPos(ps);
    return ansType;
}

//完成
ExprType SyntaxParser::factorParser(string &ansSymbol, IRSyntaxNode *parentNode) {//因子解析
    auto* kid = new IRSyntaxNode(syntax_factor);
    parentNode->addKid(kid);
    ExprType ansType = expr_char;
    string ans;
    ansSymbol.clear();
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    if (type == token_identifier) {
        checkEntryExist(ans);//检查名字是否定义
        string ans2;
        auto* ps2 = getCurPosition();
        TokenType type2 = getNextSym(ans2);
        if (type2 == token_lparent && isReturnFunc(ans)) {//有返回值函数调用的因子
            callBackStreamPos(ps);
            if (isIntReturnFunc(ans)) ansType = expr_int;//返回函数是int型
            returnFuncCallParser(ansSymbol, kid);//有返回值的函数调用
        } else {
            if (type2 == token_lbrack) {//数组
                if (isIntArray(ans)) ansType = expr_int;//只要有数组是int类型
                string arrayIndex;
                ExprType indexType = expressionParser(arrayIndex, kid);
                ansSymbol = applyVariableName();
                string op = "[]";
                addOperationParser(op, arrayIndex, ans, ansSymbol, kid);
                if (indexType == expr_char) errorHandler(error_i);//数组下表是字符类型
                auto* pss = getCurPosition();
                type = getNextSym(ans);
                if (type != token_rbrack) {
                    callBackStreamPos(pss);
                    errorHandler(error_m);
                }
            } else {
                if (isIntVar(ans)) ansType = expr_int;//只要有变量是int类型
                callBackStreamPos(ps2);
                ansSymbol = ans;
            }
        }
    } else if (type == token_lparent) {//只要是有嵌套表达式
        ansType = expr_int;
        expressionParser(ansSymbol, kid);
        auto* pss = getCurPosition();
        type = getNextSym(ans);
        if (type != token_rparent) {
            callBackStreamPos(pss);
            errorHandler(error_l);
        }
    } else if (type == token_plus || type == token_minus || type == token_intConstant) {//只要存在数字
        ansType = expr_int;
        callBackStreamPos(ps);
        ansSymbol = to_string(integerParser());
    }  else if (type == token_charConstant) {
        ansSymbol = "\'" + ans + "\'";
        return ansType;
    }
    return ansType;
}

//完成
void SyntaxParser::returnFuncCallParser(string &ansSymbol, IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_valueRetFuncCall);
    parentNode->addKid(kid);
    ansSymbol = funcCallParser(true, kid);
}

//完成
void SyntaxParser::voidFuncCallParser(IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_voidRetFuncCall);
    parentNode->addKid(kid);
    funcCallParser(false, kid);
}

//完成
string SyntaxParser::funcCallParser(bool isReturn, IRSyntaxNode *parentNode) {
    string ans;
    string ansSymbol;
    string functionName;
    TokenType type = getNextSym(functionName);
    if (type != token_identifier) error();
    checkEntryExist(functionName);//检查函数是否存在
    type = getNextSym(ans);
    if (type != token_lparent) error();
    funcCallValueListParser(functionName, parentNode);//函数调用的参数表
    if (isReturn) {
        ansSymbol = applyVariableName();//申请一个变量
        addFunctionCallParser(ansSymbol, functionName, parentNode);
    } else {
        addFunctionCallParser(functionName, parentNode);
    }
    auto* ps = getCurPosition();
    type = getNextSym(ans);
    if (type != token_rparent) {
        callBackStreamPos(ps);
        errorHandler(error_l);
    }
    return ansSymbol;
}

void SyntaxParser::addFunctionCallParser(string &retValueSymbol, string &functionName, IRSyntaxNode *parentNode) { //有返回值函数的返回语句添加
    auto * kid = new IRSyntaxNode(syntax_funcCallStmt_ir);
    parentNode->addKid(kid);
    retFunctionCallIR(retValueSymbol, functionName, kid);
}

void SyntaxParser::addFunctionCallParser(string &functionName, IRSyntaxNode *parentNode) {
    auto * kid = new IRSyntaxNode(syntax_funcCallStmt_ir);
    parentNode->addKid(kid);
    voidFunctionCallIR(functionName, kid);
}

//完成
void SyntaxParser::funcCallValueListParser(string &functionName, IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_funcCallValueTable);
    parentNode->addKid(kid);
    string ans;
    auto* ps = getCurPosition();
    TokenType type = getNextSym(ans);
    vector<ExprType> valueList;
    vector<string> paras;
    while (type != token_rparent && type != token_semicolon) {//缺少右括号的情况
        string funcPara;
        callBackStreamPos(ps);
        valueList.push_back(expressionParser(funcPara, kid));
        paras.push_back(funcPara);
        flushStreamPos(ps);
        type = getNextSym(ans);
        if (type == token_comma) {
            flushStreamPos(ps);
        }
    }
    addFuncCallRetAddrParser(functionName, kid); // 添加函数返回地址存放的中间代码
    for (auto para : paras) {
        addFuncCallValueParser(para, kid);
    }
    checkFunctionParameterList(functionName, valueList);//检查函数参数表
    callBackStreamPos(ps);
}

void SyntaxParser::addFuncCallRetAddrParser(string &functionName, IRSyntaxNode *parentNode) {
    auto *kid = new IRSyntaxNode(syntax_funcReturnAddrStore_ir);
    parentNode->addKid(kid);
    spareRetStackAddrIR(functionName, kid);
}

void SyntaxParser::addFuncCallValueParser(string &paraName, IRSyntaxNode *parentNode) {
    auto *kid = new IRSyntaxNode(syntax_funcCallValuePass_ir);
    parentNode->addKid(kid);
    functionCallParaIR(paraName, kid);
}

void SyntaxParser::addCodeLabelParser(string& label, IRSyntaxNode *parentNode) {
    auto* kid = new IRSyntaxNode(syntax_stmtLabel_ir);
    parentNode->addKid(kid);
    setDownLabelIR(label, kid);
}

void error() {
    printf("Sorry, something wrong happened, we will fix it soon...\n");
}
