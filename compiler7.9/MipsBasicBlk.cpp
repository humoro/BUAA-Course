#include "Mips.h"
#include <sstream>
void IRTranslator::divideBasicBlk() {//读取中间代码指令并且划分基本快
    this->blks.clear();
    unsigned int codeNum = 0;
    string function = ".global.";
    this->blks.push_back(new BasicBlk(function));
    for (auto& stmt : this->statements) {
        if (stmt->isBPStatement()) { // 是一个断点语句
            if (stmt->getIRType() == ir_funcDefLabel) {
                stringstream ss(stmt->getIR());
                getline(ss, function, ' ');
                getline(ss, function, ' ');
                function.pop_back(); // 函数名切换
                this->blks.push_back(new BasicBlk(function));
                codeNum = 1;
                this->blks.back()->push_statement(new IRStatement(stmt, 0));
            } else if (isUnconditionalJump(stmt) ||
                       isConditionalJumpIR(stmt) ||
                       isReturnIR(stmt)) {
                this->blks.back()->push_statement(new IRStatement(stmt, codeNum));
                this->blks.push_back(new BasicBlk(function));
                codeNum = 0;
            } else {
                this->blks.push_back(new BasicBlk(function));
                codeNum = 1;
                this->blks.back()->push_statement(new IRStatement(stmt, 0));
            }
        } else {
            this->blks.back()->push_statement(new IRStatement(stmt, codeNum));
            codeNum += 1;
        }
    }
}


void IRTranslator::blkPreProcess() { // 对每个基本块标号，并且对每个中间代码标号
    // 重置函数对应的基本块
    this->functionBlks.clear();
    this->functions.clear();
    this->blockNumberBlkMap.clear();
    this->blkBlockNumberMap.clear();
    this->globalBlk = nullptr;
    unsigned int codeNumber = 0;
    BlockNumber blkNumber = 0;
    string function = ".global.";
    // 给每个函数分配基本块并且把基本块的进出口进行标记
    for (auto blk : this->blks) {
        blk->clearParentKid();
        if (!blk->empty()) {
            this->blockNumberBlkMap[blkNumber] = blk;
            this->blkBlockNumberMap[blk] = blkNumber;
            for (auto* stmt : blk->getIntermediateCode()) {
                stmt->setCodeNumber(codeNumber++);
            }
            if (blk->getFunction() != function) {
                this->functions.push_back(function); // 按顺序记录函数
                function = blk->getFunction();
                this->functionBlks[function] = vector<BlockNumber>(0);
            }
            if (function != ".global.") {
                this->functionBlks[function].push_back(blkNumber);
            }
            if (blk->getFunction() == ".global.") this->globalBlk = blk;
            blkNumber++;
        }
    }
    this->functions.push_back(function); // 按顺序记录函数
    // 给每个函数的所有基本快确定前后关联关系
    for (const auto& it : this->functionBlks) {
        vector<BlockNumber> curBlks = it.second;
        #ifdef debug
        for (auto blk : curBlks) {
            for (auto stmt : this->blockNumberBlkMap[blk]->getInterCode()) {
                cout << stmt->getIR() << endl;
            }
            cout << "------------------------" << endl;
        }
        #endif
        for (auto blk : curBlks) { // 把所有基本块的入口和出口语句截取出来
            auto entranceStmt = this->blockNumberBlkMap[blk]->operator[](0);
            auto exitStmt = this->blockNumberBlkMap[blk]->back();
            if (entranceStmt->getIRType() == ir_stmtLabel) {
                string label = entranceStmt->getIR();
                label.pop_back();
                this->blockNumberBlkMap[blk]->setInLabel(label);
            }
            if (isConditionalJumpIR(exitStmt)) {
                stringstream ss(exitStmt->getIR());
                string label;
                getline(ss, label,' ');
                getline(ss, label,' ');
                getline(ss, label,' ');
                getline(ss, label,' ');
                this->blockNumberBlkMap[blk]->setOutLabel(label);
            } else if (isUnconditionalJump(exitStmt)) {
                stringstream ss(exitStmt->getIR());
                string label;
                getline(ss, label,' ');
                getline(ss, label,' ');
                this->blockNumberBlkMap[blk]->setOutLabel(label);
            }
        }
        for (auto blknumber : curBlks) { // 先添加所有跳转带来的基本块数据流关系
            auto blk = this->blockNumberBlkMap[blknumber];
            if (!blk->getOutLabel().empty()) { // 出口连接下一个基本快
                for (auto kidBlk : curBlks) {
                    if (this->blockNumberBlkMap[kidBlk]->getInLabel() == blk->getOutLabel()) {
                        blk->addKid(kidBlk);
                        this->blockNumberBlkMap[kidBlk]->addParent(blknumber);
                    }
                }
            }
        }
        for (unsigned long long i = 0;i < curBlks.size();i++) { // 先添加所有跳转带来的基本块数据流关系
            auto blk = this->blockNumberBlkMap[curBlks[i]];
            if (blk->getOutLabel().empty() || isConditionalJumpIR(blk->back())) { // 条件跳转顺序下一个基本快也是子块，无条件跳转的子块就是跳转标签决定的
                if (i < curBlks.size() - 1) {
                    blk->addKid(curBlks[i + 1]);
                    this->blockNumberBlkMap[curBlks[i + 1]]->addParent(curBlks[i]);
                }
            }
        }
    }
}

