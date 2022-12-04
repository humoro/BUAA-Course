#include "Mips.h"
void IRTranslator::storeToStack(string& varName, regNum valueReg) {
    unsigned int addr = this->table->getPartialVarAddr(varName,this->curFunctionName);
    outMips("sw $" + to_string(valueReg) + ",-" + to_string(addr) + "($sp)");
}

void IRTranslator::storeToMemory(string& varName, regNum valueReg) {//为用户的全局变量设计
    outMips("sw $" + to_string(valueReg) + "," + varName + "($0)");
}

void IRTranslator::storeToMemory(regNum reg, unsigned int addr) {
    outMips("sw $" + to_string(reg) + "," + to_string(addr) + "($0)");
}

bool IRTranslator::loadVariable(string &varName, regNum valueReg) {
    if (this->table->isPartialVariable(varName, this->curFunctionName)) { //该变量在栈中分配了内存
        loadFromStack(varName,valueReg);
        return true;
    }
    if (this->table->globalExist(varName)) {
        loadFromMemory(varName, valueReg);
        return true;
    }
    return false;
}

void IRTranslator::loadFromStack(string &varName, regNum valueReg) {
    if (!this->table->isPartialVarRefreshed(varName, this->curFunctionName)) return;
    unsigned int addr = this->table->getPartialVarAddr(varName,this->curFunctionName);
    outMips("lw $" + to_string(valueReg) + ",-" + to_string(addr) + "($sp)");
    this->table->setPartialVariableLoaded(varName, this->curFunctionName);
}

void IRTranslator::loadFromMemory(string &varName, regNum valueReg) { //这是已经全局声明内存的变量提取
    if (!this->table->isGlobalVarRefreshed(varName)) return;
    outMips("lw $" + to_string(valueReg) + "," + varName + "($0)");
    this->table->setGlobalVariableLoaded(varName);
}

void IRTranslator::loadFromMemory(regNum reg, unsigned int addr) {
    outMips("lw $" + to_string(reg) + "," + to_string(addr) + "($0)");
}


