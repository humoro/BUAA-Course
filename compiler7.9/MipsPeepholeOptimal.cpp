#include "Mips.h"
#include <sstream>
void MipsOptimal::outputMips() {
    for (auto code : this->codes) {
        this->out << code->getCode() << endl;
    }
}

void MipsOptimal::peepholeOptimization() { // 窥孔优化

}
