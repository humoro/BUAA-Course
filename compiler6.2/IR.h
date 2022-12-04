#pragma once
#ifndef COMPILER6_2_IR_H
#define COMPILER6_2_IR_H
#include "Type.h"
#include "IR.h"
#include <vector>
#include <fstream>
#include <unordered_set>
using namespace std;
class IRStatement{
    string IRstr;//中间代码内容
    IRType type;
    bool isBreakPoint = false;
    unsigned int blk = 0;
public:
    explicit IRStatement(string& str,IRType type) : IRstr(str),type(type){}

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

    void setBlk(unsigned int blkNum) {
        this->blk = blkNum;
    }
};

class IRSyntaxNode{
    TokenType type;
    vector<IRSyntaxNode*> kids;
    vector<IRStatement> interCodes{}; //该语法树节点所对应生成的中间代码
public:
    explicit IRSyntaxNode(TokenType type) : type(type){
        this->kids = vector<IRSyntaxNode*>(0, nullptr);
    }

    void addKid(IRSyntaxNode* node) {
        this->kids.push_back(node);
    }

    void addInterStatement(IRStatement& stmt) {
        this->interCodes.push_back(stmt);
    }

    vector<IRSyntaxNode*> getKids() {
        return this->kids;
    }

    vector<IRStatement> getInterCodes() {
        return this->interCodes;
    }
};

class IRGenerator{
    int forLabelNum = 0;//for循环
    int doLabelNum = 0;//do-while循环
    int whileLabelNum = 0;//while循环
    int opSymNum = 0;//变量标签
    int codeLabelNum = 0;

    static void generateIR(string &str, IRType type, bool isBP, IRSyntaxNode *node);

public:
    explicit IRGenerator() = default;
    void partialConstDefIR(string &name, int value, IRSyntaxNode *node);
    void globalConstDefIR(string &name, int value, IRSyntaxNode *node);
    void partialVariableDefIR(string &name, unsigned int size, IRSyntaxNode *node);
    void globalVariableDefIR(string &name, unsigned int size, IRSyntaxNode *node);
    void functionDef(string &funcName, IRSyntaxNode *node);
    void functionParaDef(string &paraName, IRSyntaxNode *node);
    void functionCallPara(string &paraName, IRSyntaxNode *node);
    void read(SymbolType type, string &rName, IRSyntaxNode *node);
    void operationIR(string &op, string &opSym1, string &opSym2, string &ans, IRSyntaxNode *node);
    void assignIR(string &left, string &right, IRSyntaxNode *node);
    void retFunctionCall(string &leftSymbol, string &funcName, IRSyntaxNode *node);
    void voidFunctionCall(string &funcName, IRSyntaxNode *node);
    void valueReturn(string &name, IRSyntaxNode *node);
    void voidReturn(IRSyntaxNode *node);
    void printString(string &str, IRSyntaxNode *node);
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
