#ifndef COMPILER6_1_IR_H
#define COMPILER6_1_IR_H
#include "Type.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

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

    unsigned int getBlkNum() {
        return this->blk;
    }
};

class IRGenerator{
    int forLabelnum = 0;//for循环
    int doLabelnum = 0;//do-while循环
    int whileLabelnum = 0;//while循环
    int opSymNum = 0;//变量标签
    int codeLabelnum = 0;
    vector<IRStatement>& statements;
    ofstream& irOut;

    void outputIR(string& ir);

    void generateIR(string& str,IRType type,bool isBP);

public:
    explicit IRGenerator(ofstream& outFile,vector<IRStatement>& statements) : irOut(outFile),statements(statements){}
    void partialConstDefIR(string& name, int value);
    void globalConstDefIR(string& name, int value);
    void partialVariableDefIR(string& name, unsigned int size);
    void globalVariableDefIR(string& name, unsigned int size);
    void functionDef(string& funcName);
    void functionParaDef(string& paraName);
    void functionCallPara(string& paraName);
    void read(SymbolType type, string& rName);
    void operationIR(string& op,string& opSym1,string& opSym2,string& ans);
    void assignIR(string& left,string& right);
    void retFunctionCall(string& leftSymbol, string& funcName);
    void voidFunctionCall(string& funcName);
    void valueReturn(string& name);
    void voidReturn();
    void printString(string& printStr);
    void printExpr(string& name, ExprType type);
    void printNewline();
    void conditionalJump(string& label, string& condition);
    void unconditionalJump(string& label);
    void setDownLabel(string& label);
    void spareRetStackAddr(string& funcName);
    void endProgram();
    string applyVariableName();
    string applyForCycleLabel();
    string applyDoCycleLabel();
    string applyWhileCycleLabel();
    string applyCodeBlkLabel();
};
#endif
