#include <sstream>
#include "Mips.h"
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
void IRTranslator::allocGlbRegister(bool needOptimal) { // 分配全局寄存器，全局变量不分配全局寄存器
    // 无优化模式下使用顺序分配寄存器方法
    if (!needOptimal) {
        // 目前是再每个函数块中重新分配全局寄存器，按照就近原则，声明靠前的先得到全局寄存器
        string function;
        RegisterNumber glbRegister;
        for (auto blk : this->blks) {
            if (blk->getFunction() == ".global.") continue;
            else {
                if (blk->getFunction() != function) {
                    function = blk->getFunction();
                    glbRegister = glbRegisterBegin;
                    this->functionGlbRegisterMap[function] = unordered_map<string, RegisterNumber>(0);
                    for (auto stmt : blk->getIntermediateCode()) { // 得到每个函数块的全局寄存器分配表，全局寄存器中没有任何中间变量
                        if ((stmt->getIRType() == ir_funcParameterDef || stmt->getIRType() == ir_partialVariableDef) && glbRegister <= glbRegisterEnd) { // 函数的参数声明或者变量的声明
                            stringstream ss(stmt->getIR());
                            string varName, size;
                            getline(ss, varName, ' ');
                            getline(ss, varName, ' ');
                            getline(ss, varName, ' ');
                            getline(ss, size, ' ');
                            if (size.empty() || size == "1") {
                                (this->functionGlbRegisterMap[function])[varName] = glbRegister++;
                            }
                        }
                    }
                    this->functionGlbNumber[function] = glbRegister - glbRegisterBegin;
                }
            }
        }
        return;
    }
    // 优化模式下采用冲突图来分配全局寄存器
    for (auto it : this->conflictGraph) {
        unordered_set<string> noRegisterSet; // 不分配寄存器的集合
        vector<string> node;
        auto graph = it.second->getGraph();// 得到冲突图
        unsigned int regnum = glbRegisterEnd - glbRegisterBegin + 1; // 八个全局寄存器
        while (!graph.empty()) {
            bool find = true;
            while (find) {
                find = false;
                string name;
                name.clear();
                for (auto erasenode : graph) {
                    if (erasenode.second->getDegree() < regnum) { // 找到度数小于k的节点
                        find = true;
                        name = erasenode.first;
                        for (auto next : graph) {
                            next.second->eraseRelevant(name); // 删除所有和该节点相关边
                        }
                        break;
                    }
                }
                if (!name.empty()) { // 该节点从图中删除
                    graph.erase(name);
                    node.push_back(name);
                }
            }
            if (!graph.empty()) { // 在删除完可以删除的节点之后仍然存在不可删除的节点
                auto noReg = graph.begin(); // 删除一个节点标记不分配寄存器
                string noRegName = noReg->first;
                noRegisterSet.insert(noRegName);
                graph.erase(noRegName);
            }
        }
        unordered_map<string, RegisterNumber > registerMap;
        unordered_set<unsigned int> allocedReg;
        for (int i = node.size() - 1;i >= 0;i--) {
            string regname;
            vector<bool> alloced(regnum,false);
            for (const auto& reg : registerMap) {
                regname = reg.first;
                if (it.second->haveEdge(node[i], regname)) {
                    alloced[reg.second - glbRegisterBegin] = true;
                }
            }
            for (int j = 0;j < alloced.size();j++) {
                if (!alloced[j]) {
                    registerMap[node[i]] = glbRegisterBegin + j;
                    allocedReg.insert(glbRegisterBegin + j);
                    break;
                }
            }
        }
        this->functionGlbRegisterMap[it.first] = registerMap; // 得到寄存器的分配图
        this->functionGlbNumber[it.first] = allocedReg.size(); // 分配使用的到的寄存器数目
    }
}

vector<RegisterNumber> IRTranslator::applyRegister(vector<string> &varNames, vector<bool> &needLoads, unsigned int beginLoc) {//这里是申请临时寄存器
    vector<RegisterNumber> ans;
    RegisterNumber reg;
    for (unsigned long long i = 0;i < varNames.size();i++) {
        reg = applyGlbRegister(varNames[i], needLoads[i]);
        if (reg < 0) {
            reg = applyTmpRegister(varNames[i], needLoads[i], beginLoc);
            this->lockedRegister.insert(reg);
        }
        ans.push_back(reg);
    }
    this->lockedRegister.clear();
    return ans;
}

RegisterNumber IRTranslator::applyRegister(SymbolName &varName, bool needLoad, unsigned int beginLoc) { // 申请寄存器
    RegisterNumber ansReg = applyGlbRegister(varName, needLoad);
    if (ansReg < 0) {
        ansReg = applyTmpRegister(varName, needLoad, beginLoc);
    }
    return ansReg;
}

RegisterNumber IRTranslator::applyGlbRegister(SymbolName &varName, bool needLoad) {
    if (varName == "0") return 0;
    if (this->glbVariableRegisterMap.find(varName) != this->glbVariableRegisterMap.end()) {
        if (needLoad) { // 需要加载的话从内存中加载变量
            loadVariable(varName, this->glbVariableRegisterMap[varName]);
        }
        RegisterNumber ans = this->glbVariableRegisterMap[varName];
        if (this->glbRegisterVariableMap.find(ans) != this->glbRegisterVariableMap.end()) {
            if (this->loadedGlbVariable.find(this->glbRegisterVariableMap[ans]) != this->loadedGlbVariable.end()) {
                this->loadedGlbVariable.erase(this->glbRegisterVariableMap[ans]);
            }
            this->glbRegisterVariableMap[ans] = varName;
        }
        return ans;
    } else {
        return -1;
    }
}

RegisterNumber IRTranslator::applyTmpRegister(SymbolName &varName, bool needLoad, unsigned int beginLoc) { // 临时寄存器池
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
        if (this->entryTable->translateIR_isGlobalEntry(name) ||
            this->entryTable->translateIR_isPartialEntry(name, this->curFunctionName)) { //如果该变量已经分配了内存并且更新成功了那么当前寄存器已经释放成功了
            this->entryTable->translateIR_setGlobalVariableRefreshed(name);
            this->entryTable->translateIR_setPartialVariableRefreshed(name, this->curFunctionName);
            this->tmpVariableRegisterMap.erase(name);
            this->tmpRegisterVariableMap.erase(this->regAlloc);
        } else { //否则需要分配内存空间存放这个变量来释放寄存器
            this->tmpVariableRegisterMap.erase(name);
            this->tmpRegisterVariableMap.erase(this->regAlloc);
            if (variableAfterUser(name, beginLoc)) { //对于中间变量 后续没有对该变量的使用就不再存进内存
                unsigned int addr = this->memAlloc;
                this->memAlloc += 4;//分配新的内存
                this->curBlk->allocMem(4);//改基本块临时变量占用了内存
                storeToMemory(this->regAlloc, addr);
                this->variableMemoryMap[name] = addr;
            }
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

void IRTranslator::flushRegisterMap() {
    string name;
    for (auto & it : this->tmpVariableRegisterMap) {
        name = it.first;
        this->entryTable->translateIR_setPartialVariableRefreshed(name, this->curFunctionName);
        this->entryTable->translateIR_setGlobalVariableRefreshed(name);
    }
    this->tmpVariableRegisterMap.clear();
    this->variableMemoryMap.clear();
    this->tmpRegisterVariableMap.clear();
    this->regAlloc = tmpRegisterBegin;
    this->memAlloc -= this->curBlk->allocedMem();//每个基本块中的临时变量在离开该基本块后不占用内存
}