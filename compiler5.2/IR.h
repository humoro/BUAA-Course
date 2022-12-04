#ifndef COMPILER5_2_IR_H
#define COMPILER5_2_IR_H
#include "Type.h"
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
class IRGenerator{
    int conditionLabelnum = 0;//条件语句标签
    int forLabelnum = 0;//for循环
    int doLabelnum = 0;//do-while循环
    int whileLabelnum = 0;//while循环
    int opSymNum = 0;//变量标签
    int codeLabelnum = 0;
    ofstream& irOut;

    void outputIR(string& ir);

public:
    explicit IRGenerator(ofstream& outFile) : irOut(outFile){}

    void constDefIR(string& name, int value);

    void globalConstDefIR(string& name, int value);

    void variableDefIR(string& name, unsigned int size);

    void globalVariableDefIR(string& name, unsigned int size);

    void functionDef(string& funcName);

    void functionDefPara(string& paraName);

    void functionCallPara(string& paraName);

    void readStatIR(SymbolType type, string& rName);

    void operationIR(string& op,string& opSym1,string& opSym2,string& ans);

    void assignIR(string& left,string& right);

    void returnFuncCall(string& leftSymbol, string& funcName);

    void voidFuncCall(string& funcName);

    void retFuncReturn(string& name);

    void voidFuncReturn();

    void printStrIR(string& printStr);

    void printSymbolIR(string& name);

    void conditionIR(string& label,string& condition);

    void gotoIR(string& label);

    void setLabel(string& label);

    string applyOperationSymbol();

    string applyForCycleLabel();

    string applyDoCycleLabel();

    string applyWhileCycleLabel();

    string applyCodeLabel();

};
#endif
