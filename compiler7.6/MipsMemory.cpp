#include "Mips.h"
bool IRTranslator::flushVariable(string& varName, RegisterNumber valueReg) {
    if (this->entryTable->translateIR_isPartialVariable(varName, this->curFunctionName)) { //该变量在栈中分配了内存
        storeToStack(varName,valueReg);
        return true;
    }
    if (this->entryTable->translateIR_isGlobalVariable(varName)) {
        storeToMemory(varName, valueReg);
        return true;
    }
    return false;
}

void IRTranslator::storeToStack(string& varName, RegisterNumber valueReg) {
    unsigned int addr = this->entryTable->translateIR_getPartialVarAddr(varName, this->curFunctionName);
    generateMipsCode("sw $" + to_string(valueReg) + ",-" + to_string(addr) + "($sp)", swRI);
}

void IRTranslator::storeToMemory(string& varName, RegisterNumber valueReg) {//为用户的全局变量设计
    generateMipsCode("sw $" + to_string(valueReg) + "," + varName + "($0)", swRL);
}

void IRTranslator::storeToMemory(RegisterNumber reg, unsigned int addr) {
    generateMipsCode("sw $" + to_string(reg) + "," + to_string(addr) + "($0)", swRI);
}

bool IRTranslator::loadVariable(string &varName, RegisterNumber valueReg) {
    if (this->entryTable->translateIR_isPartialVariable(varName, this->curFunctionName)) { //该变量在栈中分配了内存
        loadFromStack(varName,valueReg);
        return true;
    }
    if (this->entryTable->translateIR_isGlobalVariable(varName)) {
        loadFromMemory(varName, valueReg);
        return true;
    }
    return false;
}

void IRTranslator::loadFromStack(string &varName, RegisterNumber valueReg) {
    if (!this->entryTable->translateIR_isPartialVarRefreshed(varName, this->curFunctionName)) return;
    unsigned int addr = this->entryTable->translateIR_getPartialVarAddr(varName, this->curFunctionName);
    generateMipsCode("lw $" + to_string(valueReg) + ",-" + to_string(addr) + "($sp)", lwRI);
    this->entryTable->translateIR_setPartialVariableLoaded(varName, this->curFunctionName);
}

void IRTranslator::loadFromMemory(string &varName, RegisterNumber valueReg) { //这是已经全局声明内存的变量提取
    if (!this->entryTable->translateIR_isGlobalVarRefreshed(varName)) return;
    generateMipsCode("lw $" + to_string(valueReg) + "," + varName + "($0)", lwRL);
    this->entryTable->translateIR_setGlobalVariableLoaded(varName);
}

void IRTranslator::loadFromMemory(RegisterNumber reg, unsigned int addr) {
    generateMipsCode("lw $" + to_string(reg) + "," + to_string(addr) + "($0)", lwRI);
}


