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

static bool isConstantString(const string& str) {//查看字符串是否代表常量
    return isCharConstantString(str) | isNumberConstantString(str);
}

static long long stringToNumber(string& str) {
    if (str.empty()) return 0;
    if (str.size() == 3 && str[0] == '\'' && str[2] == str[0]) {
        return (long long)str[1];//字符常量
    }
    return stoi(str);
}


/*
 * 关于函数调用
 * 函数调用之前要保护现场在调用之后恢复现场
 * */
/*
 * $0           0
 * $2           函数调用返回值
 * $3           中间寄存器
 * $4           系统调用传参数($a0)
 * $8-$15,24,25 临时寄存器
 * $16-$23      全局寄存器
 * $28          全局指针($gp)
 * $29          堆栈指针($sp)
 * $30          帧指针($fp)配合栈指针使用
 * $31          返回地址($ra)
 * */
void IRTranslator::outMips(const string& str) {
    this->mipsFile << str << endl;
}

void IRTranslator::gConstDefTranslator(IRStatement& statement) {//变量声明的解析
    //全局常量:全局常量放到内存中
    stringstream ss(statement.getIR());
    string symbol;
    string value;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');//获取常量的标识符
    getline(ss,value,' ');
    getline(ss,value,' ');//获取常量的数值
    //无论字符还是整数类型都分配一个字的大小
    outMips(symbol + ": .word " + value);
    this->table->setGlobalConstAddr(symbol,this->memAlloc);//符号表中设置变量的地址
    this->memAlloc += 4;
}

void IRTranslator::gVarDefTranslator(IRStatement& statement) {
    //全局变量放到内存中
    stringstream ss(statement.getIR());
    string symbol;
    string scale;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,scale,' ');
    unsigned int size = stringToNumber(scale);
    outMips(symbol + " : .space " + to_string(size * 4));
    if (size == 1) {
        this->table->setGlobalVarAddr(symbol,this->memAlloc);
    } else {
        this->table->setGlobalArrayInfo(symbol,size,this->memAlloc);
    }
    this->memAlloc += 4 * size;
}

void IRTranslator::pConstDefTranslator(IRStatement& statement) {//对于函数参数对于栈空间的占用，调用函数进入新的复合语句块之后先把函数参数取出之后再声明其他常量或者变量
    stringstream ss(statement.getIR());
    string symbol;
    string value;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,value,' ');
    getline(ss,value,' ');
    this->table->setPartialConstAddr(symbol,this->curFunctionName,this->curStackAlloc);
    outMips("li $v0," + value);
    outMips("sw $v0,-" + to_string(this->curStackAlloc) + "($sp)");
    this->curStackAlloc += 4;
}

void IRTranslator::pVarDefTranslator(IRStatement& statement){
    stringstream ss(statement.getIR());
    string symbol;
    string scale;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss, scale, ' ');
    unsigned int size = stringToNumber(scale);
    if (size == 1) {
        this->table->setPartialVarAddr(symbol,this->curFunctionName,this->curStackAlloc);
    } else {
        this->table->setPartialArrayInfo(symbol, this->curFunctionName, size, this->curStackAlloc);
    }
    this->curStackAlloc += 4 * size;
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
//保护现场和回复现场都是在寄存器分配情况在编译时完全没有改变的状态之下进行的
unsigned int IRTranslator::sceneStore() {//函数调用之前现场存储
    string mips;
    unsigned int addr = this->curStackAlloc;
    for (auto & it : this->varRegisterMap) {
        regNum reg = it.second;//没有在内存中存储
        outMips("sw $" + to_string(reg) + ",-" + to_string(addr) + "($sp)");
        addr += 4;
    }
    return addr;
}

void IRTranslator::sceneLoad() {
    string mips;
    unsigned int addr = this->curStackAlloc;
    for (auto & it : this->varRegisterMap) {
        regNum reg = it.second;//没有在内存中存储
        outMips("lw $" + to_string(reg) + ",-" + to_string(addr) + "($sp)");
        addr += 4;
    }
}

void IRTranslator::nonRetFuncCallTranslator(IRStatement& statement){
    string functionName;
    stringstream ss(statement.getIR());
    getline(ss,functionName,' ');
    getline(ss,functionName,' ');
    outMips("jal " + functionName);
    outMips("subi $gp,$gp,4");
    outMips("lw $sp,0($gp)");
    sceneLoad();//回复现场
}

void IRTranslator::retFuncCallTranslator(IRStatement& statement){
    unsigned int reg;
    string retVar;
    string functionName;
    stringstream ss(statement.getIR());
    getline(ss,functionName,' ');
    getline(ss,functionName,' ');
    getline(ss,retVar,' ');
    outMips("jal " + functionName);
    outMips("subi $gp,$gp,4");
    outMips("lw $sp,0($gp)");
    sceneLoad();//回复现场
    reg = tmpRegisterPool(retVar, false); // 一个变量直接从寄存器中申请寄存器
    outMips("move $" + to_string(reg) + ",$v0");
}

void IRTranslator::funcDefParaTranslator(IRStatement& statement){//函数定义中的参数
    stringstream ss(statement.getIR());
    string symbol;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    this->table->setPartialVarAddr(symbol,this->curFunctionName,this->curStackAlloc);
    this->curStackAlloc +=  4;//每个参数都占四个字节
}

void IRTranslator::funcCallBeginTranslator(IRStatement& statement){
    unsigned int addr = sceneStore();
    outMips("sw $sp,0($gp)");
    outMips("addi $gp,$gp,4");
    outMips("subi $sp,$sp," + to_string(addr));
    this->pushParaStack = 4; //为返回值留出保存地址
}

void IRTranslator::funcCallParaTranslator(IRStatement& statement){
    string paraVar;
    stringstream ss(statement.getIR());
    getline(ss,paraVar,' ');
    getline(ss,paraVar,' ');
    string reg = to_string(tmpRegisterPool(paraVar, true));
    outMips("sw $" + reg + ",-" + to_string(this->pushParaStack) + "($sp)");
    this->pushParaStack += 4;
}

void IRTranslator::voidRetTranslator(IRStatement& statement){
    outMips("lw $ra,0($sp)");
    outMips("jr $ra");
}

// 这里的返回值一定是变量，没有考虑常量的情况
void IRTranslator::valueRetTranslator(IRStatement& statement){
    string retVar;
    stringstream ss(statement.getIR());
    getline(ss,retVar,' ');
    getline(ss,retVar,' ');
    if (isConstantString(retVar)) {
        retVar = to_string(stringToNumber(retVar));
        outMips("li $v0," + retVar);
    } else {
        unsigned int reg = tmpRegisterPool(retVar, true);
        outMips("move $v0,$" + to_string(reg));
    }
    outMips("lw $ra,0($sp)");
    outMips("jr $ra");
}

void IRTranslator::statLabelTranslator(IRStatement& statement){//所有标签直接输出即可
    outMips(statement.getIR());
}

void IRTranslator::jumpTranslator(IRStatement& statement){//无条件跳转，使用j指令直接跳转
    stringstream ss(statement.getIR());
    string label;
    getline(ss,label,' ');
    getline(ss,label,' ');
    outMips("j " + label);
}

void IRTranslator::readIntTranslator(IRStatement& statement){
    outMips("li $v0,5");
    outMips("syscall");
    string variable;
    stringstream ss(statement.getIR());
    getline(ss, variable, ' ');
    getline(ss, variable, ' ');
    regNum reg = tmpRegisterPool(variable, false);
    outMips("move $" + to_string(reg) + ",$v0");
    flushVariable(variable, reg); // 读到的变量更新到内存中
}

void IRTranslator::readCharTranslator(IRStatement& statement){
    outMips("li $v0,12");
    outMips("syscall");
    string variable;
    stringstream ss(statement.getIR());
    getline(ss, variable, ' ');
    getline(ss, variable, ' ');
    regNum reg = tmpRegisterPool(variable, false);
    outMips("move $" + to_string(reg) + ",$v0");
    flushVariable(variable, reg);
}

void IRTranslator::printStringTranslator(IRStatement& statement){
    stringstream ss(statement.getIR());
    string label;
    getline(ss,label,' ');
    getline(ss,label, ' ');
    outMips("la $a0," + label);
    outMips("li $v0,4");
    outMips("syscall");
}

void IRTranslator::printIntTranslator(IRStatement& statement){
    string variable;
    stringstream ss(statement.getIR());
    getline(ss, variable, ' ');
    getline(ss, variable, ' ');
    if (isConstantString(variable)) {
        variable = to_string(stringToNumber(variable));
        outMips("li $a0," + variable);
    } else {
        regNum reg = tmpRegisterPool(variable, true);
        outMips("move $a0,$" + to_string(reg));
    }
    outMips("li $v0,1");
    outMips("syscall");
}


//------//
void IRTranslator::printCharTranslator(IRStatement &statement) {
    string variable;
    stringstream ss(statement.getIR());
    getline(ss, variable, ' ');
    getline(ss, variable, ' ');
    if (isConstantString(variable)) {
        variable = to_string(stringToNumber(variable));
        outMips("li $a0," + variable);
    } else {
        regNum reg = tmpRegisterPool(variable, true);
        outMips("move $a0,$" + to_string(reg));
    }
    outMips("li $v0,11");
    outMips("syscall");
}

void IRTranslator::printNewLineTranslator(IRStatement &statement) {
    outMips(R"(li $a0 '\n')");
    outMips("li $v0,11");
    outMips("syscall");
}

//表达式计算中没有改变变量的数值
void IRTranslator::operationTranslator(IRStatement& statement) {
    string op,opnum1,opnum2,ansSym;
    stringstream ss(statement.getIR());
    getline(ss,op,',');
    getline(ss,opnum1,',');
    getline(ss,opnum2,',');
    getline(ss,ansSym,',');
    string mips;
    if (op == "[]") { // 取数组元素
        if (this->table->isPartialVariable(opnum2, this->curFunctionName)) {
            unsigned int addr = this->table->getPartialVarAddr(opnum2,this->curFunctionName);
            if (isConstantString(opnum1)) {//index不可能是单独的字符常量，此时下标是一个常量
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
                outMips("sll $" + regIndex + ",$" + regIndex + ",2");
                outMips("addi $" + regIndex + ",$" + regIndex + "," +  to_string(addr));
                outMips("sub $" + regIndex + ",$sp,$" + regIndex);
                outMips("lw $" + ansReg + ",0($" + regIndex + ")");
                flushVariable(ansSym, stringToNumber(ansReg));
            }
        } else if (this->table->globalExist(opnum2)) {
            if (isConstantString(opnum1)) {
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
                outMips("sll $" + regIndex + ",$" + regIndex + ",2");
                outMips("lw $" + ansReg + "," + opnum2 + "(" + "$" + regIndex + ")");
                flushVariable(ansSym, stringToNumber(ansReg));
            }
        } else outMips("error");
        return;
    }
    if (opnum1.empty()) opnum1 = "0";
    if (isConstantString(opnum1)) {
        opnum1 = to_string(stringToNumber(opnum1));
        outMips("li $v0," + opnum1);
        if (isConstantString(opnum2)) {
            opnum2 = to_string(stringToNumber(opnum2));
            outMips("li $v1," + opnum2);
            regNum ansReg = tmpRegisterPool(ansSym, false);
            switch (op[0]) {
                case('+') : outMips("addu $" + to_string(ansReg) + ",$v0,$v1");
                    break;
                case('-') : outMips("sub $" + to_string(ansReg) + ",$v0,$v1");
                    break;
                case('*') : outMips("mul $" + to_string(ansReg) + ",$v0,$v1");
                    break;
                case('/') : outMips("div $" + to_string(ansReg) + ",$v0,$v1");
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
                case('+') : outMips("addu $" + to_string(regs[1]) + ",$v0,$" + to_string(regs[0]));
                    break;
                case('-') : outMips("sub $" + to_string(regs[1]) + ",$v0,$" + to_string(regs[0]));
                    break;
                case('*') : outMips("mul $" + to_string(regs[1]) + ",$v0,$" + to_string(regs[0]));
                    break;
                case('/') : outMips("div $" + to_string(regs[1]) + ",$v0,$" + to_string(regs[0]));
                    break;
                default:    outMips("error");
                    break;
            }
            flushVariable(ansSym, regs[1]);
        }
    } else {
        if (isConstantString(opnum2)) {
            opnum2 = to_string(stringToNumber(opnum2));
            outMips("li $v1," + opnum2);
            vector<string> variables = {opnum1, ansSym};
            vector<bool> needLoads = {true, false};
            vector<regNum> regs = applyTmpRegister(variables, needLoads);
            switch (op[0]) {
                case('+') : outMips("addu $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$v1");
                    break;
                case('-') : outMips("sub $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$v1");
                    break;
                case('*') : outMips("mul $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$v1");
                    break;
                case('/') : outMips("div $" + to_string(regs[1]) + ",$" + to_string(regs[0]) + ",$v1");
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
            flushVariable(ansSym, regs[1]);
        }
    }
}

void IRTranslator::assignTranslator(IRStatement& statement){//同时赋值和改变寄存器
    stringstream ss(statement.getIR());
    string left,right;
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,right,' ');
    if (isConstantString(right)) { // 右侧是常量
        right = to_string(stringToNumber(right));
        outMips("li $v1," + right); // v1作为中介寄存器，存放右侧数据的数值
        if (left.find('[') != std::string::npos) { // 左侧是数组
            string array, index;
            stringstream s(left);
            getline(s, array, '[');
            getline(s, index, ']');
            #ifdef debug
            cout << "assign statement : left :array :" << array << " index : " << index << endl;
            #endif
            if (isConstantString(index)) { //索引是常量
                unsigned int indexNumber = stringToNumber(index);
                if (this->table->isPartialVariable(array, this->curFunctionName)) {
                    unsigned int addr = this->table->getPartialVarAddr(array, this->curFunctionName);
                    addr += indexNumber * 4;
                    outMips("sw $v1,-" + to_string(addr) + "($sp)");
                } else if (this->table->globalExist(array)) {
                    indexNumber *= 4;
                    outMips("sw $v1," + array + "+" + to_string(indexNumber));
                } else outMips("error");
            } else { //索引是变量
                regNum indexValueReg = tmpRegisterPool(index, true);
                outMips("sll $" + to_string(indexValueReg) + ",$" + to_string(indexValueReg) + ",2"); //index左移两位
                if (this->table->isPartialVariable(array, this->curFunctionName)) {
                    unsigned int addr = this->table->getPartialVarAddr(array, this->curFunctionName);
                    outMips("addi $" + to_string(indexValueReg) + ",$" + to_string(indexValueReg) + "," + to_string(addr)); //index寄存器中地址先加上数组的基地址
                    outMips("sub $" + to_string(indexValueReg) + ",$sp," + "$" + to_string(indexValueReg)); //由于是在栈中，为了下面使用寄存器直接寻址做准备
                    outMips("sw $v1,($" + to_string(indexValueReg) +")");
                } else if (this->table->globalExist(array)) {
                    outMips("sw $v1," + array + ",($" + to_string(indexValueReg) + "$)");
                } else outMips("error");
            }
        } else {
            regNum leftReg = tmpRegisterPool(left, false); //为左侧变量申请一个寄存器
            outMips("move $" + to_string(leftReg) + ",$v1");
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
            if (isConstantString(index)) { //索引是常量
                regNum rightValueReg = tmpRegisterPool(right, true);
                unsigned int indexNumber = stringToNumber(index);
                if (this->table->isPartialVariable(array, this->curFunctionName)) {
                    unsigned int addr = this->table->getPartialVarAddr(array, this->curFunctionName);
                    addr += indexNumber * 4;
                    outMips("sw $" + to_string(rightValueReg) + ",-" + to_string(addr) + "($sp)");
                } else if (this->table->globalExist(array)) {
                    indexNumber *= 4;
                    outMips("sw $" + to_string(rightValueReg) + "," + array + "+" + to_string(indexNumber));
                } else outMips("error");
            } else {
                vector<string> variables = {index, right};
                vector<bool> needLoads = {true, true};
                vector<regNum> regs = applyTmpRegister(variables, needLoads);
                regNum indexValueReg = regs[0];
                regNum rightValueReg = regs[1];
                outMips("sll $" + to_string(indexValueReg) + ",$" + to_string(indexValueReg) + ",2"); //index左移两位
                if (this->table->isPartialVariable(array, this->curFunctionName)) {
                    unsigned int addr = this->table->getPartialVarAddr(array, this->curFunctionName);
                    outMips("addi $" + to_string(indexValueReg) + ",$" + to_string(indexValueReg) + "," + to_string(addr)); //index寄存器中地址先加上数组的基地址
                    outMips("sub $" + to_string(indexValueReg) + ",$sp," + "$" + to_string(indexValueReg)); //由于是在栈中，为了下面使用寄存器直接寻址做准备
                    outMips("sw $" + to_string(rightValueReg) + ",($" + to_string(indexValueReg) +")");
                } else if (this->table->globalExist(array)) {
                    outMips("sw $" + to_string(rightValueReg) + "," + array + ",($" + to_string(indexValueReg) + ")");
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

void IRTranslator::geqJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    vector<string> cmps = {left, right};
    vector<bool> needLoads = {true, true};
    vector<regNum> regs = applyTmpRegister(cmps, needLoads);
    string regl,regr;
    regl = to_string(regs[0]);
    regr = to_string(regs[1]);
    outMips("bge $" + regl + ",$" + regr + "," + label);
}

void IRTranslator::greJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    vector<string> cmps = {left, right};
    vector<bool> needLoads = {true, true};
    vector<regNum> regs = applyTmpRegister(cmps, needLoads);
    string regl,regr;
    regl = to_string(regs[0]);
    regr = to_string(regs[1]);
    outMips("bgt $" + regl + ",$" + regr + "," + label);
}

void IRTranslator::lesJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    vector<string> cmps = {left, right};
    vector<bool> needLoads = {true, true};
    vector<regNum> regs = applyTmpRegister(cmps, needLoads);
    string regl,regr;
    regl = to_string(regs[0]);
    regr = to_string(regs[1]);
    outMips("blt $" + regl + ",$" + regr + "," + label);
}

void IRTranslator::leqJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    vector<string> cmps = {left, right};
    vector<bool> needLoads = {true, true};
    vector<regNum> regs = applyTmpRegister(cmps, needLoads);
    string regl,regr;
    regl = to_string(regs[0]);
    regr = to_string(regs[1]);
    outMips("ble $" + regl + ",$" + regr + "," + label);
}

void IRTranslator::eqlJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    vector<string> cmps = {left, right};
    vector<bool> needLoads = {true, true};
    vector<regNum> regs = applyTmpRegister(cmps, needLoads);
    string regl,regr;
    regl = to_string(regs[0]);
    regr = to_string(regs[1]);
    outMips("beq $" + regl + ",$" + regr + "," + label);
}

void IRTranslator::neqJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    vector<string> cmps = {left, right};
    vector<bool> needLoads = {true, true};
    vector<regNum> regs = applyTmpRegister(cmps, needLoads);
    string regl,regr;
    regl = to_string(regs[0]);
    regr = to_string(regs[1]);
    outMips("bne $" + regl + ",$" + regr + "," + label);
}

void IRTranslator::funcRetAddrSwTranslator() {
    outMips("sw $ra,0($sp)"); //存储返回地址
    this->curStackAlloc += 4;//地址占用栈中一个字空间
}

bool IRTranslator::flushVariable(string& varName,regNum valueReg) {
    if (this->table->isPartialVariable(varName, this->curFunctionName)) { //该变量在栈中分配了内存
        storeToStack(varName,valueReg);
        return true;
    }
    if (this->table->globalExist(varName)) {
        storeToMemory(varName, valueReg);
        return true;
    }
    return false;
}

void IRTranslator::storeToStack(string& varName, regNum valueReg) {
    unsigned int addr = this->table->getPartialVarAddr(varName,this->curFunctionName);
    outMips("sw $" + to_string(valueReg) + ",-" + to_string(addr) + "($sp)");
}

void IRTranslator::storeToMemory(string& varName, regNum valueReg) {
    outMips("sw $" + to_string(valueReg) + "," + varName + "($0)");
}

void IRTranslator::storeToMemory(regNum reg, unsigned int addr) {
    outMips("sw $" + to_string(reg) + "," + to_string(addr) + "($0)");
}

bool IRTranslator::loadVariable(string &varName, regNum valueReg) {
    if (this->table->isPartialVariable(varName, this->curFunctionName)) { //该变量在栈中分配了内存
        loadFromStack(varName,valueReg);
        return true;
    }
    if (this->table->globalExist(varName)) {
        loadFromMemory(varName, valueReg);
        return true;
    }
    return false;
}

void IRTranslator::loadFromStack(string &varName, regNum valueReg) {
    unsigned int addr = this->table->getPartialVarAddr(varName,this->curFunctionName);
    outMips("lw $" + to_string(valueReg) + ",-" + to_string(addr) + "($sp)");
}

void IRTranslator::loadFromMemory(string &varName, regNum valueReg) { //这是已经全局声明内存的变量提取
    outMips("lw $" + to_string(valueReg) + "," + varName + "($0)");
}

void IRTranslator::loadFromMemory(regNum reg, unsigned int addr) {
    outMips("lw $" + to_string(reg) + "," + to_string(addr) + "($0)");
}



void IRTranslator::flushRegMap() {
    this->varRegisterMap.clear();
    this->varMemoryMap.clear();
    this->regAllocMap.clear();
    this->regAlloc = 8;
    this->memAlloc -= this->curBlk->allocedMem();//每个基本块中的临时变量在离开该基本块后不占用内存
}

void IRTranslator::translateBlk(BasicBlk& blk) {
    for (unsigned int i = 0;i < blk.size();i++) {
        IRStatement statement = blk[i];
        switch (statement.getIRType()) {
            case gConstDef:
                gConstDefTranslator(statement);
                break;
            case gVarDef:
                gVarDefTranslator(statement);
                break;
            case pConstDef:
                pConstDefTranslator(statement);
                break;
            case pVarDef:
                pVarDefTranslator(statement);
                break;
            case funcDefLabel:
                funcDefLabelTranslator(statement);
                break;
            case nonRetFuncCall:
                nonRetFuncCallTranslator(statement);
                break;
            case retFuncCall:
                retFuncCallTranslator(statement);
                break;
            case funcDefPara:
                funcDefParaTranslator(statement);
                break;
            case funcCallPara:
                funcCallParaTranslator(statement);
                break;
            case valueRet:
                valueRetTranslator(statement);
                break;
            case voidRet:
                voidRetTranslator(statement);
                break;
            case statLabel:
                statLabelTranslator(statement);
                break;
            case readInt:
                readIntTranslator(statement);
                break;
            case readChar:
                readCharTranslator(statement);
                break;
            case printStr:
                printStringTranslator(statement);
                break;
            case printChar:
                printCharTranslator(statement);
                break;
            case printInt:
                printIntTranslator(statement);
                break;
            case printNewLine:
                printNewLineTranslator(statement);
                break;
            case operation:
                operationTranslator(statement);
                break;
            case assign:
                assignTranslator(statement);
                break;
            case jump:
                jumpTranslator(statement);
                break;
            case greJump:
                greJumpTranslator(statement);
                break;
            case geqJump:
                geqJumpTranslator(statement);
                break;
            case lesJump:
                lesJumpTranslator(statement);
                break;
            case leqJump:
                leqJumpTranslator(statement);
                break;
            case eqlJump:
                eqlJumpTranslator(statement);
                break;
            case neqJmp:
                neqJumpTranslator(statement);
                break;
            case funcCallBegin:
                funcCallBeginTranslator(statement);
                break;
            case funcRetAddrSw:
                funcRetAddrSwTranslator();
                break;
            case empty:
                break;
        }
    }
}

void IRTranslator::translateIR() {
    #ifdef debug // 查看基本块的内容
    for (auto blk : this->blks) {
        for (unsigned int i = 0;i < blk.size();i++) {
            cout << blk[i].getIR() << endl;
        }
        cout << endl;
    }
    #endif
    //先定义所有字符串
    string str(".data");
    outMips(str);
    string printStr;
    unsigned int size;
    for (const auto& it : this->table->getStrNameMap()) {
        printStr.clear();
        size = 0;
        for (auto c : it.second) {
            size++;
            printStr += c;
            if (c == '\\') {
                printStr += '\\';
            }
        }
        size += 1;
        str = it.first + ": .asciiz \"" + printStr + "\"";
        outMips(str);
        this->memAlloc += size;//记录内存中字符串存储消耗的内存
    }
    //字对齐
    unsigned int oriMem = this->memAlloc;
    this->memAlloc = (this->memAlloc + 3) / 4 * 4;
    if (this->memAlloc - oriMem) {
        string mips("align : .space " + to_string(this->memAlloc - oriMem));
        outMips(mips);
    }
    for (auto blk : this->blks) {
        this->curBlk = &blk;
        translateBlk(blk);
        flushRegMap();
    }
}

void MipsGenerator::generateMips(bool needOptimal) {
    this->classifier->nonOptimalIROutput(this->root);
    this->classifier->divideBasicBlk();
    if (needOptimal) {
        this->optimal->firstRoundCopyDiffuse();//先做基础的赋值传播处理，除去大部分不必要的中间变量声明
        this->optimal->commenSubExpr(); // 消除公共子表达式
        this->optimal->secondRoundCopyDiffuse(); // 消除公共子子表达式之后会出现很多不必要的赋值语句
        this->optimal->constantDiffuse(); // 常量传播
        this->optimal->jumpLabelOptimal(); // 跳转条件和标签的简化
    }
    this->optimal->optimizedIROutput();
    this->translator->translateIR();
}
