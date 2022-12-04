#ifndef COMPILER4_1_PARSER_H
#define COMPILER4_1_PARSER_H
#include "Type.h"
#include "Signary.h"
#include "Error.h"
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
    SymbolTable symbolTable;
    ErrorProcessor errorProcessor;
    bool existsError = false;

    //----------------------------------------------syntax parser functions-------------------------------------------//
    void constDeclarationParser();
    void constDefinitionParser();
    void variableDeclarationParser();
    void variableDefinitionParser();
    void returnFuncDefinitionParser();
    void voidFuncDefinitionParser();
    void mainFuncParser();
    void integerParser();
    void unsignedIntParser();
    void funcStatementParser(string& functionName);
    void funcParameterListParser();
    void compoundStatementParser(string& functionName);
    void statementColumnParser();
    void statementParser();
    void assignStatementParser();
    void conditionStatementParser();
    void conditionParser();
    void whileRecycleStatementParser();
    void doWhileRecycleStatementParser();
    void forRecycleStatementParser();
    void recycleStatementParser();
    void stepStatementParser();
    void readStatementParser();
    void printStatementParser();
    void returnStatementParser();
    void returnFuncCallParser();
    void voidFuncCallParser();
    void stringParser();
    ExprType expressionParser();
    ExprType itemParser();
    ExprType factorParser();
    void funcCallParser();
    void funcCallValueListParser(string& functionName);

    //-----------------------------------------------inquire functions------------------------------------------------//
    bool entryExist(SymbolName& name) {
        return this->symbolTable.entryExist(name);
    }

    bool isReturnFunc(SymbolName& name) {
        return this->symbolTable.isReturnFunc(name);
    }

    bool isIntReturnFunc(SymbolName& name) {
        return this->symbolTable.isIntReturnFunc(name);
    }

    bool isExistVar(SymbolName& name) {
        return this->symbolTable.isExistVar(name);
    }

    bool isConstVar(SymbolName& name) {
        return this->symbolTable.isConstVar(name);
    }

    bool isIntVar(SymbolName& name) {
        return this->symbolTable.isIntVar(name);
    }

    bool isIntArray(SymbolName& name) {
        return this->symbolTable.isIntArray(name);
    }


    //----------------------------------------------add symbol entry functions----------------------------------------//
    void addFunction(TokenType type, SymbolName& funcName) {
        if (this->symbolTable.addFunction(type, funcName)) return;
        errorHandle(b);
    }

    void addFunctionParameter(TokenType varType, string& varName) {
        if (this->symbolTable.addFunctionParameter(varType, varName)) return;
        errorHandle(b);
    }

    void addVariableEntry(TokenType varType, SymbolName& varName, bool isConst) {
        if (this->symbolTable.addVariableEntry(varType, varName, isConst)) return;
        errorHandle(b);
    }

    void addArrayEntry(TokenType varType, SymbolName& varName) {
        if (this->symbolTable.addArrayEntry(varType, varName)) return;
        errorHandle(b);
    }

    void addFunctionReturn(ExprType type) {
        checkFunctionReturn(type);
        this->symbolTable.getCurrentFunction()->addReturnSatement();
    }

    //------------------------------------------------check functions-------------------------------------------------//
    void checkFunctionReturn(ExprType type) {
        FunctionEntry* curFunction = this->symbolTable.getCurrentFunction();
        SymbolType sType = curFunction->getType();
        if (type == None && (sType == intReturnFunc || sType == charReturnFunc)) {
            errorHandle(h);
            printf("(function name is: %s)\n\n",curFunction->getName().c_str());
        }
        if (type == IntExp && (sType == charReturnFunc)) {
            errorHandle(h);
            printf("(function name is: %s)\n\n",curFunction->getName().c_str());
        }
        if (type == CharExp && (sType == intReturnFunc)) {
            errorHandle(h);
            printf("(function name is: %s)\n\n",curFunction->getName().c_str());
        }
        if ((type == IntExp || type == CharExp) && sType == voidFunc) {
            errorHandle(g);
            printf("(function name is: %s)\n\n",curFunction->getName().c_str());
        }
    }

    void checkReturnFunctionReturn() {
        auto* function = this->symbolTable.getCurrentFunction();
        if (function->getType() != intReturnFunc && function->getType() != charReturnFunc) return;
        if ((!function->haveReturnStatement() &&
            (function->getType() == intReturnFunc || function->getType() == charReturnFunc))) {
            errorHandle(h);//返回函数没有返回语句
            printf("(function name is: %s)\n\n",function->getName().c_str());
        }
    }

    void checkFunctionParameterList(string& functionName, vector<ExprType>& valueList) {
        vector<SymbolTableEntry*> paras;
        if (!this->symbolTable.getFunctionParaList(functionName,paras)) return;//获取函数参数列表失败
        if (paras.size() != valueList.size()) {//发现参数不匹配
            errorHandle(d);
            return;
        }
        for (unsigned long long i = 0;i < paras.size();i++) {
            if (!((valueList[i] == IntExp && paras[i]->getType() == plantIntVar) ||
                  (valueList[i] == CharExp && paras[i]->getType() == plantCharVar))) {
                errorHandle(e);//参数类型不匹配
                return;
            }
        }
    }

    void checkEntryExist(SymbolName& name) {
        if (!entryExist(name)) {
            errorHandle(c);
            printf("(undefined variable name is %s)\n\n",name.c_str());
        }
    }

    void checkVariableExist(SymbolName& name) {
        if (!isExistVar(name)) {
            errorHandle(c);
            printf("(undefined variable name is %s)\n\n",name.c_str());
        }
    }

    void checkAssignable(SymbolName& name) {//检查是不是给不可赋值的变量赋值(const检查)
        if (isConstVar(name)) {
            errorHandle(j);
            printf("(const variable name is %s)\n\n",name.c_str());
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

    ParsePosition* getCurPostion() {//返回当前的读取指针的位置
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

    //------------------------------------------------error handle function-------------------------------------------//
    void errorHandle(ErrorType errorType) {
        this->existsError = true;
        this->errorProcessor.error(errorType,this->wordParser->getLineNumber());
    }


public:
    explicit SyntaxParser(ifstream& inFile) {
        this->wordParser = new WordParser(inFile);
    }
    void startProgramParser();

    bool haveError() {
        return this->existsError | this->wordParser->haveError();
    }

};

void error();
#endif //COMPILER3_2_PARSERS_H
