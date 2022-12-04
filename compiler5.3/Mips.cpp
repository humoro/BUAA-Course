#include "Mips.h"
void IRReader::readIR() {//读取中间代码指令并且划分基本快
    vector<IRStatement> statements;

}

void IROptimal::optimizeIR() {

}

void IRTranslator::translateIR() {

}

void MipsGenerator::generateMips(bool needOptimal) {
    this->reader->readIR();
    if (needOptimal) {
        this->optimal->optimizeIR();
    }
    this->translator->translateIR();
}
