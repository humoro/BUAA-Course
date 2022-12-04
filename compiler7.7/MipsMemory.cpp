#include "Mips.h"
static unsigned int tmpRegisterBegin = 8;
static unsigned int tmpRegisterEnd = 15;
static unsigned int glbRegisterBegin = 16;
static unsigned int glbRegisterEnd = 23;
bool isTmpRegister(unsigned int number) {
    return number >= tmpRegisterBegin && number <= tmpRegisterEnd;
}

bool isGlbRegister(unsigned int number) {
    return number >= glbRegisterBegin && number <= glbRegisterEnd;
}
bool IRTranslator::flushVariable(string& varName, RegisterNumber valueReg) {
    if (isGlbRegister(valueReg)) { // 全局变量只需要标识不需要再从内存中初始化数值
        this->loadedGlbRegister.insert(valueReg);
        this->loadedGlbVariable.insert(varName);
    } else {
        if (this->entryTable->translateIR_isPartialVariable(varName, this->curFunctionName)) { //该变量在栈中分配了内存
            storeToStack(varName, valueReg);
            return true;
        }
        if (this->entryTable->translateIR_isGlobalVariable(varName)) {
            storeToMemory(varName, valueReg);
            return true;
        }
        return false;
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
    if (isTmpRegister(valueReg) && !this->entryTable->translateIR_isPartialVarRefreshed(varName, this->curFunctionName)) return;
    if (isGlbRegister(valueReg) && this->loadedGlbVariable.find(varName) != this->loadedGlbVariable.end()) return;
    unsigned int addr = this->entryTable->translateIR_getPartialVarAddr(varName, this->curFunctionName);
    generateMipsCode("lw $" + to_string(valueReg) + ",-" + to_string(addr) + "($sp)", lwRI);
    if (isTmpRegister(valueReg)) this->entryTable->translateIR_setPartialVariableLoaded(varName, this->curFunctionName);
    if (isGlbRegister(valueReg)) this->loadedGlbVariable.insert(varName);
    if (isGlbRegister(valueReg)) this->loadedGlbRegister.insert(valueReg);
}

void IRTranslator::loadFromMemory(string &varName, RegisterNumber valueReg) { //这是已经全局声明内存的变量提取
    if (isTmpRegister(valueReg) && !this->entryTable->translateIR_isGlobalVarRefreshed(varName)) return;
    if (isGlbRegister(valueReg) && this->loadedGlbVariable.find(varName) != this->loadedGlbVariable.end()) return;
    generateMipsCode("lw $" + to_string(valueReg) + "," + varName + "($0)", lwRL);
    this->entryTable->translateIR_setGlobalVariableLoaded(varName);
    if (isTmpRegister(valueReg)) this->entryTable->translateIR_setGlobalVariableLoaded(varName);
    if (isGlbRegister(valueReg)) this->loadedGlbVariable.insert(varName);
    if (isGlbRegister(valueReg)) this->loadedGlbRegister.insert(valueReg);
}

void IRTranslator::loadFromMemory(RegisterNumber reg, unsigned int addr) {
    generateMipsCode("lw $" + to_string(reg) + "," + to_string(addr) + "($0)", lwRI);
}


