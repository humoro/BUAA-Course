#include "Mips.h"
#include <sstream>
void MipsOptimal::outputMips() {
    for (auto code : this->codes) {
        this->out << code->getCode() << endl;
    }
}

void MipsOptimal::peepholeOptimization() { // 窥孔优化
    vector<unsigned int> eraseLocs;
    for (unsigned int i = 0;i < this->codes.size();i++) { // 删除lw后不必要的sw
        if (this->codes[i]->getType() == mips_lwLabelImm ||
            this->codes[i]->getType() == mips_lwRegImm ||
            this->codes[i]->getType() == mips_lwRegLabel) {
            auto j = i + 1;
            if (j >= this->codes.size()) continue;
            string lwright,swright;
            stringstream ss(this->codes[i]->getCode());
            getline(ss, lwright, ' ');
            getline(ss, lwright, ' ');
            if (this->codes[j]->getType() == mips_swLabelImm ||
                this->codes[j]->getType() == mips_swRegImm||
                this->codes[j]->getType() == mips_swRegLabel) {
                stringstream ns(this->codes[i]->getCode());
                getline(ns, swright, ' ');
                getline(ns, swright, ' ');
                if (lwright == swright)
                    eraseLocs.push_back(j);
            }
        }
    }
    unsigned int erased = 0;
    for (auto loc : eraseLocs) {
        this->codes.erase(this->codes.begin() + loc - erased);
        erased++;
    }
    for (unsigned int i = 0;i < this->codes.size();i++) { // 消除函数返回值计算的时候对v0的不必要操作
        if (this->codes[i]->getType() == mips_lwLabelImm ||
            this->codes[i]->getType() == mips_lwRegImm ||
            this->codes[i]->getType() == mips_lwRegLabel ||
            this->codes[i]->getType() == mips_addImm ||
            this->codes[i]->getType() == mips_addReg ||
            this->codes[i]->getType() == mips_subImm ||
            this->codes[i]->getType() == mips_subReg ||
            this->codes[i]->getType() == mips_mulImm ||
            this->codes[i]->getType() == mips_mulReg ||
            this->codes[i]->getType() == mips_divImm ||
            this->codes[i]->getType() == mips_divReg ||
            this->codes[i]->getType() == mips_moveReg ) {
            string instruction, leftReg, rest;
            stringstream ss(this->codes[i]->getCode());
            getline(ss, instruction, ' ');
            getline(ss, leftReg, ',');
            getline(ss, rest);
            auto j = i + 1;
            if (j >= this->codes.size()) continue;
            if (this->codes[j]->getType() == mips_moveReg) {
                stringstream ns(this->codes[j]->getCode());
                string nleft, nright;
                getline(ns, nleft,' ');
                getline(ns, nleft,',');
                getline(ns, nright);
                if (nleft == "$v0" && nright == leftReg) {
                    instruction += " $v0," + rest;
                    this->codes.insert(this->codes.begin() + i, new MipsCode(instruction, this->codes[i]->getType()));
                    this->codes.erase(this->codes.begin() + i + 1);
                    this->codes.erase(this->codes.begin() + i + 1);
                }
            }
        }
        for (unsigned int i = 0;i < this->codes.size();i++) { // 消除j label,  label形式中的j语句
            if (this->codes[i]->getType() == mips_jLabel || this->codes[i]->getType() == mips_branchZero || this->codes[i]->getType() == mips_branchReg) {
                string label;
                stringstream ss(this->codes[i]->getCode());
                getline(ss, label, ' ');
                getline(ss, label, ' ');
                if (this->codes[i]->getType() == mips_branchZero || this->codes[i]->getType() == mips_branchReg) {
                    string right = label;
                    stringstream ns(right);
                    getline(ns, label, ',');
                    getline(ns, label, ',');
                    if (this->codes[i]->getType() == mips_branchReg) {
                        getline(ns, label, ',');
                    }
                }
                auto j = i + 1;
                if (j >= this->codes.size()) continue;
                if (this->codes[j]->getType() == mips_codeLabel) {
                    string jlabel = this->codes[j]->getCode();
                    jlabel.pop_back();
                    if (label == jlabel) {
                        this->codes.erase(this->codes.begin() + 1);
                        i -= 2;
                    }
                }
            }
        }
    }
}
