#include "IR.h"
#include "Type.h"
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

void IRGenerator::generateIR(string& str,IRType type,bool isBP) {
    this->statements.emplace_back(str,type);
    if (isBP) {
        this->statements.back().setBreakPoint();
    }
}

void IRGenerator::outputIR(string &ir) {
    this->irOut << ir << endl;
}

void IRGenerator::globalVariableDefIR(string &name, unsigned int size) {
    string ir(globalVariableDef + name + " " + to_string(size));
    generateIR(ir,gVarDef,false);
    outputIR(ir);
}

void IRGenerator::globalConstDefIR(string &name, int value) {
    string ir(globalConstDef + name + " = " + to_string(value));
    generateIR(ir,gConstDef,false);
    outputIR(ir);
}

void IRGenerator::partialConstDefIR(string& name, int value) {
    string ir(partialConstDef + name + " = " + to_string(value));
    generateIR(ir,pConstDef,true);
    outputIR(ir);
}

void IRGenerator::partialVariableDefIR(string& name, unsigned int size) {
    string ir(partialVariableDef + name + " " + to_string(size));
    generateIR(ir,pVarDef,true);
    outputIR(ir);
}

void IRGenerator::functionDef(string& funcName) {
    string label(funcName + ":");
    generateIR(label,funcDefLabel,true);
    outputIR(label);
    label = spareRetAddr;
    generateIR(label,funcRetAddrSw,false);
    outputIR(label);
}

void IRGenerator::retFunctionCall(string &leftSymbol, string &funcName) {
    string ir(retFuncCallIR + funcName + " " + leftSymbol);
    generateIR(ir,retFuncCall, false);
    outputIR(ir);
}

void IRGenerator::voidFunctionCall(string &funcName) {
    string ir(voidFuncCallIR + funcName);
    generateIR(ir,nonRetFuncCall,false);
    outputIR(ir);
}

void IRGenerator::functionParaDef(string &paraName) {
    string para(functionDefParaIR + paraName);
    generateIR(para,funcDefPara,false);
    outputIR(para);
}


void IRGenerator::functionCallPara(string &paraName) {
    string para(functionCallParaIR + paraName);
    generateIR(para,funcCallPara,false);
    outputIR(para);
}


void IRGenerator::operationIR(string &op, string &opSym1, string &opSym2,string& ans) {
    string ir(op + "," + opSym1 + "," + opSym2 + "," + ans);
    generateIR(ir,operation,false);
    outputIR(ir);
}

void IRGenerator::assignIR(string &left, string &right) {
    string ir(left + " = " + right);
    generateIR(ir,assign,false);
    outputIR(ir);
}

void IRGenerator::valueReturn(string &name) {
    string ir(returnFuncReturnIR + name);
    generateIR(ir,valueRet,true);
    outputIR(ir);
}

void IRGenerator::voidReturn() {
    string ir(voidFuncReturnIR);
    generateIR(ir,voidRet,true);
    outputIR(ir);
}

void IRGenerator::read(SymbolType type, string &rName) {
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
    generateIR(ir,irType,false);
    outputIR(ir);
}

void IRGenerator::printString(string &str) {
    string ir(printIR + "str " + str);
    generateIR(ir,printStr, false);
    outputIR(ir);
}

void IRGenerator::printExpr(string &name, ExprType type) {
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
    generateIR(ir,printType,false);
    outputIR(ir);
}

void IRGenerator::printNewline() {
    string ir("@print@newline");
    generateIR(ir,printNewLine,false);
    outputIR(ir);
}

void IRGenerator::conditionalJump(string &label, string &condition) {
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
    generateIR(ir,type,true);
    outputIR(ir);
}

void IRGenerator::unconditionalJump(string &label) {
    string ir("@jumpTo " + label);
    generateIR(ir,jump,true);
    outputIR(ir);
}

void IRGenerator::setDownLabel(string &label) {
    string ir(label + ":");
    generateIR(ir, statLabel, true);
    outputIR(ir);
}

void IRGenerator::spareRetStackAddr(string& funcName) {
    string ir("@funcCallBegin: " + funcName);
    generateIR(ir,funcCallBegin,false);
    outputIR(ir);
}

void IRGenerator::endProgram() {
    string ir("__end:");
    generateIR(ir, funcDefLabel, false);
    outputIR(ir);
}

string IRGenerator::applyVariableName() {
    return string(varLabel + to_string(this->opSymNum++));
}

string IRGenerator::applyForCycleLabel() {
    return string(forLabel + to_string(this->forLabelnum++));

}

string IRGenerator::applyDoCycleLabel() {
    return string(doLabel + to_string(this->doLabelnum++));
}

string IRGenerator::applyWhileCycleLabel() {
    return string(whileLabel + to_string(this->whileLabelnum++));
}

string IRGenerator::applyCodeBlkLabel() {
    string ans(codeLabel + to_string(this->codeLabelnum++));
    return ans;
}