#include "IR.h"
#include "Type.h"
#include <sstream>
using namespace std;

void IRGenerator::generateIR(string &str, IRType type, bool isBP, IRSyntaxNode *node) {
    IRStatement stmt(str, type);
    if (isBP) stmt.setBreakPoint();
    node->addInterStatement(stmt);
}

void IRGenerator::globalVariableDefIR(string &name,string &typeString, unsigned int size, IRSyntaxNode *node) {
    string ir(this->globalVariableDef + typeString + " " + name + " " + to_string(size));
    generateIR(ir, gVarDef, false, node);
}

void IRGenerator::globalConstDefIR(string &name, string &typeString, int value, IRSyntaxNode *node) {
    string ir(this->globalConstDef + typeString + " " + name + " = " + to_string(value));
    generateIR(ir, gConstDef, false, node);
}

void IRGenerator::partialConstDefIR(string &name, string &typeString, int value, IRSyntaxNode *node) {
    string ir(this->partialConstDef + typeString + " " + name + " = " + to_string(value));
    generateIR(ir, pConstDef, false, node);
}

void IRGenerator::partialVariableDefIR(string &name, string &typeString, unsigned int size, IRSyntaxNode *node) {
    string ir(this->partialVariableDef + typeString + " " + name + " " + to_string(size));
    generateIR(ir, pVarDef, false, node);
}

void IRGenerator::functionDef(string &funcName, string &typeString, IRSyntaxNode *node) {
    string label(typeString + " " + funcName + ":");
    generateIR(label, funcDefLabel, true, node);
    label = this->spareRetAddr;
    generateIR(label, funcRetAddrSw, false, node);
}

void IRGenerator::retFunctionCall(string &leftSymbol, string &funcName, IRSyntaxNode *node) {
    string ir(this->retFuncCallIR + funcName + " " + leftSymbol);
    generateIR(ir, retFuncCall, false, node);
}

void IRGenerator::voidFunctionCall(string &funcName, IRSyntaxNode *node) {
    string ir(this->voidFuncCallIR + funcName);
    generateIR(ir, nonRetFuncCall, false, node);
}

void IRGenerator::functionParaDef(string &paraName, string &typeString,  IRSyntaxNode *node) {
    string para(this->functionDefParaIR + typeString + " " + paraName);
    generateIR(para, funcDefPara, false, node);
}


void IRGenerator::functionCallPara(string &paraName, IRSyntaxNode *node) {
    string para(this->functionCallParaIR + paraName);
    generateIR(para, funcCallPara, false, node);
}


void IRGenerator::operationIR(string &op, string &opSym1, string &opSym2, string &ans, IRSyntaxNode *node) {
    string ir(op + "," + opSym1 + "," + opSym2 + "," + ans);
    generateIR(ir, operation, false, node);
}

void IRGenerator::assignIR(string &left, string &right, IRSyntaxNode *node) {
    string ir(left + " = " + right);
    generateIR(ir, assign, false, node);
}

void IRGenerator::valueReturn(string &name, IRSyntaxNode *node) {
    string ir(this->returnFuncReturnIR + name);
    generateIR(ir, valueRet, true, node);
}

void IRGenerator::voidReturn(IRSyntaxNode *node) {
    string ir(this->voidFuncReturnIR);
    generateIR(ir, voidRet, true, node);
}

void IRGenerator::read(SymbolType type, string &rName, IRSyntaxNode *node) {
    string typeStr;
    IRType irType;
    if (type == plantCharVar) {
        typeStr = "char";
        irType = readChar;
    } else if (type == plantIntVar) {
        typeStr = "int";
        irType = readInt;
    } else {
        typeStr = "error";
        irType = empty;
    }
    string ir(this->readIR + typeStr + " " + rName);
    generateIR(ir, irType, false, node);
}

void IRGenerator::printString(string &str, string &content, IRSyntaxNode *node) {
    string ir(this->printIR + "str " + str + " \"" + content + "\"");
    generateIR(ir, printStr, false, node);
}

void IRGenerator::printExpr(string &name, ExprType type, IRSyntaxNode *node) {
    string ir(this->printIR);
    IRType printType = printStr;
    if (type == IntExp) {
        ir += "int";
        printType = printInt;
    } else if (type == CharExp) {
        ir += "char";
        printType = printChar;
    }
    ir += " " + name;
    generateIR(ir, printType, false, node);
}

void IRGenerator::printNewline(IRSyntaxNode *node) {
    string ir("@print@newline");
    generateIR(ir, printNewLine, false, node);
}

void IRGenerator::conditionalJump(string &label, string &condition, IRSyntaxNode *node) {
    IRType type = geqJump;
    string bge = "@bge";//对应大于等于转移指令
    string ble = "@ble";//小于等于
    string beq = "@beq";//等于转移
    string bne = "@bne";//不等于转移
    string bgt = "@bgt";//大于转移
    string blt = "@blt";//小于转移
    stringstream ss(condition);
    string left,right,op;
    getline(ss,left,' ');
    getline(ss,op,' ');
    getline(ss,right,' ');
    string ir;
    if (op == ">=") ir += bge,type = geqJump;
    if (op == ">") ir += bgt,type = greJump;
    if (op == "<=") ir += ble,type = leqJump;
    if (op == "<") ir += blt,type = lesJump;
    if (op == "==") ir += beq,type = eqlJump;
    if (op == "!=") ir += bne,type = neqJmp;
    ir += " " + left + " " + right + " " + label;
    generateIR(ir, type, true, node);
}

void IRGenerator::unconditionalJump(string &label, IRSyntaxNode *node) {
    string ir("@jumpTo " + label);
    generateIR(ir, jump, true, node);
}

void IRGenerator::setDownLabel(string &label, IRSyntaxNode *node) {
    string ir(label + ":");
    generateIR(ir, statLabel, true, node);
}

void IRGenerator::spareRetStackAddr(string &funcName, IRSyntaxNode *node) {
    string ir("@funcCallBegin: " + funcName);
    generateIR(ir, funcCallBegin, false, node);
}

void IRGenerator::endProgram(IRSyntaxNode *node) {
    string ir("__end:");
    generateIR(ir, funcDefLabel, false, node);
}

string IRGenerator::applyVariableName() {
    return string(this->varLabel + to_string(this->opSymNum++));
}

string IRGenerator::applyForCycleLabel() {
    return string(this->forLabel + to_string(this->forLabelNum++));

}

string IRGenerator::applyDoCycleLabel() {
    return string(this->doLabel + to_string(this->doLabelNum++));
}

string IRGenerator::applyWhileCycleLabel() {
    return string(this->whileLabel + to_string(this->whileLabelNum++));
}

string IRGenerator::applyCodeBlkLabel() {
    string ans(this->codeLabel + to_string(this->codeLabelNum++));
    return ans;
}