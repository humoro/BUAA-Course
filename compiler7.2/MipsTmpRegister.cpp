#include "Mips.h"
//保证已经分配过系统内存的变量不再临时申请内存空间，只有中间临时变量申请内存
vector<regNum> IRTranslator::applyTmpRegister(vector<string> &varNames, vector<bool> &needLoads) {//这里是申请临时寄存器
    vector<regNum> ans;
    for (unsigned long long i = 0;i < varNames.size();i++) {
        ans.push_back(tmpRegisterPool(varNames[i], needLoads[i]));
        if (ans.back()) {
            this->lockedReg.insert(ans.back());
        }
    }
    this->lockedReg.clear();
    return ans;
}

regNum IRTranslator::tmpRegisterPool(SymbolName &varName, bool needLoad) {
    if (varName == "0") return 0;
    if (this->varRegisterMap.find(varName) != this->varRegisterMap.end()) { //该变量已经分配了寄存器
        regNum reg = this->varRegisterMap[varName];
        if (needLoad) loadVariable(varName, reg);
        return reg;
    }
    while (this->lockedReg.find(this->regAlloc) != this->lockedReg.end()) { //当前寄存器被锁定
        tmpRegisterRotate();
    }
    if (this->regAllocMap.find(this->regAlloc) != this->regAllocMap.end()) {//如果当前寄存器被占用
        string name = this->regAllocMap[this->regAlloc];//当前占用寄存器的变量
        if (flushVariable(name, this->regAlloc)) { //如果该变量已经分配了内存并且更新成功了那么当前寄存器已经释放成功了
            this->varRegisterMap.erase(name);
            this->regAllocMap.erase(this->regAlloc);
        } else { //否则需要分配内存空间存放这个变量来释放寄存器
            unsigned int addr = this->memAlloc;
            this->memAlloc += 4;//分配新的内存
            this->curBlk->allocMem(4);//改基本块临时变量占用了内存
            storeToMemory(this->regAlloc, addr);
            this->varMemoryMap[name] = addr;
            this->varRegisterMap.erase(name);
            this->regAllocMap.erase(this->regAlloc);
        }
    }
    if (needLoad)
        if (!loadVariable(varName, this->regAlloc)) { //如果是事先在内存中分配了地址的变量，提取数值，
            if (this->varMemoryMap.find(varName) != this->varMemoryMap.end()) { // 如果临时变量本身有内存分配
                unsigned int addr = this->varMemoryMap[varName];
                loadFromMemory(this->regAlloc,addr); // 从内存中提取出数值
            }
        }
    this->varRegisterMap[varName] = this->regAlloc;
    this->regAllocMap[this->regAlloc] = varName;
    this->varMemoryMap.erase(varName);
    unsigned int ans = this->regAlloc;
    tmpRegisterRotate();
    return ans;
}

void IRTranslator::tmpRegisterRotate() {
    if (this->regAlloc == 15){
        this->regAlloc = 8;//寄存器轮转
    } else {
        this->regAlloc++;
    }
}