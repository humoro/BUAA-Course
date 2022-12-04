#include "Mips.h"
#include <sstream>

static bool isCharConstantString(const string& str) {
    return str.size() == 3 && (str[0] == str[2]) && (str[0] == '\'');//字符型的常量
}

static bool isNumberConstantString(string str) {
    if (str[0] == '+' || str[0] == '-') {
        str.erase(str.begin());
    }
    for (auto c : str) {
        if (!isdigit(c))return false;
    }
    return true;
}

bool IRTranslator::isConstVariable(string &str) {//查看字符串是否代表常量
    return (isCharConstantString(str) ||
            isNumberConstantString(str) ||
            this->table->translateIR_isGlobalConstVar(str) ||
            this->table->translateIR_isPartialConstVar(str, this->curFunctionName));
}

long long IRTranslator::stringToNumber(string& str) {
    if (str.empty()) return 0;
    long long value;
    if (str.size() == 3 && str[0] == '\'' && str[2] == str[0]) {
        value = (long long)str[1];
        return value;
    } else if (this->table->translateIR_isPartialConstVar(str, this->curFunctionName)) { // 先找部分的当前程序块的const
        value = this->table->translateIR_getPartialConstValue(str, this->curFunctionName);
        return value;
    } else if (this->table->translateIR_isGlobalConstVar(str)) { // 再找部分的当前程序块的const
        value = this->table->translateIR_getGlobalConstValue(str);
        return value;
    } else {
        value = stoi(str);
        return value;
    }
}

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

void IRTranslator::constantDefinitionTranslator(IRStatement &statement) {
    stringstream ss(statement.getIR());
    string symbol;
    string value;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,value,' ');
    getline(ss,value);
    if (statement.getIRType() == gConstDef) {
        outMips(symbol + ": .word " + value);
        this->table->translateIR_setGlobalConstInfo(symbol, this->memAlloc, stringToNumber(value));//符号表中设置变量的地址
        this->memAlloc += 4;
    } else {
        this->table->translateIR_setPartialConstInfo(symbol, this->curFunctionName, this->curStackAlloc,stringToNumber(value));
        this->curStackAlloc += 4; // 局部const只分配内存不使用访存语句存储数值
    }
}

void IRTranslator::variableDefinitionTranslator(IRStatement &statement) {
    stringstream ss(statement.getIR());
    string symbol;
    string scale;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss, scale, ' ');
    unsigned int size = stringToNumber(scale);
    if (statement.getIRType() == gVarDef) {
        outMips(symbol + " : .space " + to_string(size * 4)); // 全局的变量定义，声明地址空间
        if (size == 1) {
            this->table->tranlasteIR_setGlobalVarAddr(symbol, this->memAlloc);
        } else {
            this->table->translateIR_setGlobalArrayInfo(symbol, size, this->memAlloc);
        }
    } else {
        if (size == 1) {
            this->table->translateIR_setPartialVarAddr(symbol, this->curFunctionName, this->curStackAlloc);
        } else {
            this->table->translateIR_setPartialArrayInfo(symbol, this->curFunctionName, size, this->curStackAlloc);
        }
    }
    this->curStackAlloc += 4 * size;
}

void IRTranslator::programLabelSetTranslator(IRStatement& statement){//所有标签直接输出即可
    outMips(statement.getIR());
}

void IRTranslator::funcDefLabelTranslator(IRStatement& statement){
    if (!switchText) {
        outMips(".text");
        outMips("jal main");
        outMips("j __end");
        switchText = true;
    }
    stringstream ss(statement.getIR());
    string label;
    getline(ss,label,' ');
    getline(ss,label);
    outMips(label);
    //函数切换
    label.pop_back(); //弹掉冒号
    this->curFunctionName = label;
    this->curStackAlloc = 0;
}

void IRTranslator::funcDefParaTranslator(IRStatement& statement){//函数定义中的参数
    stringstream ss(statement.getIR());
    string symbol;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    this->table->translateIR_setPartialVarAddr(symbol, this->curFunctionName, this->curStackAlloc);
    this->curStackAlloc +=  4;//每个参数都占四个字节
}

void IRTranslator::funcCallBeginTranslator(IRStatement &statement) {
    unsigned int addr = sceneMemoryAlloc();
    this->sceneMemory = addr ;
    this->pushParaStack = addr + 4; //为返回值留出保存地址
}

void IRTranslator::funcRetAddrSwTranslator() {
    outMips("sw $ra,0($sp)"); //存储返回地址
    this->curStackAlloc += 4;//地址占用栈中一个字空间
}

void IRTranslator::funcCallParaTranslator(IRStatement& statement){
    string paraVar;
    stringstream ss(statement.getIR());
    getline(ss,paraVar,' ');
    getline(ss,paraVar,' ');
    string reg;
    if (isConstVariable(paraVar)) {
        outMips("li $t8," + to_string(stringToNumber(paraVar)));
        reg = "t8";
    } else {
        reg = to_string(tmpRegisterPool(paraVar, true));
    }
    outMips("sw $" + reg + ",-" + to_string(this->pushParaStack) + "($sp)");
    this->pushParaStack += 4;
}

void IRTranslator::funcCallTranslator(IRStatement &statement, unsigned int beginloc) {
    string retVar;
    string functionName;
    stringstream ss(statement.getIR());
    getline(ss,functionName,' ');
    getline(ss,functionName,' ');
    getline(ss,retVar,' ');
    sceneAnalysis(beginloc);
    sceneClear();
    sceneStore();
    outMips("sw $sp,0($gp)");
    outMips("addi $gp,$gp,4");
    outMips("subi $sp,$sp," + to_string(this->sceneMemory));
    outMips("jal " + functionName);
    outMips("subi $gp,$gp,4");
    outMips("lw $sp,0($gp)");
    sceneLoad();//回复现场
    if (statement.getIRType() == retFuncCall) {
        unsigned int reg;
        reg = tmpRegisterPool(retVar, false); // 一个变量直接从寄存器中申请寄存器
        outMips("move $" + to_string(reg) + ",$v0");
        flushVariable(retVar, reg);
    }
}

void IRTranslator::retStmtTranslator(IRStatement &statement) {
    if (statement.getIRType() == valueRet) {
        string retVar;
        stringstream ss(statement.getIR());
        getline(ss,retVar,' ');
        getline(ss,retVar,' ');
        if (isConstVariable(retVar)) {
            retVar = to_string(stringToNumber(retVar));
            outMips("li $v0," + retVar);
        } else {
            unsigned int reg = tmpRegisterPool(retVar, true);
            outMips("move $v0,$" + to_string(reg));
        }
    }
    outMips("lw $ra,0($sp)");
    outMips("jr $ra");
}

void IRTranslator::readFunctionTranslator(IRStatement &statement) {
    string syscallParameter;
    if (statement.getIRType() == readChar) {
        syscallParameter = "12";
    } else {
        syscallParameter = "5";
    }
    outMips("li $v0," + syscallParameter);
    outMips("syscall");
    string variable;
    stringstream ss(statement.getIR());
    getline(ss, variable, ' ');
    getline(ss, variable, ' ');
    regNum reg = tmpRegisterPool(variable, false);
    outMips("move $" + to_string(reg) + ",$v0");
    flushVariable(variable, reg); // 读到的变量更新到内存中
}

void IRTranslator::printFunctionTranslator(IRStatement& statement) {
    string variable;
    stringstream ss(statement.getIR());
    getline(ss, variable, ' ');
    getline(ss, variable, ' ');
    if (statement.getIRType() == printStr) {
        outMips("la $a0," + variable);
    } else if (statement.getIRType() == printNewLine) {
        outMips(R"(li $a0 '\n')");
    } else {
        if (isConstVariable(variable)) {
            variable = to_string(stringToNumber(variable));
            outMips("li $a0," + variable);
        } else {
            regNum reg = tmpRegisterPool(variable, true);
            outMips("move $a0,$" + to_string(reg));
        }
    }
    string syscallParameter;
    if (statement.getIRType() == printInt) {
        syscallParameter = "1";
    } else if (statement.getIRType() == printChar || statement.getIRType() == printNewLine) {
        syscallParameter = "11";
    } else {
        syscallParameter = "4";
    }
    outMips("li $v0," + syscallParameter);
    outMips("syscall");
}

//表达式计算中没有改变变量的数值
void IRTranslator::operationTranslator(IRStatement& statement) {
    string op,opnum1,opnum2,ansSym;
    stringstream ss(statement.getIR());
    getline(ss,op,',');
    getline(ss,opnum1,',');
    getline(ss,opnum2,',');
    getline(ss,ansSym);
    string mips;
    if (op == "[]") { // 取数组元素
        if (this->table->translateIR_isPartialVariable(opnum2, this->curFunctionName)) {
            unsigned int addr = this->table->translateIR_getPartialArrayAddr(opnum2, this->curFunctionName);
            if (isConstVariable(opnum1)) {//index不可能是单独的字符常量，此时下标是一个常量
                string ansReg = to_string(tmpRegisterPool(ansSym, false));
                unsigned int indexnum;
                indexnum = stringToNumber(opnum1);
                addr += indexnum * 4;
                outMips("lw $" + ansReg + ",-" + to_string(addr) + "($sp)");
                flushVariable(ansSym, stringToNumber(ansReg));
            } else { //说明index是一个变量
                //此时需要同时申请结果和下标的寄存器，两者是互斥的
                vector<string> variables = {ansSym, opnum1};
                vector<bool> needLoads = {false, true};
                vector<regNum> regs = applyTmpRegister(variables, needLoads);
                string regIndex = to_string(regs[1]);
                string ansReg = to_string(regs[0]);
                outMips("sll $t8,$" + regIndex + ",2");
                outMips("addi $t8,$t8," +  to_string(addr));
                outMips("sub $t8,$sp,$t8");
                outMips("lw $" + ansReg + ",0($t8)");
                flushVariable(ansSym, stringToNumber(ansReg));
            }
        } else if (this->table->translateIR_isGlobalVariable(opnum2)) {
            if (isConstVariable(opnum1)) {
                string ansReg = to_string(tmpRegisterPool(ansSym, false));
                unsigned int indexnum;
                indexnum = stringToNumber(opnum1);
                indexnum = indexnum * 4;
                outMips("lw $" + ansReg + "," + opnum2 + " + " + to_string(indexnum));
                flushVariable(ansSym, stringToNumber(ansReg));
            } else {
                vector<string> variables = {ansSym, opnum1};
                vector<bool> needLoads = {false, true};
                vector<regNum> regs = applyTmpRegister(variables, needLoads);
                string regIndex = to_string(regs[1]);
                string ansReg = to_string(regs[0]);
                outMips("sll $t8,$" + regIndex + ",2");
                outMips("lw $" + ansReg + "," + opnum2 + "($t8)");
                flushVariable(ansSym, stringToNumber(ansReg));
            }
        } else outMips("error");
        return;
    }
    if (opnum1.empty()) opnum1 = "0";
    if (isConstVariable(opnum1)) {
        opnum1 = to_string(stringToNumber(opnum1));
        outMips("li $t8," + opnum1);
        if (isConstVariable(opnum2)) {
            opnum2 = to_string(stringToNumber(opnum2));
            outMips("li $t9," + opnum2);
            regNum ansReg = tmpRegisterPool(ansSym, false);
            switch (op[0]) {
                case('+') : outMips("addu $" + to_string(ansReg) + ",$t8,$t9");
                    break;
                case('-') : outMips("sub $" + to_string(ansReg) + ",$t8,$t9");
                    break;
                case('*') : outMips("mul $" + to_string(ansReg) + ",$t8,$t9");
                    break;
                case('/') : outMips("div $" + to_string(ansReg) + ",$t8,$t9");
                    break;
                default:    outMips("error");
                    break;
            }
            flushVariable(ansSym, ansReg);
        } else {
            vector<string> variables = {opnum2, ansSym};
            vector<bool> needLoads = {true, false};
            vector<regNum> regs = applyTmpRegister(variables, needLoads);
            switch (op[0]) {
                case('+') : outMips("addu $" + to_string(regs[1]) + ",$t8,$" + to_string(regs[0]));
                    break;
                case('-') : outMips("sub $" + to_string(regs[1]) + ",$t8,$" + to_string(regs[0]));
                    break;
                case('*') : outMips("mul $" + to_string(regs[1]) + ",$t8,$" + to_string(regs[0]));
                    break;
                case('/') : outMips("div $" + to_string(regs[1]) + ",$t8,$" + to_string(regs[0]));
                    break;
                default:    outMips("error");
                    break;
            }
            flushVariable(ansSym, regs[1]);
        }
    } else {
        if (isConstVariable(opnum2)) {
            opnum2 = to_string(stringToNumber(opnum2));
            outMips("li $t9," + opnum2);
            vector<string> variables = {opnum1, ansSym};
            vector<bool> needLoads = {true, false};
            vector<regNum> regs = applyTmpRegister(variables, needLoads);
            switch (op[0]) {
                case('+') : outMips("addu $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$t9");
                    break;
                case('-') : outMips("sub $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$t9");
                    break;
                case('*') : outMips("mul $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$t9");
                    break;
                case('/') : outMips("div $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$t9");
                    break;
                default:    outMips("error");
                    break;
            }
            flushVariable(ansSym, regs[1]);
        } else {
            vector<string> variables = {opnum1, opnum2, ansSym};
            vector<bool> needLoads = {true, true, false};
            vector<regNum> regs = applyTmpRegister(variables, needLoads);
            switch (op[0]) {
                case('+') : outMips("addu $" + to_string(regs[2]) + ",$" + to_string(regs[0]) + ",$" + to_string(regs[1]));
                    break;
                case('-') : outMips("sub $" + to_string(regs[2]) + ",$" + to_string(regs[0]) + ",$" + to_string(regs[1]));
                    break;
                case('*') : outMips("mul $" + to_string(regs[2]) + ",$" + to_string(regs[0]) + ",$" + to_string(regs[1]));
                    break;
                case('/') : outMips("div $" + to_string(regs[2]) + ",$" + to_string(regs[0]) + ",$" + to_string(regs[1]));
                    break;
                default:    outMips("error");
                    break;
            }
            flushVariable(ansSym, regs[2]);
        }
    }
}

void IRTranslator::assignTranslator(IRStatement& statement){//同时赋值和改变寄存器
    stringstream ss(statement.getIR());
    string left,right;
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,right,' ');
    if (isConstVariable(right)) { // 右侧是常量
        right = to_string(stringToNumber(right));
        outMips("li $t9," + right); // t9作为中介寄存器，存放右侧数据的数值
        if (left.find('[') != std::string::npos) { // 左侧是数组
            string array, index;
            stringstream s(left);
            getline(s, array, '[');
            getline(s, index, ']');
            #ifdef debug
            cout << "assign statement : left :array :" << array << " index : " << index << endl;
            #endif
            if (isConstVariable(index)) { //索引是常量
                unsigned int indexNumber = stringToNumber(index);
                if (this->table->translateIR_isPartialVariable(array, this->curFunctionName)) {
                    unsigned int addr = this->table->translateIR_getPartialArrayAddr(array, this->curFunctionName);
                    addr += indexNumber * 4;
                    outMips("sw $t9,-" + to_string(addr) + "($sp)");
                } else if (this->table->translateIR_isGlobalVariable(array)) {
                    indexNumber *= 4;
                    outMips("sw $t9," + array + "+" + to_string(indexNumber));
                } else outMips("error");
            } else { //索引是变量
                regNum indexValueReg = tmpRegisterPool(index, true);
                outMips("sll $t8,$" + to_string(indexValueReg) + ",2"); //index左移两位
                if (this->table->translateIR_isPartialVariable(array, this->curFunctionName)) {
                    unsigned int addr = this->table->translateIR_getPartialArrayAddr(array, this->curFunctionName);
                    outMips("addi $t8,$t8," + to_string(addr)); //index寄存器中地址先加上数组的基地址
                    outMips("sub $t8,$sp,$t8"); //由于是在栈中，为了下面使用寄存器直接寻址做准备
                    outMips("sw $t9,($t8)");
                } else if (this->table->translateIR_isGlobalVariable(array)) {
                    outMips("sw $t9," + array + ",($t8)");
                } else outMips("error");
            }
        } else {
            regNum leftReg = tmpRegisterPool(left, false); //为左侧变量申请一个寄存器
            outMips("move $" + to_string(leftReg) + ",$t9");
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
                regNum rightValueReg = tmpRegisterPool(right, true);
                unsigned int indexNumber = stringToNumber(index);
                if (this->table->translateIR_isPartialVariable(array, this->curFunctionName)) {
                    unsigned int addr = this->table->translateIR_getPartialArrayAddr(array, this->curFunctionName);
                    addr += indexNumber * 4;
                    outMips("sw $" + to_string(rightValueReg) + ",-" + to_string(addr) + "($sp)");
                } else if (this->table->translateIR_isGlobalVariable(array)) {
                    indexNumber *= 4;
                    outMips("sw $" + to_string(rightValueReg) + "," + array + "+" + to_string(indexNumber));
                } else outMips("error");
            } else {
                vector<string> variables = {index, right};
                vector<bool> needLoads = {true, true};
                vector<regNum> regs = applyTmpRegister(variables, needLoads);
                regNum indexValueReg = regs[0];
                regNum rightValueReg = regs[1];
                outMips("sll $t8,$" + to_string(indexValueReg) + ",2"); //index左移两位
                if (this->table->translateIR_isPartialVariable(array, this->curFunctionName)) {
                    unsigned int addr = this->table->translateIR_getPartialArrayAddr(array, this->curFunctionName);
                    outMips("addi $t8,$t8," + to_string(addr)); //index寄存器中地址先加上数组的基地址
                    outMips("sub $t8,$sp,$t8"); //由于是在栈中，为了下面使用寄存器直接寻址做准备
                    outMips("sw $" + to_string(rightValueReg) + ",($t8)");
                } else if (this->table->translateIR_isGlobalVariable(array)) {
                    outMips("sw $" + to_string(rightValueReg) + "," + array + ",($t8)");
                } else outMips("error");
            }
        } else { //左侧是变量
            vector<string> variables = {left, right}; // 此时左右两侧都是变量
            vector<bool> needLoads = {false, true};
            vector<regNum> regs = applyTmpRegister(variables, needLoads);
            outMips("move $" + to_string(regs[0]) + ",$" + to_string(regs[1]));
            flushVariable(left, regs[0]);
        }
    }
}

void IRTranslator::conditionJumpTranslator(IRStatement &statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    string cmpMips;
    if (isConstVariable(left) || isConstVariable(right)) {
        if (isConstVariable(left) && isConstVariable(right)) { // 两个比较数都是常数
            if (right == "0") {
                left = to_string(stringToNumber(left));
                outMips("li $t8," + left); // t8是左侧比较数
                if (statement.getIRType() == geqJump) { // 大于等于跳转
                    cmpMips = "bgez";
                } else if (statement.getIRType() == greJump) { // 大于跳转
                    cmpMips = "bgtz";
                } else if (statement.getIRType() == leqJump) { // 小于等于跳转
                    cmpMips = "blez";
                } else if (statement.getIRType() == lesJump) { // 小于跳转
                    cmpMips = "bltz";
                } else if (statement.getIRType() == eqlJump) { // 等于跳转
                    cmpMips = "beqz";
                } else { // 不等于跳转
                    cmpMips = "bnez";
                }
                outMips(cmpMips + " $t8," + label);
            } else {
                if (left == "0") {
                    //注意此时有左侧数是0所以比较语句要颠倒
                    right = to_string(stringToNumber(right));
                    outMips("li $t8," + right); // t8是右侧比较数
                    if (statement.getIRType() == geqJump) { // 大于等于跳转
                        cmpMips = "blez";
                    } else if (statement.getIRType() == greJump) { // 大于跳转
                        cmpMips = "bltz";
                    } else if (statement.getIRType() == leqJump) { // 小于等于跳转
                        cmpMips = "bgez";
                    } else if (statement.getIRType() == lesJump) { // 小于跳转
                        cmpMips = "bgtz";
                    } else if (statement.getIRType() == eqlJump) { // 等于跳转
                        cmpMips = "beqz";
                    } else { // 不等于跳转
                        cmpMips = "bnez";
                    }
                    outMips(cmpMips + " $t8," + label);
                } else { // 两个比较数都是非零常数

                }
            }
        } else { // 有一个比较数是变量
            if (right == "0") {
                left = to_string(tmpRegisterPool(left, true)); // 左侧是变量获取其寄存器
                if (statement.getIRType() == geqJump) { // 大于等于跳转
                    cmpMips = "bgez";
                } else if (statement.getIRType() == greJump) { // 大于跳转
                    cmpMips = "bgtz";
                } else if (statement.getIRType() == leqJump) { // 小于等于跳转
                    cmpMips = "blez";
                } else if (statement.getIRType() == lesJump) { // 小于跳转
                    cmpMips = "bltz";
                } else if (statement.getIRType() == eqlJump) { // 等于跳转
                    cmpMips = "beqz";
                } else { // 不等于跳转
                    cmpMips = "bnez";
                }
                outMips(cmpMips + " $" + left + "," + label);
            } else if (left == "0"){
                right = to_string(tmpRegisterPool(right, true)); // 右侧侧是变量获取其寄存器
                if (statement.getIRType() == geqJump) { // 大于等于跳转
                    cmpMips = "blez";
                } else if (statement.getIRType() == greJump) { // 大于跳转
                    cmpMips = "bltz";
                } else if (statement.getIRType() == leqJump) { // 小于等于跳转
                    cmpMips = "bgez";
                } else if (statement.getIRType() == lesJump) { // 小于跳转
                    cmpMips = "bgtz";
                } else if (statement.getIRType() == eqlJump) { // 等于跳转
                    cmpMips = "beqz";
                } else { // 不等于跳转
                    cmpMips = "bnez";
                }
                outMips(cmpMips + " $" + right + "," + label);
            } else { // 没有常数0
                if (isConstVariable(right)) { // 右侧数是常数
                    left = to_string(tmpRegisterPool(left, true)); // 左侧是变量获取其寄存器
                    right = to_string(stringToNumber(right));
                    if (statement.getIRType() == geqJump) { // 大于等于跳转
                        cmpMips = "bge";
                    } else if (statement.getIRType() == greJump) { // 大于跳转
                        cmpMips = "bgt";
                    } else if (statement.getIRType() == leqJump) { // 小于等于跳转
                        cmpMips = "ble";
                    } else if (statement.getIRType() == lesJump) { // 小于跳转
                        cmpMips = "blt";
                    } else if (statement.getIRType() == eqlJump) { // 等于跳转
                        cmpMips = "beq";
                    } else { // 不等于跳转
                        cmpMips = "bne";
                    }
                    outMips(cmpMips + " $" + left + "," + right + "," + label);
                } else { // 左侧数是常数
                    right = to_string(tmpRegisterPool(right, true)); // 右侧侧是变量获取其寄存器
                    left = to_string(stringToNumber(left));
                    if (statement.getIRType() == geqJump) { // 大于等于跳转
                        cmpMips = "ble";
                    } else if (statement.getIRType() == greJump) { // 大于跳转
                        cmpMips = "blt";
                    } else if (statement.getIRType() == leqJump) { // 小于等于跳转
                        cmpMips = "bge";
                    } else if (statement.getIRType() == lesJump) { // 小于跳转
                        cmpMips = "bgt";
                    } else if (statement.getIRType() == eqlJump) { // 等于跳转
                        cmpMips = "beq";
                    } else { // 不等于跳转
                        cmpMips = "bne";
                    }
                    outMips(cmpMips + " $" + right + "," + left + "," + label);
                }
            }
        }
        return;
    }
    // 两个比较数都是变量
    vector<string> cmps = {left, right};
    vector<bool> needLoads = {true, true};
    vector<regNum> regs = applyTmpRegister(cmps, needLoads);
    string regl,regr;
    regl = to_string(regs[0]);
    regr = to_string(regs[1]);
    if (statement.getIRType() == geqJump) { // 大于等于跳转
        cmpMips = "bge";
    } else if (statement.getIRType() == greJump) { // 大于跳转
        cmpMips = "bgt";
    } else if (statement.getIRType() == leqJump) { // 小于等于跳转
        cmpMips = "ble";
    } else if (statement.getIRType() == lesJump) { // 小于跳转
        cmpMips = "blt";
    } else if (statement.getIRType() == eqlJump) { // 等于跳转
        cmpMips = "beq";
    } else { // 不等于跳转
        cmpMips = "bne";
    }
    outMips(cmpMips + " $" + regl + ",$" + regr + "," + label);
}

void IRTranslator::jumpTranslator(IRStatement& statement){//无条件跳转，使用j指令直接跳转
    stringstream ss(statement.getIR());
    string label;
    getline(ss,label,' ');
    getline(ss,label,' ');
    outMips("j " + label);
}

void IRTranslator::translateBlk(BasicBlk& blk) {
    for (unsigned int i = 0;i < blk.size();i++) {
        IRStatement statement = blk[i];
        switch (statement.getIRType()) {
            case gConstDef:
            case pConstDef:
                constantDefinitionTranslator(statement);
                break;
            case gVarDef:
            case pVarDef:
                variableDefinitionTranslator(statement);
                break;
            case statLabel:
                programLabelSetTranslator(statement);
                break;
            case funcDefLabel:
                funcDefLabelTranslator(statement);
                break;
            case funcDefPara:
                funcDefParaTranslator(statement);
                break;
            case valueRet:
            case voidRet:
                retStmtTranslator(statement);
                break;
            case funcCallBegin:
                funcCallBeginTranslator(statement);
                break;
            case funcRetAddrSw:
                funcRetAddrSwTranslator();
                break;
            case funcCallPara:
                funcCallParaTranslator(statement);
                break;
            case nonRetFuncCall:
            case retFuncCall:
                funcCallTranslator(statement, i);
                break;
            case readInt:
            case readChar:
                readFunctionTranslator(statement);
                break;
            case printNewLine:
            case printStr:
            case printChar:
            case printInt:
                printFunctionTranslator(statement);
                break;
            case operation:
                operationTranslator(statement);
                break;
            case assign:
                assignTranslator(statement);
                break;
            case greJump:
            case geqJump:
            case lesJump:
            case leqJump:
            case eqlJump:
            case neqJmp:
                conditionJumpTranslator(statement); // 条件跳转
                break;
            case jump:
                jumpTranslator(statement);
                break;
            default:
                break;
        }
    }
}
