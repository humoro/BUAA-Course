#include "Mips.h"
#include <sstream>
#include <queue>
static bool isConditionalJump(IRStatement* stmt) {
    return (stmt->getIRType() == greJump ||
            stmt->getIRType() == geqJump ||
            stmt->getIRType() == leqJump ||
            stmt->getIRType() == lesJump ||
            stmt->getIRType() == eqlJump ||
            stmt->getIRType() == neqJmp);
}

static bool isUnconditionalJump(IRStatement* stmt) {
    return stmt->getIRType() == jump;
}

bool IRTranslator::isInnerVariable(string& variable) {
    return variable.find('@') != string::npos && !isConstVariable(variable);
}

bool IRTranslator::isUserDefinitionVariable(string& variable) {
    return !variable.empty() && variable.find('[') == string::npos && !isConstVariable(variable) && variable.find('@') == string::npos; // 不是中间变量并且不是数组
}

unordered_set<unsigned int> IRTranslator::dataFlowUnion(const unordered_set<unsigned int>& left,
                                                        const unordered_set<unsigned int>& right) {
    unordered_set<unsigned int> ans = left;
    for (auto it : right) {
        ans.insert(it);
    }
    return ans;
}

unordered_set<unsigned int> IRTranslator::dataFlowDiff(const unordered_set<unsigned int>& left,
                                                       const unordered_set<unsigned int>& right) {
    unordered_set<unsigned int> ans = left;
    for (auto it : right) {
        if (ans.find(it) != ans.end()) {
            ans.erase(it);
        }
    }
    return ans;
}

unordered_set<string> IRTranslator::variableUnion(const unordered_set<string> &left,
                                                  const unordered_set<string> &right) {
    unordered_set<string> ans = left;
    for (const auto& it : right) {
        ans.insert(it);
    }
    return ans;
}

unordered_set<string> IRTranslator::variableDiff(const unordered_set<string> &left,
                                                  const unordered_set<string> &right) {
    unordered_set<string> ans = left;
    for (const auto& it : right) {
        if (ans.find(it) != ans.end()) {
            ans.erase(it);
        }
    }
    return ans;
}

void IRTranslator::addDefNode(string &variable, IRStatement *stmt) {
    this->defNodeVariableMap[stmt->getCodeNumber()] = variable;
}

void IRTranslator::blkPreProcess() { // 对每个基本快标号，并且对每个中间代码标号
    unsigned int codeNumber = 0;
    BlockNumber blkNumber = 0;
    string function = ".global.";
    // 给每个函数分配基本块并且把基本块的进出口进行标记
    for (auto blk : this->blks) {
        if (!blk->empty()) {
            this->blockNumberBlkMap[blkNumber] = blk;
            this->blkBlockNumberMap[blk] = blkNumber;
            for (auto* stmt : blk->getInterCode()) {
                stmt->setCodeNumber(codeNumber++);
            }
            if (blk->getFunction() != function) {
                function = blk->getFunction();
                this->functionBlks[function] = vector<BlockNumber>(0);
            }
            if (function != ".global.") {
                this->functionBlks[function].push_back(blkNumber);
            }
            blkNumber++;
        }
    }
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
            if (entranceStmt->getIRType() == statLabel) {
                string label = entranceStmt->getIR();
                label.pop_back();
                this->blockNumberBlkMap[blk]->setInLabel(label);
            }
            if (isConditionalJump(exitStmt)) {
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
            if (blk->getOutLabel().empty() || isConditionalJump(blk->back())) { // 条件跳转顺序下一个基本快也是子块，无条件跳转的子块就是跳转标签决定的
                if (i < curBlks.size() - 1) {
                    blk->addKid(curBlks[i + 1]);
                    this->blockNumberBlkMap[curBlks[i + 1]]->addParent(curBlks[i]);
                }
            }
        }
    }
}

void IRTranslator::dataFlowInitial() {
    // 先初始化所有语句的gen和kill，并且只考虑用户定义的变量不考虑中间变量
    // 可能产生定义点的中间代码有：复制，运算，读入，有返回值的函数调用，对于数组的赋值语句不考虑，数组是即用即取即存
    for (auto it : this->blockNumberBlkMap) {
        it.second->initialDefUseNode();
        it.second->clearGenKillDataFlow(); // 下面的函数开始计算每个基本快的gen和kill
    }
    string genVariable, killVariable;
    for (const auto& it : this->functionBlks) {
        for (auto blkNumber : it.second) {
            auto* blk = this->blockNumberBlkMap[blkNumber];
            for (auto* stmt : blk->getInterCode()) { // 先初始化各个语句的gen数据,并存储得到的定义点信息和变量
                genVariable.clear();
                if (stmt->getIRType() == assign) {
                    stringstream ss(stmt->getIR());
                    getline(ss, genVariable, ' '); // 对于定义点信息，只需要得到左侧的变量
                }
                if (stmt->getIRType() == operation) {
                    stringstream ss(stmt->getIR());
                    getline(ss, genVariable, ',');
                    getline(ss, genVariable, ',');
                    getline(ss, genVariable, ',');
                    getline(ss, genVariable, ','); // 对于定义点信息，只需要得到左侧的变量
                }
                if (stmt->getIRType() == retFuncCall ||
                    stmt->getIRType() == funcDefPara) {
                    stringstream ss(stmt->getIR());
                    getline(ss, genVariable, ' ');
                    getline(ss, genVariable, ' ');
                    getline(ss, genVariable, ' ');
                }
                if (stmt->getIRType() == readInt ||
                    stmt->getIRType() == readChar) {
                    stringstream ss(stmt->getIR());
                    getline(ss, genVariable, ' ');
                    getline(ss, genVariable, ' ');
                }
                if (isUserDefinitionVariable(genVariable)) { // 是用户定义变量，那么产生定义信息
                    addDefNode(genVariable, stmt);
                    stmt->addGen(stmt->getCodeNumber()); // 该语句产生定义信息
                    //以下计算该定义点的kill
                    for (auto killBlkNumber : it.second) {
                        auto killBlk = this->blockNumberBlkMap[killBlkNumber];
                        for (auto killed : killBlk->getInterCode()) {
                            killVariable.clear();
                            if (killed->operator==(stmt)) continue; // 跳过原始定义语句本身
                            if (killed->getIRType() == assign) {
                                stringstream ss(killed->getIR());
                                getline(ss, killVariable, ' '); // 对于定义点信息，只需要得到左侧的变量
                            }
                            if (killed->getIRType() == operation) {
                                stringstream ss(killed->getIR());
                                getline(ss, killVariable, ',');
                                getline(ss, killVariable, ',');
                                getline(ss, killVariable, ',');
                                getline(ss, killVariable, ','); // 对于定义点信息，只需要得到左侧的变量
                            }
                            if (killed->getIRType() == retFuncCall) {
                                stringstream ss(killed->getIR());
                                getline(ss, killVariable, ' ');
                                getline(ss, killVariable, ' ');
                                getline(ss, killVariable, ' ');
                            }
                            if (killed->getIRType() == readInt ||
                                stmt->getIRType() == readChar) {
                                stringstream ss(killed->getIR());
                                getline(ss, killVariable, ' ');
                                getline(ss, killVariable, ' ');
                            }
                            if (killVariable == genVariable) {
                                stmt->addKill(killed->getCodeNumber()); // stmt kill该定义点
                            }
                        }
                    }
                }
            }
        }
    }

    for (const auto& it : this->functionBlks) {
        vector<BlockNumber> curBlks = it.second;
        for (BlockNumber blkNumber : curBlks) {
            unordered_set<unsigned int> kill;
            auto blk = this->blockNumberBlkMap[blkNumber];
            for (auto stmt : blk->getInterCode()) {
                kill = dataFlowUnion(kill, stmt->getKill()); // 取所有kill集合的并集
            }
            blk->addKill(kill);
        }
    }
    // 计算基本块gen信息：
    for (const auto& it : this->functionBlks) {
        for (BlockNumber blkNumber : it.second) {
            unordered_set<unsigned int> gen;
            auto blk = this->blockNumberBlkMap[blkNumber];
            for (int i = int(blk->size() - 1);i >= 0;i--) {
                unordered_set<unsigned int> genTmp;
                genTmp = blk->operator[](i)->getGen();
                for (auto j = i;j < blk->size();j++) {
                    genTmp = dataFlowDiff(genTmp, blk->operator[](j)->getKill());
                }
                gen = dataFlowUnion(gen, genTmp);
            }
            blk->addGen(gen);
        }
    }

    // 查找所有变量的使用点：只统计用户定义变量
    for (auto blk : this->blks) {
        string variable, opnum1, opnum2, op;
        for (auto stmt : blk->getInterCode()) {
            variable.clear();
            opnum1.clear();
            opnum2.clear();
            stringstream ss(stmt->getIR());
            if (stmt->getIRType() == printInt ||
                stmt->getIRType() == printChar ||
                stmt->getIRType() == funcCallPara ||
                stmt->getIRType() == valueRet) {
                getline(ss, variable, ' ');
                getline(ss, variable, ' ');
            }
            if (isConditionalJump(stmt)) {
                getline(ss, opnum1, ' ');
                getline(ss, opnum1, ' ');
                getline(ss, opnum2, ' ');
            }
            if (stmt->getIRType() == operation) {
                getline(ss, op, ',');
                getline(ss, opnum1, ',');
                getline(ss, opnum2, ',');
            }
            if (stmt->getIRType() == assign) {
                getline(ss, variable, ' ');
                getline(ss, variable, ' ');
                getline(ss, variable, ' ');
            }
            if (isUserDefinitionVariable(variable)) {
                blk->addUse(variable,stmt->getCodeNumber());
            }
            if (isUserDefinitionVariable(opnum1) && op != "[]") {
                blk->addUse(opnum1,stmt->getCodeNumber());
            }
            if (isUserDefinitionVariable(opnum2) && op != "[]") {
                blk->addUse(opnum2,stmt->getCodeNumber());
            }
        }
    }
}

void IRTranslator::dataFlowAnalysis() {
    for (auto it  : this->blkBlockNumberMap) {
        it.first->clearInOutDataFlow(); // 初始化每个基本块的in和out集合为空集
        it.first->setDataFlowOutRefreshed();
    }
    for (const auto& it : this->functionBlks) {
        bool cycleSwitch = true;
        while (cycleSwitch) { // 当有基本块的数据流信息有变化的时候不断计算
            for (auto blknumber : it.second) {
                this->blockNumberBlkMap[blknumber]->setDataFlowOutRefreshed();
            }
            for (auto blknumber : it.second) {
                unordered_set<BlockNumber> parents = this->blockNumberBlkMap[blknumber]->getParents();
                unordered_set<unsigned int> inSet, outSet;
                for (auto node : parents) {
                    inSet = dataFlowUnion(inSet, this->blockNumberBlkMap[node]->getOut());
                }
                outSet = dataFlowUnion(this->blockNumberBlkMap[blknumber]->getGen(), dataFlowDiff(inSet, this->blockNumberBlkMap[blknumber]->getKill()));
                this->blockNumberBlkMap[blknumber]->refreshIn(inSet);
                this->blockNumberBlkMap[blknumber]->refreshOut(outSet);
            }
            cycleSwitch = false;
            for (auto blknumber : it.second) {
                cycleSwitch |= this->blockNumberBlkMap[blknumber]->getIsDataFlowOutRefreshed();
            }
        }
    }
    #if debug
    cout << "data flow : " <<  endl;
    for (auto it : this->blkBlockNumberMap) {
        cout << it.first->getFunction() << endl;
        cout << "in" <<  endl;
        for (auto node : it.first->getIn()) {
            cout << node << " ";
        }
        cout << endl;
        cout << "out" << endl;
        for (auto node : it.first->getOut()) {
            cout << node << " ";
        }
        cout << endl;
    }
    #endif
}

void IRTranslator::buildDefUseChain() {
    string variable;
    for (const auto& it : this->functionBlks) {
        if (this->defUseChains.find(it.first) == this->defUseChains.end()) {
            this->defUseChains[it.first] = unordered_map<string, vector<DefUseChain*>>(0);
        }
        for (auto blk : it.second) { // 遍历该函数中的每个基本块的gen
            for (auto defNode : this->blockNumberBlkMap[blk]->getGen()) {
                variable = this->defNodeVariableMap[defNode]; // 找到定义的变量
                auto* chain = new DefUseChain(variable, defNode);
                // 找到当前定义点活跃的基本块
                for (auto nblk : it.second) {
                    unordered_set<unsigned int> set = this->blockNumberBlkMap[nblk]->getIn();
                    if (set.find(defNode) != set.end() || nblk == blk) { // 该变量定义点到达该基本块
                        auto useSet = this->blockNumberBlkMap[nblk]->getUseNode(variable);
                        //把所有的使用点加到这个网
                        if (set.find(defNode) != set.end()) {
                            for (auto useNode : useSet) {
                                chain->addDefUseNode(useNode);
                            }
                        } else {
                            for (auto useNode : useSet) {
                                if (useNode > defNode)
                                    chain->addDefUseNode(useNode);
                            }
                        }
                    }
                }
                if (this->defUseChains[it.first].find(variable) == this->defUseChains[it.first].end()) {
                    this->defUseChains[it.first][variable] = vector<DefUseChain*>(0);
                }
                this->defUseChains[it.first][variable].push_back(chain);
                if (this->blkDefUseChainMap.find(blk) == this->blkDefUseChainMap.end()) {
                    this->blkDefUseChainMap[blk] = vector<DefUseChain*>(0);
                }
                this->blkDefUseChainMap[blk].push_back(chain);
            }
        }
    }
}

void IRTranslator::dataAnalysis() {
    blkPreProcess();// 数据流分析
    dataFlowInitial();
    dataFlowAnalysis();
    buildDefUseChain();
}

void IRTranslator::deleteDeadCode() {
    string variable,opnum1, opnum2,left;
    for (const auto& it : this->blkDefUseChainMap) {
        vector<unsigned int> eraseNode;
        auto blk = this->blockNumberBlkMap[it.first];
        for (auto chain : it.second) {
            if (chain->getChain().empty()) {
                auto stmt = blk->operator[](chain->getDefNode() - blk->operator[](0)->getCodeNumber());
                variable.clear();
                opnum1.clear();
                opnum2.clear();
                if (stmt->getIRType() == assign) {
                    stringstream ss(stmt->getIR());
                    getline(ss, left, ' ');
                    getline(ss, variable, ' ');
                    getline(ss, variable, ' ');
                }
                if (stmt->getIRType() == operation) {
                    stringstream ss(stmt->getIR());
                    getline(ss, opnum1, ',');
                    getline(ss, opnum1, ',');
                    getline(ss, opnum2, ',');
                    getline(ss, left, ',');
                }
                string function = blk->getFunction();
                if (!this->entryTable->translateIR_isPartialVariable(left, function)) { // 全局变量的定义点不进行删除
                    continue;
                }
                eraseNode.push_back(chain->getDefNode());
                queue<string> eraseInnerVariable;
                if (isInnerVariable(variable)) {
                    eraseInnerVariable.push(variable); // 该定义点右侧的中间变量也可以去除
                }
                if (isInnerVariable(opnum1)) {
                    eraseInnerVariable.push(opnum1); // 该定义点右侧的中间变量也可以去除
                }
                if (isInnerVariable(opnum2)) {
                    eraseInnerVariable.push(opnum2); // 该定义点右侧的中间变量也可以去除
                }
                while (!eraseInnerVariable.empty()) {
                    string eraseVariable = eraseInnerVariable.front(); //每次寻找队列最开始的变量的定义点
                    eraseInnerVariable.pop();
                    unsigned int erasedNode = 0;
                    for (auto nstmt : blk->getInterCode()) {
                        if (nstmt->getIRType() == assign) {
                            stringstream ss(nstmt->getIR());
                            getline(ss, variable, ' ');
                            if (variable == eraseVariable) {
                                eraseNode.push_back(nstmt->getCodeNumber());
                                erasedNode = nstmt->getCodeNumber();
                                break;
                            }
                        }
                        if (nstmt->getIRType() == operation) {
                            stringstream ss(nstmt->getIR());
                            getline(ss, opnum1, ',');
                            getline(ss, opnum1, ',');
                            getline(ss, opnum1, ',');
                            getline(ss, opnum1, ',');
                            if (opnum1 == eraseVariable) {
                                eraseNode.push_back(nstmt->getCodeNumber());
                                erasedNode = nstmt->getCodeNumber();
                                break;
                            }
                        }
                    }
                    auto estmt = blk->operator[](erasedNode - blk->operator[](0)->getCodeNumber());
                    variable.clear();
                    opnum1.clear();
                    opnum2.clear();
                    if (estmt->getIRType() == assign) {
                        stringstream ss(estmt->getIR());
                        getline(ss, variable, ' ');
                        getline(ss, variable, ' ');
                        getline(ss, variable, ' ');
                    }
                    if (estmt->getIRType() == operation) {
                        stringstream ss(estmt->getIR());
                        getline(ss, opnum1, ',');
                        getline(ss, opnum1, ',');
                        getline(ss, opnum2, ',');
                    }
                    if (isInnerVariable(variable)) {
                        eraseInnerVariable.push(variable); // 该定义点右侧的中间变量也可以去除
                    }
                    if (isInnerVariable(opnum1)) {
                        eraseInnerVariable.push(opnum1); // 该定义点右侧的中间变量也可以去除
                    }
                    if (isInnerVariable(opnum2)) {
                        eraseInnerVariable.push(opnum2); // 该定义点右侧的中间变量也可以去除
                    }
                }
            }
        }
        blk->erase(eraseNode);
    }
}

void IRTranslator::activeVariableInitial() {
    string left, right, opnum1, opnum2,ans, useVariable, defVariable, op;
    for (const auto& it : this->functionBlks) {
        for (auto number : it.second) {
            auto blk = this->blockNumberBlkMap[number];
            unordered_set<string> definition;
            unordered_set<string> useSet;
            for (auto stmt : blk->getInterCode()) {
                left.clear();
                right.clear();
                opnum1.clear();
                opnum2.clear();
                ans.clear();
                useVariable.clear();
                defVariable.clear();
                stringstream ss(stmt->getIR());
                if (stmt->getIRType() == assign) {
                    getline(ss, left, ' ');
                    getline(ss, right, ' ');
                    getline(ss, right, ' ');
                }
                if (stmt->getIRType() == operation) {
                    getline(ss, op, ',');
                    getline(ss, opnum1, ',');
                    getline(ss, opnum2, ',');
                    getline(ss, ans, ',');
                }
                if (stmt->getIRType() == readInt ||
                    stmt->getIRType() == readChar) { // 只会产生定义
                    getline(ss, defVariable, ' ');
                    getline(ss, defVariable, ' ');
                }
                if (stmt->getIRType() == funcDefPara ||
                    stmt->getIRType() == retFuncCall) { // 只会产生定义
                    getline(ss, defVariable, ' ');
                    getline(ss, defVariable, ' ');
                    getline(ss, defVariable, ' ');
                }
                if (stmt->getIRType() == funcCallPara ||
                    stmt->getIRType() == printChar ||
                    stmt->getIRType() == printInt ||
                    stmt->getIRType() == valueRet) { // 只会产生使用
                    getline(ss, useVariable, ' ');
                    getline(ss, useVariable, ' ');
                }
                if (isConditionalJump(stmt)) {
                    getline(ss, opnum1, ' ');
                    getline(ss, opnum1, ' ');
                    getline(ss, opnum2, ' ');
                }
                if (isUserDefinitionVariable(right)) {
                    if (definition.find(right) == definition.end()) { // 使用先于所有定义
                        useSet.insert(right);
                    }
                }
                if (isUserDefinitionVariable(left)) { // 产生的定义先于所有使用
                    if (useSet.find(left) == useSet.end()) {
                        definition.insert(left);
                    }
                }

                if (isUserDefinitionVariable(opnum1) && op != "[]") {
                    if (definition.find(opnum1) == definition.end()) { // 使用先于所有定义
                        useSet.insert(opnum1);
                    }
                }
                if (isUserDefinitionVariable(opnum2) && op != "[]") {
                    if (definition.find(opnum2) == definition.end()) { // 使用先于所有定义
                        useSet.insert(opnum2);
                    }
                }
                if (isUserDefinitionVariable(ans)) { // 产生的定义先于所有使用
                    if (useSet.find(ans) == useSet.end()) {
                        definition.insert(ans);
                    }
                }
                if (isUserDefinitionVariable(useVariable)) {
                    if (definition.find(useVariable) == definition.end()) { // 使用先于所有定义
                        useSet.insert(useVariable);
                    }
                }
                if (isUserDefinitionVariable(defVariable)) { // 产生的定义先于所有使用
                    if (useSet.find(defVariable) == useSet.end()) {
                        definition.insert(defVariable);
                    }
                }
            }
            blk->setDef(definition);
            blk->setUse(useSet);
        }
    }
}

void IRTranslator::activeVariableAnalysis() {
    for (auto it  : this->blkBlockNumberMap) {
        it.first->clearVariableData(); // 初始化每个基本块活跃变量的in和out集合为空集
        it.first->setDataFlowOutRefreshed();
    }
    for (const auto& it : this->functionBlks) {
        bool cycleSwitch = true;
        while (cycleSwitch) { // 当有基本块的数据流信息有变化的时候不断计算
            for (auto blknumber : it.second) {
                this->blockNumberBlkMap[blknumber]->setVariableDataRefreshed();
            }
            for (int i = it.second.size() - 1;i >= 0;i--) { // 倒序计算
                auto blknumber = it.second[i];
                unordered_set<BlockNumber> kids = this->blockNumberBlkMap[blknumber]->getKids();
                unordered_set<string> inSet, outSet;
                for (auto node : kids) {
                    outSet = variableUnion(outSet, this->blockNumberBlkMap[node]->getVariableIn());
                }
                inSet = variableUnion(this->blockNumberBlkMap[blknumber]->getUseSet(), variableDiff(outSet, this->blockNumberBlkMap[blknumber]->getDefSet()));
                this->blockNumberBlkMap[blknumber]->refreshInVariable(inSet);
                this->blockNumberBlkMap[blknumber]->refreshOutVariable(outSet);
            }
            cycleSwitch = false;
            for (auto blknumber : it.second) {
                cycleSwitch |= this->blockNumberBlkMap[blknumber]->getIsVariableDataRefreshed();
            }
        }
    }
    #ifdef debug
    cout << "variable" << endl;
    for (auto it : this->blkBlockNumberMap) {
        cout << "--------------------------" << endl;
        for (auto code : it.first->getInterCode()) {
            cout << code->getIR() << endl;
        }
        cout << "def" << endl;
        for (auto name : it.first->getDefSet()) {
            cout << name << " ";
        }
        cout << endl;
        cout << "use" << endl;
        for (auto name : it.first->getUseSet()) {
            cout << name << " ";
        }
        cout << endl;
        cout << "in" << endl;
        for (auto name : it.first->getVariableIn()) {
            cout << name << " ";
        }
        cout << endl;
        cout << "out" << endl;
        for (auto name : it.first->getVariableOut()) {
            cout << name << " ";
        }
        cout << endl;
        cout << "--------------------------" << endl;
    }
    #endif
}

void IRTranslator::buildConflictGraphBasedOnSet(unordered_set<string> &set,unordered_map<string, ConflictNode*>& nodemap, ConflictGraph* graph) {
    vector<string> nodes;
    for (const auto& it1 : set) {
        nodes.push_back(it1);
    }
    for (int i = 0;i < nodes.size();i++) {
        if (nodemap.find(nodes[i]) == nodemap.end()) nodemap[nodes[i]] = new ConflictNode(nodes[i]);
        for (int j = i + 1;j < nodes.size();j++) {
            if (nodemap.find(nodes[j]) == nodemap.end()) nodemap[nodes[j]] = new ConflictNode(nodes[j]);
            nodemap[nodes[i]]->addRelevantNode(nodes[j]);
            nodemap[nodes[j]]->addRelevantNode(nodes[i]); // 每个集合中的变量两两相关
            graph->addEdge(nodes[i], nodes[j]);
        }
    }
}

void IRTranslator::buildConflictGraph() { // 创建冲突图
    for (const auto& it : this->functionBlks) {
        if (this->conflictGraph.find(it.first) == this->conflictGraph.end()) {
            this->conflictGraph[it.first] = new ConflictGraph();
        }
        auto graph = this->conflictGraph[it.first];
        // 遍历每个基本块的in,out,use,def集合
        unordered_map<string, ConflictNode*> nodemap;
        for (auto number : it.second) {
            auto blk = this->blockNumberBlkMap[number];
            vector<unordered_set<string>> sets;
            sets.push_back(blk->getVariableIn());
            sets.push_back(blk->getVariableOut());
            sets.push_back(blk->getDefSet());
            sets.push_back(blk->getUseSet());
            unordered_set<string> set;
            for (const auto& cset : sets) {
                for (auto item : cset) {
                    string var = item;
                    string function = it.first;
                    if (this->entryTable->translateIR_isPartialVariable(item, function)) { // 只对函数内的局部变量构建和分配全局变量
                        set.insert(item);
                    }
                }
            }
            buildConflictGraphBasedOnSet(set, nodemap, graph);
        }
        graph->setGraph(nodemap);
    }
    cout << "conflict graph builded" << endl;
}