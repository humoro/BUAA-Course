#include "Mips.h"

void MipsGenerator::generateMips(bool needOptimal) {
    this->classifier->nonOptimalIROutput(this->root);
    this->classifier->divideBasicBlk();
    if (needOptimal) {
        this->optimal->firstRoundCopyDiffuseOptimization();//先做基础的赋值传播处理，除去大部分不必要的中间变量声明
        this->optimal->commenSubExprOptimization(); // 消除公共子表达式
        this->optimal->secondRoundCopyDiffuseOptimization(); // 消除公共子子表达式之后会出现很多不必要的赋值语句
        this->optimal->thirdRoundCopyDiffuseOptimization();
        this->optimal->constantDiffuseOptimization(); // 常量传播
        this->optimal->jumpLabelOptimization(); // 跳转条件和标签的简化
    }
    this->optimal->optimizedIROutput();
    this->translator->translateIR();
}