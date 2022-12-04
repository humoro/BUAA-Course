#include "IR.h"
#include "Type.h"
#include <sstream>
#include <fstream>
using namespace std;
const static string partialConstDef = "@const ";
const static string partialVariableDef = "@var ";
const static string globalConstDef = "@const ";
const static string globalVariableDef = "@var ";
const static string functionDefParaIR = "@paraDef ";
const static string functionCallParaIR = "@pushPara ";
const static string readIR = "@read@";
const static string retFuncCallIR = "@call@retFunc ";
const static string voidFuncCallIR = "@call@voidFunc ";
const static string voidFuncReturnIR = "@ret@void ";
const static string returnFuncReturnIR = "@ret@value ";
const static string printIR = "@print@";
const static string varLabel = "var@";
const static string forLabel = "__for";
const static string doLabel = "__do";
const static string whileLabel = "__while";
const static string codeLabel = "__codeBlk";
const static string spareRetAddr = "@para@retaddr";

void IRGenerator::generateIR(string &str, IRType type, bool isBP, IRSyntaxNode *node) {
    IRStatement stmt(str, type);
    if (isBP) stmt.setBreakPoint();
    node->addInterStatement(stmt);
}

void IRGenerator::globalVariableDefIR(string &name, unsigned int size, IRSyntaxNode *node) {
    string ir(globalVariableDef + name + " " + to_string(size));
    generateIR(ir, gVarDef, false, node);
}

void IRGenerator::globalConstDefIR(string &name, int value, IRSyntaxNode *node) {
    string ir(globalConstDef + name + " = " + to_string(value));
    generateIR(ir, gConstDef, false, node);
}

void IRGenerator::partialConstDefIR(string &name, int value, IRSyntaxNode *node) {
    string ir(partialConstDef + name + " = " + to_string(value));
    generateIR(ir, pConstDef, true, node);
}

void IRGenerator::partialVariableDefIR(string &name, unsigned int size, IRSyntaxNode *node) {
    string ir(partialVariableDef + name + " " + to_string(size));
    generateIR(ir, pVarDef, true, node);
}

void IRGenerator::functionDef(string &funcName, IRSyntaxNode *node) {
    string label(funcName + ":");
    generateIR(label, funcDefLabel, true, node);
    label = spareRetAddr;
    generateIR(label, funcRetAddrSw, false, node);
}

void IRGenerator::retFunctionCall(string &leftSymbol, string &funcName, IRSyntaxNode *node) {
    string ir(retFuncCallIR + funcName + " " + leftSymbol);
    generateIR(ir, retFuncCall, false, node);
}

void IRGenerator::voidFunctionCall(string &funcName, IRSyntaxNode *node) {
    string ir(voidFuncCallIR + funcName);
    generateIR(ir, nonRetFuncCall, false, node);
}

void IRGenerator::functionParaDef(string &paraName, IRSyntaxNode *node) {
    string para(functionDefParaIR + paraName);
    generateIR(para, funcDefPara, false, node);
}


void IRGenerator::functionCallPara(string &paraName, IRSyntaxNode *node) {
    string para(functionCallParaIR + paraName);
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
    string ir(returnFuncReturnIR + name);
    generateIR(ir, valueRet, true, node);
}

void IRGenerator::voidReturn(IRSyntaxNode *node) {
    string ir(voidFuncReturnIR);
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
    string ir(readIR + typeStr + " " + rName);
    generateIR(ir, irType, false, node);
}

void IRGenerator::printString(string &str, IRSyntaxNode *node) {
    string ir(printIR + "str " + str);
    generateIR(ir, printStr, false, node);
}

void IRGenerator::printExpr(string &name, ExprType type, IRSyntaxNode *node) {
    string ir(printIR);
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
    return string(varLabel + to_string(this->opSymNum++));
}

string IRGenerator::applyForCycleLabel() {
    return string(forLabel + to_string(this->forLabelNum++));

}

string IRGenerator::applyDoCycleLabel() {
    return string(doLabel + to_string(this->doLabelNum++));
}

string IRGenerator::applyWhileCycleLabel() {
    return string(whileLabel + to_string(this->whileLabelNum++));
}

string IRGenerator::applyCodeBlkLabel() {
    string ans(codeLabel + to_string(this->codeLabelNum++));
    return ans;
}