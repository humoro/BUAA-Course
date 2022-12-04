#include <sstream>
#include "Mips.h"
static unsigned int scale = 15; // 中间代码小于15并且是叶子函数才进行函数内联
//函数的运行基本过程是先扫描获取所有函数的语句，并且获得函数调用其他函数的数据信息，所以需要先给所有的删除过死代码的语句重新编号

string IRTranslator::inlineFunctionVariableTransform(string& function, string& varName){
    if (!(this->entryTable->translateIR_isPartialEntry(varName, function))) return varName; // 不是局部变量或者常量
    string ans;
    ans = "__" + function + "__" + varName;
    return ans;
}

IRStatement * IRTranslator::inlineFunctionStatementTransform(IRStatement *stmt, string &function, string &retName) {
    string ir;
    stringstream ss(stmt->getIR());
    string variable, op, opnum1, opnum2, ans;
    switch(stmt->getIRType()) {
        case ir_valueReturn:
        {
            getline(ss, variable, ' ');
            getline(ss, variable, ' '); // 得到返回值返回变量
            variable = inlineFunctionVariableTransform(function, variable); //转换变量
            // 插入内联函数后必须把返回语句转换为赋值语句
            ir = retName + " = " + variable;
            return new IRStatement(ir, ir_assign);
        }
        case ir_greJump:
        case ir_geqJump:
        case ir_lesJump:
        case ir_leqJump:
        case ir_eqlJump:
        case ir_neqJmp:
        {
            getline(ss, op, ' ');
            getline(ss, opnum1, ' ');
            getline(ss, opnum2, ' ');
            getline(ss, ans, ' ');
            opnum1 = inlineFunctionVariableTransform(function, opnum1); //转换第一个比较数
            opnum2 = inlineFunctionVariableTransform(function, opnum2); //转换第二个比较数
            ir = op + " " + opnum1 + " " + opnum2 + " " + ans;
            return new IRStatement(ir, stmt->getIRType());
        }
        case ir_readInt:
        case ir_readChar:
        case ir_printInt:
        case ir_printChar:
        {
            getline(ss, op, ' ');
            getline(ss, variable, ' ');
            variable = inlineFunctionVariableTransform(function, variable); //转换变量
            ir = op + " " + variable;
            return new IRStatement(ir, stmt->getIRType());
        }
        case ir_operation:
        {
            getline(ss, op, ',');
            getline(ss, opnum1, ',');
            getline(ss, opnum2, ',');
            getline(ss, ans, ',');
            opnum1 = inlineFunctionVariableTransform(function, opnum1); //转换第一个操作数
            opnum2 = inlineFunctionVariableTransform(function, opnum2); //转换第二个操作数
            ans = inlineFunctionVariableTransform(function, ans); //转换结果标识符
            ir = op + "," + opnum1 + "," + opnum2 + "," + ans;
            return new IRStatement(ir, stmt->getIRType());
        }
        case ir_assign:
        {
            getline(ss, opnum1, ' ');
            getline(ss, opnum2, ' ');
            getline(ss, opnum2, ' ');
            opnum2 = inlineFunctionVariableTransform(function, opnum2); //转换右侧数
            if (opnum1.find('[') != string::npos) { // 左侧是数组
                stringstream ns(opnum1);
                string array, index;
                getline(ns, array, '['); //获取数组标识符
                getline(ns, index, ']'); //获取下标标识符
                array = inlineFunctionVariableTransform(function, array);
                index = inlineFunctionVariableTransform(function, index);
                ir = array + "[" + index +"] = " + opnum2;
            } else { // 左侧不是数组
                opnum1 = inlineFunctionVariableTransform(function, opnum1); //转换左侧数
                ir = opnum1 + " = " + opnum2;
            }
            return new IRStatement(ir, stmt->getIRType());
        }
        case ir_stmtLabel:
        case ir_jump:
        case ir_printStr:
        case ir_printNewLine:
            return stmt;
        default: {
            cout << stmt->getIR() << endl;
            cout << "error" << endl;
        }
    }
}

void IRTranslator::functionInlineOptimal() {
    //对于函数内联，首先需要对函数内的所有用户定义的变量进行重命名，并且只有叶子函数而且规模较小
    //在确定函数内联之后重命名变量，需要调用函数的位置插入函数体，重置函数的语句
    //并且需要把内联函数的函数表项添加到调用体
    blkPreProcess();
    for (const auto& it : this->functionBlks) {
        string function = it.first;
        this->functionStatementsMap[function] = vector<IRStatement*>(0);
        this->functionCallMap[function] = unordered_set<string>(0);
        bool isLeafFunction = true;
        for (auto number : it.second) {
            for (auto stmt : this->blockNumberBlkMap[number]->getIntermediateCode()) {
                this->functionStatementsMap[function].push_back(stmt);
                if (stmt->getIRType() == ir_funcCallBegin || isConditionalJumpIR(stmt) || isUnconditionalJump(stmt)) { // 有跳转语句也不能进行内联，因为可能出现跨越基本块的变量
                    isLeafFunction = false; // 有函数调用语句说明该函数不是叶子函数
                    string callFunction;
                    stringstream ss(stmt->getIR());
                    getline(ss, callFunction, ' ');
                    getline(ss, callFunction, ' '); //得到调用函数的名称
                    this->functionCallMap[function].insert(callFunction); // 添加调用函数信息
                }
            }
        }
        this->leafFunctionMap[function] = isLeafFunction;
    }
    for (auto& it : this->functionCallMap) {
        vector<string> nonLeafFunction;
        for (const auto& call : it.second) {
            if (!this->leafFunctionMap[call] ||
                this->functionStatementsMap[call].size() > scale) { // 非叶子函数或者函数的规模过大
                nonLeafFunction.push_back(call);
            }
        }
        for (const auto& eraseFunction : nonLeafFunction) {
            it.second.erase(eraseFunction);
        }
    } // 去掉不合法的要内联的函数
    for (const auto& it : this->functionCallMap) {
        if (it.second.empty()) continue;
        vector<IRStatement*> resetIR;
        bool switchToText = false;
        string caller = it.first;
        for (unsigned int i = 0;i < this->functionStatementsMap[it.first].size();i++) {
            auto stmt = this->functionStatementsMap[it.first][i];
            if (stmt->getIRType() == ir_funcDefLabel || stmt->getIRType() == ir_funcReturnAddrStore) {
                resetIR.push_back(stmt);
                continue;
            }
            if (!(stmt->getIRType() == ir_funcParameterDef ||
                  stmt->getIRType() == ir_funcReturnAddrStore ||
                  stmt->getIRType() == ir_partialVariableDef ||
                  stmt->getIRType() == ir_partialConstantDef)) {
                if (!switchToText) {
                    switchToText = true;
                    // 下面加入内联函数的重命名变量
                    for (auto inlineFunction : it.second) { // 把重命名的内联函数的变量信息做出声明并且添加符号表项
                        for (auto inlineStmt : this->functionStatementsMap[inlineFunction]) {
                            if (inlineStmt->getIRType() == ir_funcDefLabel || inlineStmt->getIRType() == ir_funcReturnAddrStore) continue;
                            if (inlineStmt->getIRType() == ir_funcParameterDef ||
                                inlineStmt->getIRType() == ir_partialVariableDef ||
                                inlineStmt->getIRType() == ir_partialConstantDef) {
                                // 内联函数的变量重命名， 把函数参数定义为局部变量
                                stringstream ss(inlineStmt->getIR());
                                string type, name, size, ir, value;
                                if (inlineStmt->getIRType() == ir_funcParameterDef ||
                                    inlineStmt->getIRType() == ir_partialVariableDef) {
                                    getline(ss, type, ' ');
                                    getline(ss, type, ' ');
                                    getline(ss, name, ' ');
                                    getline(ss, size, ' '); // size = 0或者size = 1
                                    name = inlineFunctionVariableTransform(inlineFunction, name);
                                    if (size.empty()) size = "1";
                                    ir = "@var " + type + " " + name + " " + size;
                                    resetIR.push_back(new IRStatement(ir, ir_partialVariableDef));
                                    if (size == "1") { // 内联函数的变量信息添加到调用函数中
                                        this->entryTable->translateIR_addInlineFunctionVariable(name, caller,type);
                                    } else {
                                        this->entryTable->translateIR_addInlineFunctionArray(name,caller,stoi(size),type);
                                    }
                                } else {
                                    getline(ss, type, ' ');
                                    getline(ss, type, ' ');
                                    getline(ss, name, ' ');
                                    getline(ss, value, ' ');
                                    getline(ss, value, ' ');
                                    name = inlineFunctionVariableTransform(inlineFunction, name);
                                    ir = "@const " + type + " " + name + " = " + value;
                                    resetIR.push_back(new IRStatement(ir, ir_partialConstantDef));
                                    this->entryTable->translateIR_addInlineFunctionConstant(name,
                                                                                            this->stoi(value),
                                                                                            caller, type);
                                }
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
            if (stmt->getIRType() == ir_funcCallBegin) {
                string call, ir, pushValue;
                stringstream ss(stmt->getIR());
                getline(ss, call, ' ');
                getline(ss, call, ' '); // 获得即将调用函数的名称
                if (it.second.find(call) == it.second.end()) { // 该函数调用的不是需要内联的函数
                    resetIR.push_back(stmt);
                    continue;
                }
                // 调用的函数需要内联
                vector<string> parameters = this->entryTable->translateIR_getFunctionParameterList(call);
                unsigned int paraIndex = 0;
                for (i++; this->functionStatementsMap[it.first][i]->getIRType() != ir_valueReturnFuncCall &&
                         this->functionStatementsMap[it.first][i]->getIRType() != ir_voidRetutnFuncCall; i++, paraIndex++) {
                    //参数传递修改为赋值语句
                    stringstream ns(this->functionStatementsMap[it.first][i]->getIR());
                    getline(ns, pushValue, ' ');
                    getline(ns, pushValue, ' '); // 得到传参的参数值
                    ir = inlineFunctionVariableTransform(call, parameters[paraIndex]) + " = " + pushValue;
                    resetIR.push_back(new IRStatement(ir, ir_assign));
                } // 参数传递压栈的修改
                // 函数体内嵌
                string retName;
                if (this->functionStatementsMap[it.first][i]->getIRType() == ir_valueReturnFuncCall) {
                    stringstream ns(this->functionStatementsMap[it.first][i]->getIR());
                    getline(ns, retName, ' ');
                    getline(ns, retName, ' ');
                    getline(ns, retName, ' '); // 得到返回值存放的变量
                }
                for (auto inlineStmt : this->functionStatementsMap[call]) {
                    if (!(inlineStmt->getIRType() == ir_partialVariableDef || inlineStmt->getIRType() == ir_partialConstantDef ||
                          inlineStmt->getIRType() == ir_funcParameterDef || inlineStmt->getIRType() == ir_voidReturn ||
                          inlineStmt->getIRType() == ir_funcReturnAddrStore || inlineStmt->getIRType() == ir_funcDefLabel))
                        resetIR.push_back(inlineFunctionStatementTransform(inlineStmt, call, retName));
                }
            } else {
                resetIR.push_back(stmt);
            }
        }
        this->functionStatementsMap[it.first] = resetIR;
    }
    #if debug
    for (auto blk : this->blks) {
        cout << "-------------" << blk->getFunction() << "-----------------" << endl;
        for (auto stmt : blk->getInterCode()) {
            cout << stmt->getIR() << endl;
        }
    }
    #endif
    this->statements.clear();
    if (this->globalBlk)  //把全局基本快中语句下加入到statements
        for (auto stmt : this->globalBlk->getIntermediateCode()) {
            this->statements.push_back(stmt);
        }
    for (const auto& function : this->functions) {
        for (auto irStat : this->functionStatementsMap[function]) {
            this->statements.push_back(irStat);
        }
    } // 重置中间代码
    for (auto stmt : this->statements) {
        if (stmt->getIRType() == ir_funcDefLabel ||
            stmt->getIRType() == ir_stmtLabel ||
            isUnconditionalJump(stmt) ||
            isConditionalJumpIR(stmt) ||
            isReturnIR(stmt)) {
            stmt->setBreakPoint();
        }
    } // 重置划分基本快中间代码
    divideBasicBlk();
    #if debug
    for (auto function : this->functionBlks) {
        cout << function.first << endl;
        for (auto blk : function.second) {
            cout << "-------------------" << endl;
            for (auto stmt : this->blockNumberBlkMap[blk]->getInterCode()) {
                cout << stmt->getIR() << endl;
            }
            cout << "-------------------" << endl;
        }
    }
    cout << "inline" << endl;
    #endif
}
