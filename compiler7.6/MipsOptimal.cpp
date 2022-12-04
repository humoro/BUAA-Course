#include "Mips.h"

void MipsOptimal::outputMips() {
    for (auto code : this->codes) {
        this->out << code.getCode() << endl;
    }
}

void MipsOptimal::peepholeOptimization() {

}

