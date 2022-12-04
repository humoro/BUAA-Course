#include "Mips.h"
void IRTranslator::outMips(const string& str) {
    this->mipsFile << str << endl;
}

void IRTranslator::translateIR() {
#ifdef debug // 查看基本块的内容
    for (auto blk : this->blks) {
        for (unsigned int i = 0;i < blk.size();i++) {
            cout << blk[i].getIR() << endl;
        }
        cout << endl;
    }
#endif
    //先定义所有字符串
    string str(".data");
    outMips(str);
    string printStr;
    unsigned int size;
    for (const auto& it : this->table->translateIR_getStrNameMap()) {
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
        outMips(str);
        this->memAlloc += size;//记录内存中字符串存储消耗的内存
    }
    //字对齐
    unsigned int oriMem = this->memAlloc;
    this->memAlloc = (this->memAlloc + 3) / 4 * 4;
    if (this->memAlloc - oriMem) {
        string mips("align : .space " + to_string(this->memAlloc - oriMem));
        outMips(mips);
    }
    for (auto blk : this->blks) {
        this->curBlk = &blk;
        translateBlk(blk);
        flushRegisterMap();
    }
}

void MipsGenerator::generateMips(bool needOptimal) {
    this->classifier->nonOptimalIROutput(this->root);
    this->classifier->divideBasicBlk();
    if (needOptimal) {
        this->optimal->firstRoundCopyDiffuseOptimization();//先做基础的赋值传播处理，除去大部分不必要的中间变量声明
        this->optimal->commenSubExprOptimization(); // 消除公共子表达式
        this->optimal->secondRoundCopyDiffuseOptimization(); // 消除公共子子表达式之后会出现很多不必要的赋值语句
        this->optimal->thirdRoundCopyDiffuseOptimization();
        this->optimal->jumpLabelOptimization(); // 跳转条件和标签的简化
    }
    this->optimal->optimizedIROutput();
    this->translator->translateIR();
}