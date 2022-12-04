#pragma once
#ifndef COMPILER7_2_IR_H
#define COMPILER7_2_IR_H
#include "Type.h"
#include "Signary.h"
#include "StrTNum.h"
#include <utility>
#include <vector>
#include <fstream>
#include <unordered_set>
#include <algorithm>
using namespace std;
class IRStatement{
    string IRstr;//中间代码内容
    IRType type;
    bool isBreakPoint = false;
    unsigned int code = 0;
    unordered_set<unsigned int> gen;
    unordered_set<unsigned int> kill;
public:
    IRStatement(string& str,IRType type) : IRstr(str),type(type){}

    IRStatement(string& str,IRType type, unsigned int number) : IRstr(str),type(type),code(number){}

    IRStatement(IRStatement* stmt,string& ir) {
        this->IRstr = ir;
        this->type = stmt->type;
        this->code = stmt->code;
    }

    IRStatement(IRStatement* stmt,unsigned int code) {
        this->IRstr = stmt->IRstr;
        this->type = stmt->type;
        this->code = code;
    }

    IRStatement(IRStatement& stmt,unsigned int code) {
        this->IRstr = stmt.IRstr;
        this->type = stmt.type;
        this->code = code;
    }

    bool operator== (IRStatement* stmt) {
        return this->type == stmt->getIRType() && this->IRstr == stmt->getIR();
    }

    void setBreakPoint(){
        this->isBreakPoint = true;
    }

    bool isBPStatement() {
        return this->isBreakPoint;
    }

    IRType getIRType() {
        return this->type;
    }

    string getIR() {
        return this->IRstr;
    }

    void setCodeNumber(unsigned int number) {
        this->code = number;
    }

    unsigned int getCodeNumber() {
        return this->code;
    }

    void clearDataFlow() {
        this->gen.clear();
        this->kill.clear();
    }

    void addGen(unsigned int node) {
        this->gen.insert(node);
    }

    void addKill(unsigned int node) {
        this->kill.insert(node);
    }

    unordered_set<unsigned int> getGen() {
        return this->gen;
    }

    unordered_set<unsigned int> getKill() {
        return this->kill;
    }
};

class IRSyntaxNode{
    SyntaxType type;
    vector<IRSyntaxNode*> kids;
    vector<IRStatement*> interCodes{}; //该语法树节点所对应生成的中间代码
public:
    explicit IRSyntaxNode(SyntaxType type) : type(type){
        this->kids = vector<IRSyntaxNode*>(0, nullptr);
    }

    void addKid(IRSyntaxNode* node) {
        this->kids.push_back(node);
    }

    void addInterStatement(IRStatement* stmt) {
        this->interCodes.push_back(stmt);
    }

    vector<IRSyntaxNode*> getKids() {
        return this->kids;
    }

    vector<IRStatement*> getInterCodes() {
        return this->interCodes;
    }
};

class IRGenerator{
    int forLabelNum = 0;//for循环
    int doLabelNum = 0;//do-while循环
    int whileLabelNum = 0;//while循环
    int opSymNum = 0;//变量标签
    int codeLabelNum = 0;
    const string partialConstDef = "@const ";
    const string partialVariableDef = "@var ";
    const string globalConstDef = "@const ";
    const string globalVariableDef = "@var ";
    const string functionDefParaIR = "@paraDef ";
    const string functionCallParaIR = "@pushPara ";
    const string readIR = "@input@";
    const string retFuncCallIR = "@call@retFunc ";
    const string voidFuncCallIR = "@call@voidFunc ";
    const string voidFuncReturnIR = "@ret@void ";
    const string returnFuncReturnIR = "@ret@value ";
    const string printIR = "@print@";
    const string varLabel = "var@";
    const string forLabel = "__for";
    const string doLabel = "__do";
    const string whileLabel = "__while";
    const string codeLabel = "__codeBlk";
    const string spareRetAddr = "@para@retaddr";

    static void generateIR(string &str, IRType type, bool isBP, IRSyntaxNode *node);

public:
    explicit IRGenerator() = default;
    void partialConstDefIR(string &name, string &typeString, int value, IRSyntaxNode *node);
    void globalConstDefIR(string &name, string &typeString, int value, IRSyntaxNode *node);
    void partialVariableDefIR(string &name, string &typeString, unsigned int size, IRSyntaxNode *node);
    void globalVariableDefIR(string &name, string &typeString, unsigned int size, IRSyntaxNode *node);
    void functionDef(string &funcName, string &typeString, IRSyntaxNode *node);
    void functionParaDef(string &paraName, string &typeString, IRSyntaxNode *node);
    void functionCallPara(string &paraName, IRSyntaxNode *node);
    void read(SymbolType type, string &rName, IRSyntaxNode *node);
    void operationIR(string &op, string &opSym1, string &opSym2, string &ans, IRSyntaxNode *node);
    void assignIR(string &left, string &right, IRSyntaxNode *node);
    void retFunctionCall(string &leftSymbol, string &funcName, IRSyntaxNode *node);
    void voidFunctionCall(string &funcName, IRSyntaxNode *node);
    void valueReturn(string &name, IRSyntaxNode *node);
    void voidReturn(IRSyntaxNode *node);
    void printString(string &str, string &content, IRSyntaxNode *node);
    void printExpr(string &name, ExprType type, IRSyntaxNode *node);
    void printNewline(IRSyntaxNode *node);
    void conditionalJump(string &label, string &condition, IRSyntaxNode *node);
    void unconditionalJump(string &label, IRSyntaxNode *node);
    void setDownLabel(string &label, IRSyntaxNode *node);
    void spareRetStackAddr(string &funcName, IRSyntaxNode *node);
    void endProgram(IRSyntaxNode *node);
    string applyVariableName();
    string applyForCycleLabel();
    string applyDoCycleLabel();
    string applyWhileCycleLabel();
    string applyCodeBlkLabel();
};
#endif
