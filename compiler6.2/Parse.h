#pragma once
#ifndef COMPILER6_2_PARSER_H
#define COMPILER6_2_PARSER_H
#include "Type.h"
#include "Signary.h"
#include "Error.h"
#include "IR.h"
#include "IR.h"
#include <fstream>
using namespace std;

class ParsePosition{
    streampos sp;
    long long lineNumber;
public:
    explicit ParsePosition(long long line,streampos spos) {
        sp = spos;
        lineNumber = line;
    }

    long long getLineNumber() {
        return this->lineNumber;
    }

    streampos getStreamPos() {
        return this->sp;
    }

    void setLineNumber(long long line) {
        this->lineNumber = line;
    }

    void setStreamPos(streampos spos) {
        this->sp = spos;
    }

};

//词法分析函数
class WordParser{
private:
    bool existsError = false;
    long long lineNumber = 1;
    ifstream& in;
    ErrorProcessor errorProcessor;
    string currentToken;
    TokenType currentTokenType = ERROR;

    void errorHandle(ErrorType errorType) {
        this->existsError = true;
        errorProcessor.error(errorType,lineNumber);
    }
public:
    explicit WordParser(ifstream& in) : in(in){}
    void wordParser();
    void callBackStreamPos(ParsePosition* ps);
    void flushStreamPos(ParsePosition* ps);
    long long getLineNumber() {
        return this->lineNumber;
    }

    char chPeek() {
        return in.peek();
    }

    streampos getStreamPos() {
        return this->in.tellg();
    }

    string getCurrentToken() {
        return this->currentToken;
    }

    TokenType getCurrnetTokenType() {
        return this->currentTokenType;
    }

    bool haveError() {
        return this->existsError;
    }
};

//语法分析函数
class SyntaxParser{
private:
    IRSyntaxNode* root;
    WordParser* wordParser;
    IRGenerator* generator;
    SymbolTable* symbolTable;
    ErrorProcessor errorProcessor;
    bool existsError = false;
    bool isglobal = true;

    //----------------------------------------------syntax parser functions-------------------------------------------//
    void constDeclarationParser(IRSyntaxNode *parentNode);
    void constDefinitionParser(IRSyntaxNode *parentNode);
    void variableDeclarationParser(IRSyntaxNode *parentNode);
    void variableDefinitionParser(IRSyntaxNode *parentNode);
    void returnFuncDefinitionParser(IRSyntaxNode *parentNode);
    void voidFuncDefinitionParser(IRSyntaxNode *parentNode);
    void addFunctionNameLabelParser(string& functionName, IRSyntaxNode *parentNode);
    void mainFuncParser(IRSyntaxNode *parentNode);
    int integerParser();
    unsigned int unsignedIntParser();
    void funcStatementParser(string &functionName, IRSyntaxNode *parentNode);
    void funcParameterListParser(IRSyntaxNode *parentNode);
    void compoundStatementParser(string &functionName, IRSyntaxNode *parentNode);
    void statementColumnParser(IRSyntaxNode *parentNode);
    void statementParser(IRSyntaxNode *parentNode);
    void assignStatementParser(IRSyntaxNode *parentNode);
    void addAssignParser(string &left, string &right, IRSyntaxNode *parentNode);
    void conditionStatementParser(IRSyntaxNode *parentNode);
    void conditionParser(string &condition, IRSyntaxNode *parentNode);
    void addConditionalJumpParser(string &jumpLabel, string &condition, IRSyntaxNode *parentNode);
    void addUnconditionalJumpParser(string &jumpLabel, IRSyntaxNode *parentNode);
    static void reverseCondition(string& condition);
    void whileRecycleStatementParser(IRSyntaxNode *parentNode);
    void doWhileRecycleStatementParser(IRSyntaxNode *parentNode);
    void forRecycleStatementParser(IRSyntaxNode *parentNode);
    void recycleStatementParser(IRSyntaxNode *parentNode);
    unsigned int stepStatementParser();
    void readStatementParser(IRSyntaxNode *parentNode);
    void printStatementParser(IRSyntaxNode *parentNode);
    void addPrintNodeParser(string& strName, IRSyntaxNode *parentNode);
    void addPrintNodeParser(string& ansSymbol, ExprType exprType, IRSyntaxNode *parentNode);
    void addPrintNewLineParser(IRSyntaxNode *parentNode);
    void returnStatementParser(IRSyntaxNode *parentNode);
    void addVoidReturnStatementParser(IRSyntaxNode *parentNode);
    void addValueReturnParser(IRSyntaxNode *parentNode, string &retSymbol);
    void returnFuncCallParser(string &ansSymbol, IRSyntaxNode *parentNode);
    void voidFuncCallParser(IRSyntaxNode *parentNode);
    string stringParser();
    ExprType expressionParser(string &ansSymbol, IRSyntaxNode *parentNode);
    ExprType itemParser(string &ansSymbol, IRSyntaxNode *parentNode);
    ExprType factorParser(string &ansSymbol, IRSyntaxNode *parentNode);
    string funcCallParser(bool isReturn, IRSyntaxNode *parentNode);
    void addFunctionCallParser(string &retValueSymbol, string &functionName, IRSyntaxNode *parentNode);
    void addFunctionCallParser(string &functionName, IRSyntaxNode *parentNode);
    void addOperationParser(string &op, string &opnum1, string &opnum2, string &ans, IRSyntaxNode *parentNode);
    void funcCallValueListParser(string &functionName, IRSyntaxNode *parentNode);
    void addFuncCallRetAddrParser(string &functionName, IRSyntaxNode *parentNode);
    void addFuncCallValueParser(string &paraName, IRSyntaxNode *parentNode);
    void addCodeLabelParser(string& label, IRSyntaxNode *parentNode);

    //-----------------------------------------------inquire functions------------------------------------------------//
    bool entryExist(SymbolName& name) {
        return this->symbolTable->entryExist(name);
    }

    bool isVoidFunc(SymbolName& name) {
        return this->symbolTable->isVoidFunction(name);
    }

    bool isReturnFunc(SymbolName& name) {
        return this->symbolTable->isReturnFunction(name);
    }

    bool isIntReturnFunc(SymbolName& name) {
        return this->symbolTable->isIntReturnFunc(name);
    }

    bool isExistVar(SymbolName& name) {
        return this->symbolTable->variableExist(name);
    }

    bool isConstVar(SymbolName& name) {
        return this->symbolTable->isConstVariable(name);
    }

    bool isIntVar(SymbolName& name) {
        return this->symbolTable->isIntVariable(name);
    }

    bool isIntArray(SymbolName& name) {
        return this->symbolTable->isIntArray(name);
    }

    bool isCharArray(SymbolName& name) {
        return this->symbolTable->isCharArray(name);
    }

    bool isArray(SymbolName& name) {
        return isIntArray(name) || isCharArray(name);
    }


    //----------------------------------------------add symbol entry functions----------------------------------------//
    void addFunction(TokenType type, SymbolName& funcName) {
        if (this->symbolTable->addFunction(type, funcName)) return;
        errorHandler(b);
    }

    void addFunctionParameter(TokenType varType, string& varName) {
        if (this->symbolTable->addFunctionParameter(varType, varName)) return;
        errorHandler(b);
    }

    void addVariableEntry(TokenType varType, SymbolName& varName, bool isConst) {
        if (this->symbolTable->addVariableEntry(varType, varName, isConst)) return;
        errorHandler(b);
    }

    void addArrayEntry(TokenType varType, SymbolName& varName,unsigned int size) {
        if (this->symbolTable->addArrayEntry(varType, varName,size)) return;
        errorHandler(b);
    }

    void addFunctionReturn(ExprType type) {
        checkFunctionReturn(type);
        this->symbolTable->getCurrentFunction()->addReturnStatement();
    }

    //------------------------------------------------check functions-------------------------------------------------//
    void checkFunctionReturn(ExprType type) {
        Function* curFunction = this->symbolTable->getCurrentFunction();
        SymbolType sType = curFunction->getType();
        if (type == None && (sType == intReturnFunc || sType == charReturnFunc)) {
            errorHandler(h);
            printf("(function name is: %s)\n\n",curFunction->getName().c_str());
        }
        if (type == IntExp && (sType == charReturnFunc)) {
            errorHandler(h);
            printf("(function name is: %s)\n\n",curFunction->getName().c_str());
        }
        if (type == CharExp && (sType == intReturnFunc)) {
            errorHandler(h);
            printf("(function name is: %s)\n\n",curFunction->getName().c_str());
        }
        if ((type == IntExp || type == CharExp) && sType == voidFunc) {
            errorHandler(g);
            printf("(function name is: %s)\n\n",curFunction->getName().c_str());
        }
    }

    void checkReturnFunctionReturn() {
        auto* function = this->symbolTable->getCurrentFunction();
        if (function->getType() != intReturnFunc && function->getType() != charReturnFunc) return;
        if ((!function->haveReturnStatement() &&
            (function->getType() == intReturnFunc || function->getType() == charReturnFunc))) {
            errorHandler(h);//返回函数没有返回语句
            printf("(function name is: %s)\n\n",function->getName().c_str());
        }
    }

    void checkFunctionParameterList(string& functionName, vector<ExprType>& valueList) {
        vector<SymbolTableEntry*> paras;
        if (!this->symbolTable->getFunctionParaList(functionName,paras)) return;//获取函数参数列表失败
        if (paras.size() != valueList.size()) {//发现参数不匹配
            errorHandler(d);
            return;
        }
        for (unsigned long long i = 0;i < paras.size();i++) {
            if (!((valueList[i] == IntExp && paras[i]->getType() == plantIntVar) ||
                  (valueList[i] == CharExp && paras[i]->getType() == plantCharVar))) {
                errorHandler(e);//参数类型不匹配
                return;
            }
        }
    }

    void checkEntryExist(SymbolName& name) {
        if (!entryExist(name)) {
            errorHandler(c);
            printf("(undefined variable name is %s)\n\n",name.c_str());
        }
    }

    void checkVariableExist(SymbolName& name) {
        if (!isExistVar(name)) {
            errorHandler(c);
            printf("(undefined variable name is %s)\n\n",name.c_str());
        }
    }

    void checkAssignable(SymbolName& name) {//检查是不是给不可赋值的变量赋值(const检查)
        if (isConstVar(name)) {
            errorHandler(j);
            printf("(const variable name is %s)\n\n",name.c_str());
        }
    }

    void checkPlantVariable(SymbolName& name) {
        if ((isVoidFunc(name) || isReturnFunc(name) || isConstVar(name) || isArray(name))) {
            errorHandler(p);
        }
    }

    //---------------------------------------file stream position operation functions---------------------------------//
    void callBackStreamPos(ParsePosition* ps) {
        this->wordParser->callBackStreamPos(ps);
    }

    void flushStreamPos(ParsePosition* ps) {
        this->wordParser->flushStreamPos(ps);
    }

    long long getLineNumber() {
        return this->wordParser->getLineNumber();
    }

    ParsePosition* getCurPosition() {//返回当前的读取指针的位置
        return (new ParsePosition(this->getLineNumber(),this->wordParser->getStreamPos()));
    }

    //-------------------------------------------symbol handle operation functions------------------------------------//
    string getCurrentToken() {
        return this->wordParser->getCurrentToken();
    }

    TokenType getCurrentTokenType() {
        return this->wordParser->getCurrnetTokenType();
    }

    TokenType getNextSym(string& str) {
        this->wordParser->wordParser();
        str = getCurrentToken();
        return getCurrentTokenType();
    }

    char peek() {
        return this->wordParser->chPeek();
    }
    //------------------------------------------------interCode generate function-------------------------------------------//
    void constDefIR(string &varName, int varValue, IRSyntaxNode *node) {//常量必须初始化并且变量全部是一维
        if (this->isglobal) {
            this->generator->globalConstDefIR(varName, varValue, node);
        } else {
            this->generator->partialConstDefIR(varName, varValue, node);
        }
    }

    void variableDefIR(string &varName, unsigned int size, IRSyntaxNode *node) {//变量定义的中间代码，最后的数值代表变量的规模
        if (this->isglobal) {
            this->generator->globalVariableDefIR(varName, size, node);
        } else {
            this->generator->partialVariableDefIR(varName, size, node);
        }
    }

    void spareRetStackAddrIR(string &funcName, IRSyntaxNode *node) {//在函数标签后函数参数解析之前设置一个语句来说明要在栈中预留一个位置给函数返回地址
        this->generator->spareRetStackAddr(funcName, node);
    }


    void functionDefIR(string &funcName, IRSyntaxNode *node) {
        this->generator->functionDef(funcName, node);
    }

    void functionDefParaIR(string &paraName, IRSyntaxNode *node) {
        this->generator->functionParaDef(paraName, node);
    }

    void functionCallParaIR(string &paraName, IRSyntaxNode *node) {
        this->generator->functionCallPara(paraName, node);
    }

    void retFunctionCallIR(string &leftSymbol, string &funcName, IRSyntaxNode *node) {
        this->generator->retFunctionCall(leftSymbol, funcName, node);
    }

    void voidFunctionCallIR(string &funcName, IRSyntaxNode *node) {
        this->generator->voidFunctionCall(funcName, node);
    }

    void valueReturnIR(string &symbolName, IRSyntaxNode *node) {
        this->generator->valueReturn(symbolName, node);
    }

    void voidReturnIR(IRSyntaxNode *node) {
        this->generator->voidReturn(node);
    }

    void operationIR(string &op, string &opSym1, string &opSym2, string &ans, IRSyntaxNode *node) {//运算语句
        this->generator->operationIR(op, opSym1, opSym2, ans, node);
    }

    void assignIR(string &left, string &right, IRSyntaxNode *node) {
        this->generator->assignIR(left, right, node);
    }

    void readIR(SymbolName &name, IRSyntaxNode *node) {
        this->generator->read(this->symbolTable->getSymbolType(name), name, node);
    }

    void printStrIR(string &str, IRSyntaxNode *node) {
        this->generator->printString(str, node);
    }

    void printExprIR(string &expr, ExprType type, IRSyntaxNode *node) {
        this->generator->printExpr(expr, type, node);
    }

    void printNewlineIR(IRSyntaxNode *node) {
        this->generator->printNewline(node);
    }

    void conditionalJumpIR(string &label, string &condition, IRSyntaxNode *node) {
        this->generator->conditionalJump(label, condition, node);
    }

    void unconditionalJumpIR(string &label, IRSyntaxNode *node) {
        this->generator->unconditionalJump(label, node);
    }

    void setDownLabelIR(string &label, IRSyntaxNode *node) {
        this->generator->setDownLabel(label, node);
    }

    void endProgram(IRSyntaxNode *node) {
        this->generator->endProgram(node);
    }

    string applyVariableName() {
        return this->generator->applyVariableName();
    }

    string applyStringName(string& printStr) {
        return this->symbolTable->applyStringName(printStr);
    }

    string applyForCycleLabel() {
        return this->generator->applyForCycleLabel();
    }

    string applyDoCycleLabel() {
        return this->generator->applyDoCycleLabel();
    }

    string applyWhileCycleLabel() {
        return this->generator->applyWhileCycleLabel();
    }

    string applyCodeBlkLabel() {
        return this->generator->applyCodeBlkLabel();
    }
    //------------------------------------------------error handle function-------------------------------------------//
    void errorHandler(ErrorType errorType) {
        this->existsError = true;
        this->errorProcessor.error(errorType,this->wordParser->getLineNumber());
    }


public:
    explicit SyntaxParser(ifstream& inFile, SymbolTable* table) {
        this->generator = new IRGenerator();
        this->wordParser = new WordParser(inFile);
        this->symbolTable = table;
        this->root = new IRSyntaxNode(Non_Program);
    }

    void startProgramParser();

    bool haveSyntaxError() {
        return this->existsError | this->wordParser->haveError();
    }

    IRSyntaxNode* irNode() {
        return this->root;
    }

};

void error();
#endif
