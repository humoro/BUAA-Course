#include "Mips.h"

void IRTranslator::translateIR() {
    #ifdef debug // 查看基本块的内容
    for (auto blk : this->blks) {
        for (unsigned int i = 0;i < blk.size();i++) {
            cout << blk[i].getIR() << endl;
        }
        cout << endl;
    }
    #endif
    blkCodePreProcess();// 数据流分析
    dataFlowInitial();
    dataFlowAnalysis();
    activeVariableInitial();
    activeVariableAnalysis();
    buildDefUseNet();
    buildConflictGraph();
    allocGlbRegister();
    //先定义所有字符串
    string str(".data");
    generateMipsCode(str, segementSet);
    string printStr;
    unsigned int size;
    for (const auto& it : this->entryTable->translateIR_getStrNameMap()) {
        printStr.clear();
        size = 0;
        for (auto c : it.second) {
            size++;
            printStr += c;
            if (c == '\\') {
                printStr += '\\';
            }
        }
        size += 1;
        str = it.first + ": .asciiz \"" + printStr + "\"";
        generateMipsCode(str, globalData);
        this->memAlloc += size;//记录内存中字符串存储消耗的内存
    }
    //字对齐
    unsigned int oriMem = this->memAlloc;
    this->memAlloc = (this->memAlloc + 3) / 4 * 4;
    if (this->memAlloc - oriMem) {
        string mips("align : .space " + to_string(this->memAlloc - oriMem));
        generateMipsCode(mips, globalData);
    }
    for (auto blk : this->blks) {
        this->curBlk = blk;
        translateBlk(blk);
        flushRegisterMap();
    }
}

void MipsGenerator::generateMips(bool needOptimal) {
    this->classifier->nonOptimalIROutput(this->root);
    this->classifier->divideBasicBlk();
    if (needOptimal) {
        // 做中间代码极小化的优化
        this->irOptimal->firstRoundCopyDiffuseOptimization();// 第一轮处理，消除申请的不必要的中间的变量
        this->irOptimal->secondRoundCopyDiffuseOptimization(); // 进一步消除由于赋值原因出现的不必要的中间变量
        this->irOptimal->thirdRoundCopyDiffuseOptimization(); // 消除每个表达式结束的时候申请的不必要的中间变量
        // 赋值传播优化
        this->irOptimal->forthRoundCopyDiffuseOptimization(); // 赋值传播的优化，对于赋值传播尽量使用考前定义的变量
    }
    this->irOptimal->optimizedIROutput();
    this->translator->translateIR();
    if (needOptimal) {
        this->mipsOptimal->peepholeOptimization(); // 窥孔优化
    }
    this->mipsOptimal->outputMips(); // 输出mips代码
}