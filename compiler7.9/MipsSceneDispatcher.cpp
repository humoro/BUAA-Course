#include "Mips.h"
#include <sstream>
unsigned int IRTranslator::sceneMemoryAlloc() {
    return tmpRegisterMemory + glbRegisterMemory + this->curStackAlloc; // 腾出保存全局寄存器和临时寄存器的空间
}

void IRTranslator::sceneAnalysis(unsigned int beginloc) { // 在函数调用的时候贪心的分析现场中需要保存的临时变量
    string name;
    vector<string> names;
    for (auto & it : this->tmpVariableRegisterMap) {
        name = it.first;
        if (this->entryTable->translateIR_isPartialEntry(name, this->curFunctionName)) {
            this->entryTable->translateIR_setPartialVariableRefreshed(name, this->curFunctionName);
            names.push_back(name);
        } else if (this->entryTable->translateIR_isGlobalEntry(name)) {
            this->entryTable->translateIR_setGlobalVariableRefreshed(name);
            names.push_back(name);
        } else { // 中间变量
            bool afterUsed = false;
            for (auto j = beginloc;j < this->curBlk->size();j++) {
                switch (this->curBlk->operator[](j)->getIRType()) {
                    case ir_printInt:
                    case ir_printChar:
                    case ir_funcCallParaPush:
                    case ir_valueReturn:
                    {
                        stringstream ss(this->curBlk->operator[](j)->getIR());
                        string symbol;
                        getline(ss, symbol, ' ');
                        getline(ss, symbol, ' ');
                        if (symbol == name) {
                            afterUsed = true;
                        }
                    }
                        break;
                    case ir_greJump:
                    case ir_geqJump:
                    case ir_lesJump:
                    case ir_leqJump:
                    case ir_eqlJump:
                    case ir_neqJmp:
                    {
                        stringstream ss(this->curBlk->operator[](j)->getIR());
                        string left, right;
                        getline(ss, left, ' ');
                        getline(ss, left, ' ');
                        getline(ss, right, ' ');
                        if (right == name || left == name) {
                            afterUsed = true;
                            break;
                        }
                    }
                        break;
                    case ir_operation:
                    {
                        stringstream ss(this->curBlk->operator[](j)->getIR());
                        string opnum1, opnum2;
                        getline(ss, opnum1, ',');
                        getline(ss, opnum1, ',');
                        getline(ss, opnum2, ',');
                        if (opnum1 == name || opnum2 == name) {
                            afterUsed = true;
                            break;
                        }

                    }
                        break;
                    case ir_assign:
                    {
                        stringstream ss(this->curBlk->operator[](j)->getIR());
                        string left, right;
                        getline(ss, left, ' ');
                        getline(ss, right, ' ');
                        getline(ss, right, ' ');
                        if (right == name) {
                            afterUsed = true;
                            break;
                        }
                        if (left.find('[') != string::npos) {
                            string array, index;
                            stringstream arrays(left);
                            getline(arrays, array, '[');
                            getline(arrays, index, ']');
                            if (index == name) {
                                afterUsed = true;
                                break;
                            }
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
            if (!afterUsed) {
                names.push_back(name);
            }
        }
    }
    this->sceneNames = names;
}

void IRTranslator::sceneClear() {
    for (string& name : this->sceneNames) { // 去掉不需要保存的临时寄存器,也就是调用函数之后不用的变量的对应的寄存器
        unsigned int reg = this->tmpVariableRegisterMap[name];
        this->tmpVariableRegisterMap.erase(name);
        this->tmpRegisterVariableMap.erase(reg);
        this->entryTable->translateIR_setPartialVariableRefreshed(name, this->curFunctionName);
        this->entryTable->translateIR_setGlobalVariableRefreshed(name);
    }
}

//保护现场和回复现场都是在寄存器分配情况在编译时完全没有改变的状态之下进行的
void IRTranslator::sceneStore(string &function) {//函数调用之前现场存储
    string mips;
    unsigned int addr = this->curStackAlloc;
    for (auto reg = tmpRegisterBegin; reg <= tmpRegisterEnd; reg++) {
        if (this->tmpRegisterVariableMap.find(reg) != this->tmpRegisterVariableMap.end()) {
            generateMipsCode("sw $" + to_string(reg) + ",-" + to_string(addr + (reg - tmpRegisterBegin) * 4) + "($sp)", mips_swRegImm);
        }
    }
    string name;
    unsigned int nextGlbNumber = this->functionGlbNumber[function] + glbRegisterBegin; // 调用函数所使用的全局寄存器的个数，也就是判断影响范围，影响范围内查找需要保护的全局寄存器
    for (auto & it : this->loadedGlbRegister) {
        if (it < nextGlbNumber) {
            generateMipsCode("sw $" + to_string(it) + ",-" + to_string(addr + tmpRegisterMemory + (it - glbRegisterBegin) * 4) + "($sp)", mips_swRegImm);
        }
    }
}

void IRTranslator::sceneLoad(string &function) {
    string mips;
    string name;
    unsigned int addr = this->curStackAlloc;
    for (auto reg = tmpRegisterBegin; reg <= tmpRegisterEnd; reg++) {
        if (this->tmpRegisterVariableMap.find(reg) != this->tmpRegisterVariableMap.end()) {
            generateMipsCode("lw $" + to_string(reg) + ",-" + to_string(addr + (reg - 8) * 4) + "($sp)", mips_lwRegImm);
        }
    }
    unsigned int nextGlbNumber = this->functionGlbNumber[function] + glbRegisterBegin;
    for (auto & it : this->loadedGlbRegister) {
        if (it < nextGlbNumber) {
            generateMipsCode("lw $" + to_string(it) + ",-" + to_string(addr + tmpRegisterMemory + (it - glbRegisterBegin) * 4) + "($sp)", mips_lwRegImm);
        }
    }
}
