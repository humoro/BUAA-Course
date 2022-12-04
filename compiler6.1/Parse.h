#pragma once
#ifndef COMPILER6_1_PARSER_H
#define COMPILER6_1_PARSER_H
#include "Type.h"
#include "Signary.h"
#include "Error.h"
#include "IR.h"
#include "SyntaxTree.h"
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

    char chGet() {
        return in.get();
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
    WordParser* wordParser;
    IRGenerator* generator;
    SymbolTable* symbolTable;
    ErrorProcessor errorProcessor;
    bool existsError = false;
    bool isglobal = true;

    //----------------------------------------------syntax parser functions-------------------------------------------//
    void constDeclarationParser(SyntaxNode *parentNode);
    void constDefinitionParser(SyntaxNode *parentNode);
    void variableDeclarationParser(SyntaxNode *parentNode);
    void variableDefinitionParser(SyntaxNdoe *parentNode);
    void returnFuncDefinitionParser(SyntaxNode *parentNode);
    void voidFuncDefinitionParser(SyntaxNode *parentNode);
    void mainFuncParser(SyntaxNode *parentNode);
    int integerParser();
    unsigned int unsignedIntParser();
    void funcStatementParser(string &functionName, SyntaxNode *parentNode);
    void funcParameterListParser(SyntaxNode *parentNode);
    void compoundStatementParser(string &functionName, SyntaxNode *parentNode);
    void statementColumnParser(SyntaxNode *parentNode);
    void statementParser(SyntaxNode *parentNode);
    void assignStatementParser(SyntaxNode *parentNode);
    void conditionStatementParser(SyntaxNode *parentNode);
    void conditionParser(string &condition, SyntaxNode *parentNode);
    static void reverseCondition(string& condition);
    void whileRecycleStatementParser(SyntaxNode *parentNode);
    void doWhileRecycleStatementParser(SyntaxNode *parentNode);
    void forRecycleStatementParser(SyntaxNode *parentNode);
    void recycleStatementParser(SyntaxNode *parentNode);
    unsigned int stepStatementParser();
    void readStatementParser(SyntaxNode *parentNode);
    void printStatementParser(SyntaxNode *parentNode);
    void returnStatementParser(SyntaxNode *parentNode);
    void returnFuncCallParser(string &ansSymbol, SyntaxNode *parentNode);
    void voidFuncCallParser(SyntaxNode *parentNode);
    string stringParser();
    ExprType expressionParser(string &ansSymbol, SyntaxNode *parentNode);
    ExprType itemParser(string &ansSymbol, SyntaxNode *parentNode);
    ExprType factorParser(string &ansSymbol, SyntaxNode *parentNode);
    string funcCallParser(bool isReturn, SyntaxNode *parentNode);
    void funcCallValueListParser(string &functionName, SyntaxNode *parentNode);

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

    char get() {
        return this->wordParser->chGet();
    }

    //------------------------------------------------interCode generate function-------------------------------------------//
    void constDefIR(string& varName,int varValue) {//常量必须初始化并且变量全部是一维
        if (this->isglobal) {
            this->generator->globalConstDefIR(varName, varValue);
        } else {
            this->generator->partialConstDefIR(varName, varValue);
        }
    }

    void variableDefIR(string& varName, unsigned int size) {//变量定义的中间代码，最后的数值代表变量的规模
        if (this->isglobal) {
            this->generator->globalVariableDefIR(varName, size);
        } else {
            this->generator->partialVariableDefIR(varName, size);
        }
    }

    void spareRetStackAddrIR(string& funcName) {//在函数标签后函数参数解析之前设置一个语句来说明要在栈中预留一个位置给函数返回地址
        this->generator->spareRetStackAddr(funcName);
    }


    void functionDefIR(string& funcName) {
        this->generator->functionDef(funcName);
    }

    void functionDefParaIR(string& paraName) {
        this->generator->functionParaDef(paraName);
    }

    void functionCallParaIR(string& paraName) {
        this->generator->functionCallPara(paraName);
    }

    void retFunctionCallIR(string& leftSymbol,string& funcName) {
        this->generator->retFunctionCall(leftSymbol, funcName);
    }

    void voidFunctionCallIR(string& funcName) {
        this->generator->voidFunctionCall(funcName);
    }

    void valueReturnIR(string& symbolName) {
        this->generator->valueReturn(symbolName);
    }

    void voidReturnIR() {
        this->generator->voidReturn();
    }

    void operationIR(string& op,string& opSym1,string& opSym2,string& ans) {//运算语句
        this->generator->operationIR(op,opSym1,opSym2,ans);
    }

    void assignIR(string& left,string& right) {
        this->generator->assignIR(left,right);
    }

    void readIR(SymbolName& name) {
        this->generator->read(this->symbolTable->getSymbolType(name), name);
    }

    void printStrIR(string& str) {
        this->generator->printString(str);
    }

    void printExprIR(string& expr, ExprType type) {
        this->generator->printExpr(expr, type);
    }

    void printNewlineIR() {
        this->generator->printNewline();
    }

    void conditionalJumpIR(string& label, string& condition) {
        this->generator->conditionalJump(label,condition);
    }

    void unconditionalJumpIR(string& label) {
        this->generator->unconditionalJump(label);
    }

    void setDownLabelIR(string& label) {
        this->generator->setDownLabel(label);
    }

    void endProgram() {
        this->generator->endProgram();
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
    explicit SyntaxParser(ifstream& inFile,ofstream& irFile,SymbolTable* table,vector<IRStatement>& statements) {
        this->wordParser = new WordParser(inFile);
        this->generator = new IRGenerator(irFile,statements);
        this->symbolTable = table;
    }

    void startProgramParser();

    bool haveSyntaxError() {
        return this->existsError | this->wordParser->haveError();
    }

};

void error();
#endif
