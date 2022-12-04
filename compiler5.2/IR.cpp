#include "IR.h"
#include "Type.h"
const static string constDef = "var.const.partial ";
const static string variableDef = "var.plant.partial ";
const static string globalConstDef = "var.const.global ";
const static string globalVariableDef = "var.plant.global ";
const static string functionLabel = "function.def.";
const static string functionDefParaIR = "para.function.def ";
const static string functionCallParaIR = "para.function.call ";
const static string readIR = "read.";
const static string opSym = "opSym.";
const static string retFuncCallIR = "call.retfunc ";
const static string voidFuncCallIR = "call.voidfunc ";
const static string voidFuncReturnIR = "ret.voidfunc ";
const static string returnFuncReturnIR = "ret.retfunc ";
const static string printIR = "print.";
const static string forlabel = "for.";
const static string dolabel = "do.";
const static string whilelabel = "while.";
const static string codelabel = "code.";

void IRGenerator::outputIR(string &ir) {
    this->irOut << ir << endl;
}

void IRGenerator::constDefIR(string& name, int value) {
    string ir(constDef + name + " = " + to_string(value));
    outputIR(ir);
}

void IRGenerator::variableDefIR(string& name, unsigned int size) {
    string ir(variableDef + name + " " + to_string(size));
    outputIR(ir);
}

void IRGenerator::functionDef(string& funcName) {
    string label(functionLabel + funcName + ":");
    outputIR(label);
}


void IRGenerator::returnFuncCall(string &leftSymbol, string &funcName) {
    string ir(retFuncCallIR + funcName + " " + leftSymbol);
    outputIR(ir);
}

void IRGenerator::voidFuncCall(string &funcName) {
    string ir(voidFuncCallIR + funcName);
    outputIR(ir);
}

void IRGenerator::functionDefPara(string &paraName) {
    string para(functionDefParaIR + paraName);
    outputIR(para);
}


void IRGenerator::functionCallPara(string &paraName) {
    string ir(functionCallParaIR + paraName);
    outputIR(ir);
}

void IRGenerator::globalVariableDefIR(string &name, unsigned int size) {
    string ir(globalVariableDef + name + " " + to_string(size));
    outputIR(ir);
}

void IRGenerator::globalConstDefIR(string &name, int value) {
    string ir(globalConstDef + name + " = " + to_string(value));
    outputIR(ir);
}

void IRGenerator::readStatIR(SymbolType type, string &rName) {
    string typeStr;
    if (type == plantCharVar) {
        typeStr = "char";
    } else if (type == plantIntVar) {
        typeStr = "int";
    } else {
        typeStr = "error";
    }
    string ir(readIR + typeStr + " " + rName);
    outputIR(ir);
}

void IRGenerator::operationIR(string &op, string &opSym1, string &opSym2,string& ans) {
    if (opSym1.empty()) opSym1 = "0";
    if (opSym2.empty()) opSym2 = "0";
    string ir(op + " " + opSym1 + " " + opSym2 + " " + ans);
    outputIR(ir);
}

void IRGenerator::assignIR(string &left, string &right) {
    string ir(left + " = " + right);
    outputIR(ir);
}

void IRGenerator::retFuncReturn(string &name) {
    string ir(returnFuncReturnIR + name);
    outputIR(ir);
}

void IRGenerator::voidFuncReturn() {
    string ir(voidFuncReturnIR);
    outputIR(ir);
}

void IRGenerator::printStrIR(string &printStr) {
    string ir(printIR + "str " + printStr);
    outputIR(ir);
}

void IRGenerator::printSymbolIR(string &name) {
    string ir(printIR + "sym " + name);
    outputIR(ir);
}

void IRGenerator::conditionIR(string &label, string &condition) {
    string ir("if (" + condition + ") goto " + label);
    outputIR(ir);
}

void IRGenerator::gotoIR(string &label) {
    string ir("goto " + label);
    outputIR(ir);
}

void IRGenerator::setLabel(string &label) {
    string ir(label + ":");
    outputIR(ir);
}

string IRGenerator::applyOperationSymbol() {
    return string(opSym + to_string(this->opSymNum++));
}

string IRGenerator::applyForCycleLabel() {
    return string(forlabel + to_string(this->forLabelnum++));

}

string IRGenerator::applyDoCycleLabel() {
    return string(dolabel + to_string(this->doLabelnum++));
}

string IRGenerator::applyWhileCycleLabel() {
    return string(whilelabel + to_string(this->whileLabelnum++));
}

string IRGenerator::applyCodeLabel() {
    string ans(codelabel + to_string(this->codeLabelnum++));
    return ans;
}