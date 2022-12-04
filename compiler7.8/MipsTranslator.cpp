#include "Mips.h"
#include <sstream>

/*
 * 关于函数调用
 * 函数调用之前要保护现场,在调用之后恢复现场
 * */
/*
 * $0           0
 * $2           函数调用返回值
 * $3           中间寄存器
 * $4-$7        函数调用参数
 * $8-$15       临时寄存器
 * $16-$23      全局寄存器
 * $24-$25      中间寄存器，生成指令寄存器不够用的时候使用这两个
 * $28          全局指针($gp)
 * $29          堆栈指针($sp)
 * $30          帧指针($fp)配合栈指针使用
 * $31          返回地址($ra)
 * */

void IRTranslator::constantDefinitionTranslator(IRStatement *statement) {
    stringstream ss(statement->getIR());
    string symbol;
    string value;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,value,' ');
    getline(ss,value);
    if (statement->getIRType() == ir_globalConstantDef) {
        generateMipsCode(symbol + ": .word " + value, mips_globalData);
        this->entryTable->translateIR_setGlobalConstantInfo(symbol, this->memAlloc, this->stoi(value));//符号表中设置变量的地址
        this->memAlloc += 4;
    } else {
        this->entryTable->translateIR_setPartialConstantInfo(symbol, this->curFunctionName, this->curStackAlloc,
                                                             this->stoi(value));
        this->curStackAlloc += 4; // 局部const只分配内存不使用访存语句存储数值
        //对于所有的const变量全部替换为其具体的数值
    }
}

void IRTranslator::variableDefinitionTranslator(IRStatement *statement) {
    stringstream ss(statement->getIR());
    string symbol;
    string scale;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss, scale, ' ');
    unsigned int size = this->stoi(scale);
    if (statement->getIRType() == ir_globalVariableDef) {
        generateMipsCode(symbol + " : .space " + to_string(size * 4), mips_globalData); // 全局的变量定义，声明地址空间
        if (size == 1) {
            this->entryTable->tranlasteIR_setGlobalVariableInfo(symbol, this->memAlloc);
        } else {
            this->entryTable->translateIR_setGlobalArrayInfo(symbol, size, this->memAlloc);
        }
    } else {
        if (size == 1) {
            this->entryTable->translateIR_setPartialVariableInfo(symbol, this->curFunctionName, this->curStackAlloc);
        } else {
            this->entryTable->translateIR_setPartialArrayInfo(symbol, this->curFunctionName, size, this->curStackAlloc);
        }
    }
    this->curStackAlloc += 4 * size;
}

void IRTranslator::programLabelSetTranslator(IRStatement* statement){//所有标签直接输出即可
    generateMipsCode(statement->getIR(), mips_codeLabel);
}

void IRTranslator::funcDefLabelTranslator(IRStatement* statement){
    if (!switchText) {
        generateMipsCode(".text", mips_segementSet);
        generateMipsCode("jal main", mips_jalFunction);
        generateMipsCode("j __end", mips_jLabel);
        switchText = true;
    }
    stringstream ss(statement->getIR());
    string label;
    getline(ss,label,' ');
    getline(ss,label);
    generateMipsCode(label, mips_codeLabel);
    //函数切换
    label.pop_back(); //弹掉冒号
    if (label != ".global.") {
        this->glbVariableRegisterMap = this->functionGlbRegisterMap[label];
    }// 全局寄存器表切换
    this->loadedGlbRegister.clear();
    this->loadedGlbVariable.clear();
    this->curFunctionName = label;
    this->curStackAlloc = 0;
}

void IRTranslator::funcDefParaTranslator(IRStatement* statement){//函数定义中的参数
    stringstream ss(statement->getIR());
    string symbol;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    this->entryTable->translateIR_setPartialVariableInfo(symbol, this->curFunctionName, this->curStackAlloc);
    this->curStackAlloc +=  4;//每个参数都占四个字节
    if (this->glbVariableRegisterMap.find(symbol) != this->glbVariableRegisterMap.end()) { // 参数使用的是全局寄存器先把变量的数值提取出来以免后续问题
        loadVariable(symbol, this->glbVariableRegisterMap[symbol]);
    }
}

void IRTranslator::funcCallBeginTranslator() {
    unsigned int addr = sceneMemoryAlloc();
    this->sceneMemory = addr ;
    this->pushParaStack = addr + 4; //为返回值留出保存地址
}

void IRTranslator::funcRetAddrSwTranslator() {
    generateMipsCode("sw $ra,0($sp)", mips_swRegImm); //存储返回地址
    this->curStackAlloc += 4;//地址占用栈中一个字空间
}

void IRTranslator::funcCallParaTranslator(IRStatement* statement){
    string paraVar;
    stringstream ss(statement->getIR());
    getline(ss,paraVar,' ');
    getline(ss,paraVar,' ');
    string reg;
    if (isConstVariable(paraVar)) {
        generateMipsCode("li $t8," + to_string(this->stoi(paraVar)), mips_loadImm);
        reg = "t8";
    } else {
        reg = to_string(applyRegister(paraVar, true));
    }
    generateMipsCode("sw $" + reg + ",-" + to_string(this->pushParaStack) + "($sp)", mips_swRegImm);
    this->pushParaStack += 4;
}

void IRTranslator::funcCallTranslator(IRStatement *statement, unsigned int beginloc) {
    string retVar;
    string functionName;
    stringstream ss(statement->getIR());
    getline(ss,functionName,' ');
    getline(ss,functionName,' ');
    getline(ss,retVar,' ');
    sceneAnalysis(beginloc);
    sceneClear();
    sceneStore(functionName);
    generateMipsCode("sw $sp,0($gp)", mips_swRegImm);
    generateMipsCode("addi $gp,$gp,4", mips_addImm);
    generateMipsCode("subi $sp,$sp," + to_string(this->sceneMemory), mips_subImm);
    generateMipsCode("jal " + functionName, mips_jalFunction);
    generateMipsCode("subi $gp,$gp,4", mips_subImm);
    generateMipsCode("lw $sp,0($gp)", mips_lwRegImm);
    sceneLoad(functionName);//回复现场
    if (statement->getIRType() == ir_valueReturnFuncCall) {
        RegisterNumber reg;
        reg = applyRegister(retVar, false); // 一个变量直接从寄存器中申请寄存器
        generateMipsCode("move $" + to_string(reg) + ",$v0", mips_moveReg);
        flushVariable(retVar, reg);
    }
}

void IRTranslator::retStmtTranslator(IRStatement *statement) {
    if (statement->getIRType() == ir_valueReturn) {
        string retVar;
        stringstream ss(statement->getIR());
        getline(ss,retVar,' ');
        getline(ss,retVar,' ');
        if (isConstVariable(retVar)) {
            retVar = to_string(this->stoi(retVar));
            generateMipsCode("li $v0," + retVar, mips_loadImm);
        } else {
            RegisterNumber reg = applyRegister(retVar, true);
            generateMipsCode("move $v0,$" + to_string(reg), mips_moveReg);
        }
    }
    generateMipsCode("lw $ra,0($sp)", mips_lwRegImm);
    generateMipsCode("jr $ra", mips_jReg);
}

void IRTranslator::readFunctionTranslator(IRStatement *statement) {
    string syscallParameter;
    if (statement->getIRType() == ir_readChar) {
        syscallParameter = "12";
    } else {
        syscallParameter = "5";
    }
    generateMipsCode("li $v0," + syscallParameter, mips_loadImm);
    generateMipsCode("syscall", mips_syscall);
    string variable;
    stringstream ss(statement->getIR());
    getline(ss, variable, ' ');
    getline(ss, variable, ' ');
    RegisterNumber reg = applyRegister(variable, false);
    generateMipsCode("move $" + to_string(reg) + ",$v0", mips_moveReg);
    flushVariable(variable, reg); // 读到的变量更新到内存中
}

void IRTranslator::printFunctionTranslator(IRStatement* statement) {
    string variable;
    stringstream ss(statement->getIR());
    getline(ss, variable, ' ');
    getline(ss, variable, ' ');
    if (statement->getIRType() == ir_printStr) {
        generateMipsCode("la $a0," + variable, mips_loadAddr);
    } else if (statement->getIRType() == ir_printNewLine) {
        generateMipsCode(R"(li $a0 '\n')", mips_loadImm);
    } else {
        if (isConstVariable(variable)) {
            variable = to_string(this->stoi(variable));
            generateMipsCode("li $a0," + variable, mips_loadImm);
        } else {
            RegisterNumber reg = applyRegister(variable, true);
            generateMipsCode("move $a0,$" + to_string(reg), mips_moveReg);
        }
    }
    string syscallParameter;
    if (statement->getIRType() == ir_printInt) {
        syscallParameter = "1";
    } else if (statement->getIRType() == ir_printChar || statement->getIRType() == ir_printNewLine) {
        syscallParameter = "11";
    } else {
        syscallParameter = "4";
    }
    generateMipsCode("li $v0," + syscallParameter, mips_loadImm);
    generateMipsCode("syscall", mips_syscall);
}

//表达式计算中没有改变变量的数值
void IRTranslator::operationTranslator(IRStatement* statement) {
    string op,opnum1,opnum2,ansSym;
    stringstream ss(statement->getIR());
    getline(ss,op,',');
    getline(ss,opnum1,',');
    getline(ss,opnum2,',');
    getline(ss,ansSym);
    string mips;
    if (op == "[]") { // 取数组元素
        if (this->entryTable->translateIR_isPartialEntry(opnum2, this->curFunctionName)) {
            unsigned int addr = this->entryTable->translateIR_getPartialArrayAddr(opnum2, this->curFunctionName);
            if (isConstVariable(opnum1)) {//index不可能是单独的字符常量，此时下标是一个常量
                string ansReg = to_string(applyRegister(ansSym, false));
                unsigned int indexnum;
                indexnum = this->stoi(opnum1);
                addr += indexnum * 4;
                generateMipsCode("lw $" + ansReg + ",-" + to_string(addr) + "($sp)", mips_lwRegImm);
                flushVariable(ansSym, this->stoi(ansReg));
            } else { //说明index是一个变量
                //此时需要同时申请结果和下标的寄存器，两者是互斥的
                vector<string> variables = {ansSym, opnum1};
                vector<bool> needLoads = {false, true};
                vector<RegisterNumber> regs = applyRegister(variables, needLoads);
                string regIndex = to_string(regs[1]);
                string ansReg = to_string(regs[0]);
                generateMipsCode("sll $t8,$" + regIndex + ",2", mips_shiftLeft);
                generateMipsCode("addi $t8,$t8," + to_string(addr), mips_addImm);
                generateMipsCode("sub $t8,$sp,$t8", mips_subReg);
                generateMipsCode("lw $" + ansReg + ",0($t8)", mips_lwRegImm);
                flushVariable(ansSym, this->stoi(ansReg));
            }
        } else if (this->entryTable->translateIR_isGlobalEntry(opnum2)) {
            if (isConstVariable(opnum1)) {
                string ansReg = to_string(applyRegister(ansSym, false));
                unsigned int indexnum;
                indexnum = this->stoi(opnum1);
                indexnum = indexnum * 4;
                generateMipsCode("lw $" + ansReg + "," + opnum2 + " + " + to_string(indexnum), mips_lwLabelImm);
                flushVariable(ansSym, this->stoi(ansReg));
            } else {
                vector<string> variables = {ansSym, opnum1};
                vector<bool> needLoads = {false, true};
                vector<RegisterNumber> regs = applyRegister(variables, needLoads);
                string regIndex = to_string(regs[1]);
                string ansReg = to_string(regs[0]);
                generateMipsCode("sll $t8,$" + regIndex + ",2", mips_shiftLeft);
                generateMipsCode("lw $" + ansReg + "," + opnum2 + "($t8)", mips_lwRegLabel);
                flushVariable(ansSym, this->stoi(ansReg));
            }
        } else generateMipsCode("error", mips_syscall);
        return;
    }
    if (opnum1.empty()) opnum1 = "0";
    if (isConstVariable(opnum1)) {
        opnum1 = to_string(this->stoi(opnum1));
        generateMipsCode("li $t8," + opnum1, mips_loadImm);
        if (isConstVariable(opnum2)) {
            opnum2 = to_string(this->stoi(opnum2));
            generateMipsCode("li $t9," + opnum2, mips_loadImm);
            RegisterNumber ansReg = applyRegister(ansSym, false);
            switch (op[0]) {
                case('+') :
                    generateMipsCode("add $" + to_string(ansReg) + ",$t8,$t9", mips_addReg);
                    break;
                case('-') :
                    generateMipsCode("sub $" + to_string(ansReg) + ",$t8,$t9", mips_subReg);
                    break;
                case('*') :
                    generateMipsCode("mul $" + to_string(ansReg) + ",$t8,$t9", mips_mulReg);
                    break;
                case('/') :
                    generateMipsCode("div $" + to_string(ansReg) + ",$t8,$t9", mips_divReg);
                    break;
                default:
                    generateMipsCode("error", mips_syscall);
                    break;
            }
            flushVariable(ansSym, ansReg);
        } else {
            vector<string> variables = {opnum2, ansSym};
            vector<bool> needLoads = {true, false};
            vector<RegisterNumber> regs = applyRegister(variables, needLoads);
            switch (op[0]) {
                case('+') :
                    generateMipsCode("add $" + to_string(regs[1]) + ",$t8,$" + to_string(regs[0]), mips_addReg);
                    break;
                case('-') :
                    generateMipsCode("sub $" + to_string(regs[1]) + ",$t8,$" + to_string(regs[0]), mips_subReg);
                    break;
                case('*') :
                    generateMipsCode("mul $" + to_string(regs[1]) + ",$t8,$" + to_string(regs[0]), mips_mulReg);
                    break;
                case('/') :
                    generateMipsCode("div $" + to_string(regs[1]) + ",$t8,$" + to_string(regs[0]), mips_divReg);
                    break;
                default:
                    generateMipsCode("error", mips_syscall);
                    break;
            }
            flushVariable(ansSym, regs[1]);
        }
    } else {
        if (isConstVariable(opnum2)) {
            opnum2 = to_string(this->stoi(opnum2));
            generateMipsCode("li $t9," + opnum2, mips_loadImm);
            vector<string> variables = {opnum1, ansSym};
            vector<bool> needLoads = {true, false};
            vector<RegisterNumber> regs = applyRegister(variables, needLoads);
            switch (op[0]) {
                case('+') :
                    generateMipsCode("add $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$t9", mips_addReg);
                    break;
                case('-') :
                    generateMipsCode("sub $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$t9", mips_subReg);
                    break;
                case('*') :
                    generateMipsCode("mul $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$t9", mips_mulReg);
                    break;
                case('/') :
                    generateMipsCode("div $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$t9", mips_divReg);
                    break;
                default:
                    generateMipsCode("error", mips_syscall);
                    break;
            }
            flushVariable(ansSym, regs[1]);
        } else {
            vector<string> variables = {opnum1, opnum2, ansSym};
            vector<bool> needLoads = {true, true, false};
            vector<RegisterNumber> regs = applyRegister(variables, needLoads);
            switch (op[0]) {
                case('+') :
                    generateMipsCode("add $" + to_string(regs[2]) + ",$" + to_string(regs[0]) + ",$" + to_string(regs[1]), mips_addReg);
                    break;
                case('-') :
                    generateMipsCode("sub $" + to_string(regs[2]) + ",$" + to_string(regs[0]) + ",$" + to_string(regs[1]), mips_subReg);
                    break;
                case('*') :
                    generateMipsCode("mul $" + to_string(regs[2]) + ",$" + to_string(regs[0]) + ",$" + to_string(regs[1]), mips_mulReg);
                    break;
                case('/') :
                    generateMipsCode("div $" + to_string(regs[2]) + ",$" + to_string(regs[0]) + ",$" + to_string(regs[1]), mips_divReg);
                    break;
                default:
                    generateMipsCode("error", mips_syscall);
                    break;
            }
            flushVariable(ansSym, regs[2]);
        }
    }
}

void IRTranslator::assignTranslator(IRStatement* statement){//同时赋值和改变寄存器
    stringstream ss(statement->getIR());
    string left,right;
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,right,' ');
    if (isConstVariable(right)) { // 右侧是常量
        right = to_string(this->stoi(right));
        if (left.find('[') != std::string::npos) { // 左侧是数组
            string array, index;
            stringstream s(left);
            getline(s, array, '[');
            getline(s, index, ']');
            #ifdef debug
            cout << "assign statement : left :array :" << array << " index : " << index << endl;
            #endif
            generateMipsCode("li $t9," + right, mips_loadImm); // t9作为中介寄存器，存放右侧数据的数值
            if (isConstVariable(index)) { //索引是常量
                unsigned int indexNumber = this->stoi(index);
                if (this->entryTable->translateIR_isPartialEntry(array, this->curFunctionName)) {
                    unsigned int addr = this->entryTable->translateIR_getPartialArrayAddr(array, this->curFunctionName);
                    addr += indexNumber * 4;
                    generateMipsCode("sw $t9,-" + to_string(addr) + "($sp)", mips_swRegImm);
                } else if (this->entryTable->translateIR_isGlobalEntry(array)) {
                    indexNumber *= 4;
                    generateMipsCode("sw $t9," + array + "+" + to_string(indexNumber), mips_swLabelImm);
                } else generateMipsCode("error", mips_syscall);
            } else { //索引是变量
                RegisterNumber indexValueReg = applyRegister(index, true);
                generateMipsCode("sll $t8,$" + to_string(indexValueReg) + ",2", mips_shiftLeft); //index左移两位
                if (this->entryTable->translateIR_isPartialEntry(array, this->curFunctionName)) {
                    unsigned int addr = this->entryTable->translateIR_getPartialArrayAddr(array, this->curFunctionName);
                    generateMipsCode("addi $t8,$t8," + to_string(addr), mips_addImm); //index寄存器中地址先加上数组的基地址
                    generateMipsCode("sub $t8,$sp,$t8", mips_subReg); //由于是在栈中，为了下面使用寄存器直接寻址做准备
                    generateMipsCode("sw $t9,0($t8)", mips_swRegImm);
                } else if (this->entryTable->translateIR_isGlobalEntry(array)) {
                    generateMipsCode("sw $t9," + array + ",($t8)", mips_swRegLabel);
                } else generateMipsCode("error", mips_syscall);
            }
        } else {
            RegisterNumber leftReg = applyRegister(left, false); //为左侧变量申请一个寄存器
            generateMipsCode("li $" + to_string(leftReg) + "," + right, mips_moveReg);
            flushVariable(left,leftReg); // 如果左侧是声明的变量不是中间变量那么需要更新数值
        }
    } else { //右侧是变量
        if (left.find('[') != std::string::npos) { // 左侧是数组
            string array, index;
            stringstream s(left);
            getline(s, array, '[');
            getline(s, index, ']');
            #ifdef debug
            cout << "assign statement : left :array :" << array << " index : " << index << endl;
            #endif
            if (isConstVariable(index)) { //索引是常量
                RegisterNumber rightValueReg = applyRegister(right, true);
                unsigned int indexNumber = this->stoi(index);
                if (this->entryTable->translateIR_isPartialEntry(array, this->curFunctionName)) {
                    unsigned int addr = this->entryTable->translateIR_getPartialArrayAddr(array, this->curFunctionName);
                    addr += indexNumber * 4;
                    generateMipsCode("sw $" + to_string(rightValueReg) + ",-" + to_string(addr) + "($sp)", mips_swRegImm);
                } else if (this->entryTable->translateIR_isGlobalEntry(array)) {
                    indexNumber *= 4;
                    generateMipsCode("sw $" + to_string(rightValueReg) + "," + array + "+" + to_string(indexNumber), mips_swLabelImm);
                } else generateMipsCode("error", mips_syscall);
            } else {
                vector<string> variables = {index, right};
                vector<bool> needLoads = {true, true};
                vector<RegisterNumber> regs = applyRegister(variables, needLoads);
                RegisterNumber indexValueReg = regs[0];
                RegisterNumber rightValueReg = regs[1];
                generateMipsCode("sll $t8,$" + to_string(indexValueReg) + ",2", mips_shiftLeft); //index左移两位
                if (this->entryTable->translateIR_isPartialEntry(array, this->curFunctionName)) {
                    unsigned int addr = this->entryTable->translateIR_getPartialArrayAddr(array, this->curFunctionName);
                    generateMipsCode("addi $t8,$t8," + to_string(addr), mips_addImm); //index寄存器中地址先加上数组的基地址
                    generateMipsCode("sub $t8,$sp,$t8", mips_subReg); //由于是在栈中，为了下面使用寄存器直接寻址做准备
                    generateMipsCode("sw $" + to_string(rightValueReg) + ",0($t8)", mips_swRegImm);
                } else if (this->entryTable->translateIR_isGlobalEntry(array)) {
                    generateMipsCode("sw $" + to_string(rightValueReg) + "," + array + ",($t8)", mips_swRegLabel);
                } else generateMipsCode("error", mips_syscall);
            }
        } else { //左侧是变量
            vector<string> variables = {left, right}; // 此时左右两侧都是变量
            vector<bool> needLoads = {false, true};
            vector<RegisterNumber> regs = applyRegister(variables, needLoads);
            generateMipsCode("move $" + to_string(regs[0]) + ",$" + to_string(regs[1]), mips_moveReg);
            flushVariable(left, regs[0]);
        }
    }
}

void IRTranslator::conditionJumpTranslator(IRStatement *statement) {
    string left,right,label;
    stringstream ss(statement->getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    string cmpMips;
    if (isConstVariable(left) || isConstVariable(right)) {
        if (isConstVariable(left) && isConstVariable(right)) { // 两个比较数都是常数
            if (right == "0") {
                left = to_string(this->stoi(left));
                generateMipsCode("li $t8," + left, mips_loadImm); // t8是左侧比较数
                if (statement->getIRType() == ir_geqJump) { // 大于等于跳转
                    cmpMips = "bgez";
                } else if (statement->getIRType() == ir_greJump) { // 大于跳转
                    cmpMips = "bgtz";
                } else if (statement->getIRType() == ir_leqJump) { // 小于等于跳转
                    cmpMips = "blez";
                } else if (statement->getIRType() == ir_lesJump) { // 小于跳转
                    cmpMips = "bltz";
                } else if (statement->getIRType() == ir_eqlJump) { // 等于跳转
                    cmpMips = "beqz";
                } else { // 不等于跳转
                    cmpMips = "bnez";
                }
                generateMipsCode(cmpMips + " $t8," + label, mips_branchZero);
            } else {
                if (left == "0") {
                    //注意此时有左侧数是0所以比较语句要颠倒
                    right = to_string(this->stoi(right));
                    generateMipsCode("li $t8," + right, mips_loadImm); // t8是右侧比较数
                    if (statement->getIRType() == ir_geqJump) { // 大于等于跳转
                        cmpMips = "blez";
                    } else if (statement->getIRType() == ir_greJump) { // 大于跳转
                        cmpMips = "bltz";
                    } else if (statement->getIRType() == ir_leqJump) { // 小于等于跳转
                        cmpMips = "bgez";
                    } else if (statement->getIRType() == ir_lesJump) { // 小于跳转
                        cmpMips = "bgtz";
                    } else if (statement->getIRType() == ir_eqlJump) { // 等于跳转
                        cmpMips = "beqz";
                    } else { // 不等于跳转
                        cmpMips = "bnez";
                    }
                    generateMipsCode(cmpMips + " $t8," + label, mips_branchZero);
                } else { // 两个比较数都是非零常数
                    generateMipsCode("li $t8," + left, mips_loadImm);
                    if (statement->getIRType() == ir_geqJump) { // 大于等于跳转
                        cmpMips = "bge";
                    } else if (statement->getIRType() == ir_greJump) { // 大于跳转
                        cmpMips = "bgt";
                    } else if (statement->getIRType() == ir_leqJump) { // 小于等于跳转
                        cmpMips = "ble";
                    } else if (statement->getIRType() == ir_lesJump) { // 小于跳转
                        cmpMips = "blt";
                    } else if (statement->getIRType() == ir_eqlJump) { // 等于跳转
                        cmpMips = "beq";
                    } else { // 不等于跳转
                        cmpMips = "bne";
                    }
                    generateMipsCode(cmpMips + " $t8," + right + " " + label, mips_branchZero);
                }
            }
        } else { // 有一个比较数是变量
            if (right == "0") {
                left = to_string(applyRegister(left, true)); // 左侧是变量获取其寄存器
                if (statement->getIRType() == ir_geqJump) { // 大于等于跳转
                    cmpMips = "bgez";
                } else if (statement->getIRType() == ir_greJump) { // 大于跳转
                    cmpMips = "bgtz";
                } else if (statement->getIRType() == ir_leqJump) { // 小于等于跳转
                    cmpMips = "blez";
                } else if (statement->getIRType() == ir_lesJump) { // 小于跳转
                    cmpMips = "bltz";
                } else if (statement->getIRType() == ir_eqlJump) { // 等于跳转
                    cmpMips = "beqz";
                } else { // 不等于跳转
                    cmpMips = "bnez";
                }
                generateMipsCode(cmpMips + " $" + left + "," + label, mips_branchZero);
            } else if (left == "0"){
                right = to_string(applyRegister(right, true)); // 右侧侧是变量获取其寄存器
                if (statement->getIRType() == ir_geqJump) { // 大于等于跳转
                    cmpMips = "blez";
                } else if (statement->getIRType() == ir_greJump) { // 大于跳转
                    cmpMips = "bltz";
                } else if (statement->getIRType() == ir_leqJump) { // 小于等于跳转
                    cmpMips = "bgez";
                } else if (statement->getIRType() == ir_lesJump) { // 小于跳转
                    cmpMips = "bgtz";
                } else if (statement->getIRType() == ir_eqlJump) { // 等于跳转
                    cmpMips = "beqz";
                } else { // 不等于跳转
                    cmpMips = "bnez";
                }
                generateMipsCode(cmpMips + " $" + right + "," + label, mips_branchZero);
            } else { // 没有常数0
                if (isConstVariable(right)) { // 右侧数是常数
                    left = to_string(applyRegister(left, true)); // 左侧是变量获取其寄存器
                    right = to_string(this->stoi(right));
                    if (statement->getIRType() == ir_geqJump) { // 大于等于跳转
                        cmpMips = "bge";
                    } else if (statement->getIRType() == ir_greJump) { // 大于跳转
                        cmpMips = "bgt";
                    } else if (statement->getIRType() == ir_leqJump) { // 小于等于跳转
                        cmpMips = "ble";
                    } else if (statement->getIRType() == ir_lesJump) { // 小于跳转
                        cmpMips = "blt";
                    } else if (statement->getIRType() == ir_eqlJump) { // 等于跳转
                        cmpMips = "beq";
                    } else { // 不等于跳转
                        cmpMips = "bne";
                    }
                    generateMipsCode(cmpMips + " $" + left + "," + right + "," + label, mips_branchReg);
                } else { // 左侧数是常数
                    right = to_string(applyRegister(right, true)); // 右侧侧是变量获取其寄存器
                    left = to_string(this->stoi(left));
                    if (statement->getIRType() == ir_geqJump) { // 大于等于跳转
                        cmpMips = "ble";
                    } else if (statement->getIRType() == ir_greJump) { // 大于跳转
                        cmpMips = "blt";
                    } else if (statement->getIRType() == ir_leqJump) { // 小于等于跳转
                        cmpMips = "bge";
                    } else if (statement->getIRType() == ir_lesJump) { // 小于跳转
                        cmpMips = "bgt";
                    } else if (statement->getIRType() == ir_eqlJump) { // 等于跳转
                        cmpMips = "beq";
                    } else { // 不等于跳转
                        cmpMips = "bne";
                    }
                    generateMipsCode(cmpMips + " $" + right + "," + left + "," + label, mips_branchReg);
                }
            }
        }
        return;
    }
    // 两个比较数都是变量
    vector<string> cmps = {left, right};
    vector<bool> needLoads = {true, true};
    vector<RegisterNumber> regs = applyRegister(cmps, needLoads);
    string regl,regr;
    regl = to_string(regs[0]);
    regr = to_string(regs[1]);
    if (statement->getIRType() == ir_geqJump) { // 大于等于跳转
        cmpMips = "bge";
    } else if (statement->getIRType() == ir_greJump) { // 大于跳转
        cmpMips = "bgt";
    } else if (statement->getIRType() == ir_leqJump) { // 小于等于跳转
        cmpMips = "ble";
    } else if (statement->getIRType() == ir_lesJump) { // 小于跳转
        cmpMips = "blt";
    } else if (statement->getIRType() == ir_eqlJump) { // 等于跳转
        cmpMips = "beq";
    } else { // 不等于跳转
        cmpMips = "bne";
    }
    generateMipsCode(cmpMips + " $" + regl + ",$" + regr + "," + label, mips_branchReg);
}

void IRTranslator::jumpTranslator(IRStatement* statement){//无条件跳转，使用j指令直接跳转
    stringstream ss(statement->getIR());
    string label;
    getline(ss,label,' ');
    getline(ss,label,' ');
    generateMipsCode("j " + label, mips_jLabel);
}

void IRTranslator::translateBlk(BasicBlk* blk) {
    for (unsigned int i = 0;i < blk->size();i++) {
        IRStatement* statement = blk->operator[](i);
        switch (statement->getIRType()) {
            case ir_globalConstantDef:
            case ir_partialConstantDef:
                constantDefinitionTranslator(statement);
                break;
            case ir_globalVariableDef:
            case ir_partialVariableDef:
                variableDefinitionTranslator(statement);
                break;
            case ir_stmtLabel:
                programLabelSetTranslator(statement);
                break;
            case ir_funcDefLabel:
                funcDefLabelTranslator(statement);
                break;
            case ir_funcParameterDef:
                funcDefParaTranslator(statement);
                break;
            case ir_valueReturn:
            case ir_voidReturn:
                retStmtTranslator(statement);
                break;
            case ir_funcCallBegin:
                funcCallBeginTranslator();
                break;
            case ir_funcReturnAddrStore:
                funcRetAddrSwTranslator();
                break;
            case ir_funcCallParaPush:
                funcCallParaTranslator(statement);
                break;
            case ir_voidRetutnFuncCall:
            case ir_valueReturnFuncCall:
                funcCallTranslator(statement, i);
                break;
            case ir_readInt:
            case ir_readChar:
                readFunctionTranslator(statement);
                break;
            case ir_printNewLine:
            case ir_printStr:
            case ir_printChar:
            case ir_printInt:
                printFunctionTranslator(statement);
                break;
            case ir_operation:
                operationTranslator(statement);
                break;
            case ir_assign:
                assignTranslator(statement);
                break;
            case ir_greJump:
            case ir_geqJump:
            case ir_lesJump:
            case ir_leqJump:
            case ir_eqlJump:
            case ir_neqJmp:
                conditionJumpTranslator(statement); // 条件跳转
                break;
            case ir_jump:
                jumpTranslator(statement);
                break;
            default:
                break;
        }
    }
}
