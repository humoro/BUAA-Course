#include <sstream>
#include "Mips.h"
static unsigned int tmpRegisterBegin = 8;
static unsigned int tmpRegisterEnd = 15;
static unsigned int tmpRegisterMemory = 32;

/*
 * 关于函数调用
 * 函数调用之前要保护现场在调用之后恢复现场
 * */
/*
 * $0           0
 * $2           函数调用返回值
 * $3           中间寄存器
 * $4-$7        函数调用参数
 * $8-$15       临时寄存器
 * $16-$23      全局寄存器
 * $24-$25      中间寄存器，生成指令寄存器不够用的时候使用这两个
 * $28          全局指针($gp)
 * $29          堆栈指针($sp)
 * $30          帧指针($fp)配合栈指针使用
 * $31          返回地址($ra)*/
//保证已经分配过系统内存的变量不再临时申请内存空间，只有中间临时变量申请内存

unsigned int IRTranslator::sceneMemoryAlloc() {
    return tmpRegisterMemory + this->curStackAlloc; // 只有临时寄存器的数值需要保存每次函数调用默认开辟出所有寄存器的位置
}


void IRTranslator::sceneAnalysis(unsigned int beginloc) { // 在函数调用的时候贪心的分析现场中需要保存的临时变量
    string name;
    vector<string> names;
    for (auto & it : this->tmpVariableRegisterMap) {
        name = it.first;
        if (this->entryTable->translateIR_isPartialVariable(name, this->curFunctionName)) {
            this->entryTable->translateIR_setPartialVariableRefreshed(name, this->curFunctionName);
            names.push_back(name);
        } else if (this->entryTable->translateIR_isGlobalVariable(name)) {
            this->entryTable->translateIR_setGlobalVariableRefreshed(name);
            names.push_back(name);
        } else { // 中间变量
            bool afterUsed = false;
            for (auto j = beginloc;j < this->curBlk->size();j++) {
                switch (this->curBlk->operator[](j)->getIRType()) {
                    case printInt:
                    case printChar:
                    case funcCallPara:
                    case valueRet:
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
                    case greJump:
                    case geqJump:
                    case lesJump:
                    case leqJump:
                    case eqlJump:
                    case neqJmp:
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
                    case operation:
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
                    case assign:
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
    for (string& name : this->sceneNames) {
        unsigned int reg = this->tmpVariableRegisterMap[name];
        this->tmpVariableRegisterMap.erase(name);
        this->tmpRegisterVariableMap.erase(reg);
        this->entryTable->translateIR_setPartialVariableRefreshed(name, this->curFunctionName);
        this->entryTable->translateIR_setGlobalVariableRefreshed(name);
    }
}

//保护现场和回复现场都是在寄存器分配情况在编译时完全没有改变的状态之下进行的
void IRTranslator::sceneStore() {//函数调用之前现场存储
    string mips;
    unsigned int addr = this->curStackAlloc;
    for (auto reg = tmpRegisterBegin; reg <= tmpRegisterEnd; reg++) {
        if (this->tmpRegisterVariableMap.find(reg) != this->tmpRegisterVariableMap.end()) {
            generateMipsCode("sw $" + to_string(reg) + ",-" + to_string(addr + (reg - 8) * 4) + "($sp)", swRI);
        }
    }
}

void IRTranslator::sceneLoad() {
    string mips;
    string name;
    unsigned int addr = this->curStackAlloc;
    for (auto reg = tmpRegisterBegin; reg <= tmpRegisterEnd; reg++) {
        if (this->tmpRegisterVariableMap.find(reg) != this->tmpRegisterVariableMap.end()) {
            generateMipsCode("lw $" + to_string(reg) + ",-" + to_string(addr + (reg - 8) * 4) + "($sp)", lwRI);
        }
    }
}

vector<RegisterNumber> IRTranslator::applyTmpRegister(vector<string> &varNames, vector<bool> &needLoads) {//这里是申请临时寄存器
    vector<RegisterNumber> ans;
    for (unsigned long long i = 0;i < varNames.size();i++) {
        ans.push_back(tmpRegisterPool(varNames[i], needLoads[i]));
        if (ans.back()) {
            this->lockedRegister.insert(ans.back());
        }
    }
    this->lockedRegister.clear();
    return ans;
}

RegisterNumber IRTranslator::tmpRegisterPool(SymbolName &varName, bool needLoad) {
    if (varName == "0") return 0;
    if (this->tmpVariableRegisterMap.find(varName) != this->tmpVariableRegisterMap.end()) { //该变量已经分配了寄存器
        RegisterNumber reg = this->tmpVariableRegisterMap[varName];
        if (needLoad) loadVariable(varName, reg);
        return reg;
    }
    while (this->lockedRegister.find(this->regAlloc) != this->lockedRegister.end()) { //当前寄存器被锁定
        tmpRegisterRotate();
    }
    if (this->tmpRegisterVariableMap.find(this->regAlloc) != this->tmpRegisterVariableMap.end()) {//如果当前寄存器被占用
        string name = this->tmpRegisterVariableMap[this->regAlloc];//当前占用寄存器的变量
        if (flushVariable(name, this->regAlloc)) { //如果该变量已经分配了内存并且更新成功了那么当前寄存器已经释放成功了
            this->entryTable->translateIR_setGlobalVariableRefreshed(name);
            this->entryTable->translateIR_setPartialVariableRefreshed(name, this->curFunctionName);
            this->tmpVariableRegisterMap.erase(name);
            this->tmpRegisterVariableMap.erase(this->regAlloc);
        } else { //否则需要分配内存空间存放这个变量来释放寄存器
            unsigned int addr = this->memAlloc;
            this->memAlloc += 4;//分配新的内存
            this->curBlk->allocMem(4);//改基本块临时变量占用了内存
            storeToMemory(this->regAlloc, addr);
            this->variableMemoryMap[name] = addr;
            this->tmpVariableRegisterMap.erase(name);
            this->tmpRegisterVariableMap.erase(this->regAlloc);
        }
    }
    if (needLoad)
        if (!loadVariable(varName, this->regAlloc)) { //如果是事先在内存中分配了地址的变量，提取数值，
            if (this->variableMemoryMap.find(varName) != this->variableMemoryMap.end()) { // 如果临时变量本身有内存分配
                unsigned int addr = this->variableMemoryMap[varName];
                loadFromMemory(this->regAlloc,addr); // 从内存中提取出数值
            }
        }
    this->tmpVariableRegisterMap[varName] = this->regAlloc;
    this->tmpRegisterVariableMap[this->regAlloc] = varName;
    this->variableMemoryMap.erase(varName);
    unsigned int ans = this->regAlloc;
    tmpRegisterRotate();
    return ans;
}

void IRTranslator::tmpRegisterRotate() {
    if (this->regAlloc == tmpRegisterEnd){
        this->regAlloc = tmpRegisterBegin;//寄存器轮转
    } else {
        this->regAlloc++;
    }
}

void IRTranslator::allocGlbRegister() {

}

void IRTranslator::flushRegisterMap() {
    string name;
    if (this->curFunctionName != "__end") {
        for (auto it = this->tmpVariableRegisterMap.begin(); it != this->variableMemoryMap.end(); it++) {
            name = it->first;
            this->entryTable->translateIR_setPartialVariableRefreshed(name, this->curFunctionName);
            this->entryTable->translateIR_setGlobalVariableRefreshed(name);
        }
    }
    this->tmpVariableRegisterMap.clear();
    this->variableMemoryMap.clear();
    this->tmpRegisterVariableMap.clear();
    this->regAlloc = tmpRegisterBegin;
    this->memAlloc -= this->curBlk->allocedMem();//每个基本块中的临时变量在离开该基本块后不占用内存
}