#include "Mips.h"
void IRTranslator::blkCodePreProcess() { // 对每个基本快标号，并且对每个中间代码标号
    unsigned int codeNumber = 0;
    unsigned int blkNumber = 0;
    for (auto blk : this->blks) {
        blk->setBlkNumber(blkNumber++);
        for (auto* stmt : blk->getInterCode()) {
            stmt->setCodeNumber(codeNumber++);
        }
    }
    #ifdef debug
    for (auto blk : this->blks) {
        cout << "blknumber:" << blk->getNumber() << endl;
        for (auto* stmt : blk->getInterCode()) {
            cout << stmt->getCodeNumber() << endl;
        }
    }
    #endif
}

void IRTranslator::dataFlowInitial() {

}

void IRTranslator::dataFlowAnalysis() {

}

void IRTranslator::activeVariableInitial() {

}

void IRTranslator::activeVariableAnalysis() {

}

void IRTranslator::buildDefUseNet() {

}

void IRTranslator::buildConflictGraph() {

}