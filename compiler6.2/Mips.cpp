#include "Mips.h"
#include <sstream>
static regNum regbegin = 5;
static string tmpl = "@tmpl";
static string tmpr = "@tmpr";


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

void BasicClassifier::outputIR(IRSyntaxNode* irSyntaxNode) {
    for (auto node : irSyntaxNode->getKids()) {
        outputIR(node);
    }
    vector<IRStatement> stmts = irSyntaxNode->getInterCodes();
    for (auto& stmt : stmts) {
        this->statements.push_back(stmt);
        this->irFile << stmt.getIR() << endl;
    }
}

void BasicClassifier::preProcessIR() {//读取中间代码指令并且划分基本快
    unsigned int blkNum = 0;
    this->blks.emplace_back();
    for (auto irStat : statements) {
        this->blks.back().push_statement(irStat);
        irStat.setBlk(blkNum);
        if (irStat.isBPStatement()) {
            this->blks.emplace_back();
            blkNum++;
        }
    }
}

/*
 * 关于函数调用
 * 函数调用之前要保护现场在调用之后恢复现场
 * */
/*
 * $0           0
 * $2           函数调用返回值
 * $4           系统调用传参数($a0)
 * $8-$15,25,26 临时寄存器
 * $28          全局指针($gp)
 * $29          堆栈指针($sp)
 * $30          帧指针($fp)配合栈指针使用
 * $31          返回地址($ra)
 * */
void IRTranslator::outMips(string& str) {
    this->mipsFile << str << endl;
}

void IRTranslator::gConstDefTranslator(IRStatement& statement) {//变量声明的解析
    //全局常量:全局常量放到内存中
    stringstream ss(statement.getIR());
    string symbol;
    string value;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');//获取常量的标识符
    getline(ss,value,' ');
    getline(ss,value,' ');//获取常量的数值
    //无论字符还是整数类型都分配一个字的大小
    string decl(symbol + ": .word " + value);
    outMips(decl);
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
    getline(ss,scale,' ');
    unsigned int size = stringToNumber(scale);
    string decl(symbol + " : .space " + to_string(size * 4));
    outMips(decl);
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
    getline(ss,value,' ');
    getline(ss,value,' ');
    this->table->setPartialConstAddr(symbol,this->curFunctionName,this->curStackAlloc);
    string mips("li $v0," + value);
    outMips(mips);
    mips = "sw $v0,-" + to_string(this->curStackAlloc) + "($sp)";
    outMips(mips);
    this->curStackAlloc += 4;
}

void IRTranslator::pVarDefTranslator(IRStatement& statement){
    stringstream ss(statement.getIR());
    string symbol;
    string scale;
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
        string text(".text");
        switchText = true;
        outMips(text);
        text = "jal main";
        outMips(text);
        text = "j __end";
        outMips(text);
    }
    stringstream ss(statement.getIR());
    string label;
    getline(ss,label,' ');
    getline(ss,label);
    outMips(label);
    //函数切换
    label.pop_back();
    this->curFunctionName = label;
    this->curStackAlloc = 0;
}
//保护现场和回复现场都是在寄存器分配情况在编译时完全没有改变的状态之下进行的
unsigned int IRTranslator::sceneStore() {//函数调用之前现场存储
    string mips;
    unsigned int addr = this->curStackAlloc;
    for (auto & it : this->varRegisterMap) {
        regNum reg = it.second;//没有在内存中存储
        mips = "sw $" + to_string(reg) + ",-" + to_string(addr) + "($sp)";
        outMips(mips);
        addr += 4;
    }
    return addr;
}

void IRTranslator::sceneLoad() {
    string mips;
    unsigned int addr = this->curStackAlloc;
    for (auto & it : this->varRegisterMap) {
        regNum reg = it.second;//没有在内存中存储
        mips = "lw $" + to_string(reg) + ",-" + to_string(addr) + "($sp)";
        outMips(mips);
        addr += 4;
    }
}

void IRTranslator::nonRetFuncCallTranslator(IRStatement& statement){
    string functionName;
    stringstream ss(statement.getIR());
    getline(ss,functionName,' ');
    getline(ss,functionName,' ');
    string mips("jal " + functionName);
    outMips(mips);
    mips = "subi $gp,$gp,4";//
    outMips(mips);
    mips = "lw $sp,0($gp)";//回复栈指针
    outMips(mips);
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
    string mips("jal " + functionName);
    outMips(mips);
    mips = "subi $gp,$gp,4";//
    outMips(mips);
    mips = "lw $sp,0($gp)";//回复栈指针
    outMips(mips);
    sceneLoad();//回复现场
    reg = applyTmpRegister(retVar);
    mips = "move $" + to_string(reg) + ",$v0";//v0中存放返回值
    outMips(mips);
}

void IRTranslator::funcDefParaTranslator(IRStatement& statement){//函数定义中的参数
    stringstream ss(statement.getIR());
    string symbol;
    getline(ss,symbol,' ');
    getline(ss,symbol,' ');
    this->table->setPartialVarAddr(symbol,this->curFunctionName,this->curStackAlloc);
    this->curStackAlloc +=  4;//每个参数都占四个字节
}

void IRTranslator::funcCallBeginTranslator(IRStatement& statement){
    unsigned int addr = sceneStore();
    string mips("sw $sp,0($gp)");
    outMips(mips);
    mips = "addi $gp,$gp,4";
    outMips(mips);
    mips = "subi $sp,$sp," + to_string(addr);
    outMips(mips);
    this->pushParaStack = 4;//保存返回值地址
}

void IRTranslator::funcCallParaTranslator(IRStatement& statement){
    string paraVar;
    stringstream ss(statement.getIR());
    getline(ss,paraVar,' ');
    getline(ss,paraVar,' ');
    string reg = to_string(applyTmpRegister(paraVar));
    string mips("sw $" + reg + ",-" + to_string(this->pushParaStack) + "($sp)");
    outMips(mips);
    this->pushParaStack += 4;
}

void IRTranslator::voidRetTranslator(IRStatement& statement){
    string mips("lw $ra,0($sp)");
    outMips(mips);
    mips = "jr $ra";
    outMips(mips);
}

void IRTranslator::valueRetTranslator(IRStatement& statement){
    string retVar;
    stringstream ss(statement.getIR());
    getline(ss,retVar,' ');
    getline(ss,retVar,' ');
    unsigned int reg = applyTmpRegister(retVar);
    string mips("move $v0,$" + to_string(reg));//返回值v0
    outMips(mips);
    mips = "lw $ra,0($sp)";
    outMips(mips);
    mips = "jr $ra";
    outMips(mips);
}

void IRTranslator::statLabelTranslator(IRStatement& statement){//所有标签直接输出即可
    string label = statement.getIR();
    outMips(label);
}

void IRTranslator::jumpTranslator(IRStatement& statement){//无条件跳转，使用j指令直接跳转
    stringstream ss(statement.getIR());
    string label;
    getline(ss,label,' ');
    getline(ss,label,' ');
    string mips = "j ";
    mips += label;
    outMips(mips);
}

void IRTranslator::readIntTranslator(IRStatement& statement){
    string mips("li $v0,5");
    outMips(mips);
    mips = "syscall";
    outMips(mips);
    string regVar;
    stringstream ss(statement.getIR());
    getline(ss,regVar,' ');
    getline(ss,regVar,' ');
    string reg = to_string(applyTmpRegister(regVar));
    mips = "move $" + reg + ",$v0";
    outMips(mips);
    if (this->table->isPartialVariable(regVar, this->curFunctionName)) {
        unsigned int addr = this->table->getPartialVarAddr(regVar,this->curFunctionName);
        mips = "sw $" + reg + ",-" + to_string(addr) + "($sp)";
    } else {
        mips = "sw $" + reg + "," + regVar + "($0)";
    }
    outMips(mips);
}

void IRTranslator::readCharTranslator(IRStatement& statement){
    string mips("li $v0,12");
    outMips(mips);
    mips = "syscall";
    outMips(mips);
    string regVar;
    stringstream ss(statement.getIR());
    getline(ss,regVar,' ');
    getline(ss,regVar,' ');
    string reg = to_string(applyTmpRegister(regVar));
    mips = "move $" + reg + ",$v0";
    outMips(mips);
    if (this->table->isPartialVariable(regVar, this->curFunctionName)) {
        unsigned int addr = this->table->getPartialVarAddr(regVar,this->curFunctionName);
        mips = "sw $" + reg + ",-" + to_string(addr) + "($sp)";
    } else {
        mips = "sw $" + reg + "," + regVar + "($0)";
    }
    outMips(mips);
}

void IRTranslator::printStringTranslator(IRStatement& statement){
    stringstream ss(statement.getIR());
    string label;
    getline(ss,label,' ');
    getline(ss,label);
    string lStrAddr("la $a0," + label);
    string syscallPara("li $v0,4");
    string syscall("syscall");
    outMips(lStrAddr);
    outMips(syscallPara);
    outMips(syscall);
}

void IRTranslator::printIntTranslator(IRStatement& statement){
    string regVar;
    stringstream ss(statement.getIR());
    getline(ss,regVar,' ');
    getline(ss,regVar,' ');
    string reg = to_string(applyTmpRegister(regVar));
    string mips("move $a0,$" + reg);
    outMips(mips);
    mips = "li $v0,1";
    outMips(mips);
    mips = "syscall";
    outMips(mips);
}

void IRTranslator::printCharTranslator(IRStatement &statement) {
    string regVar;
    stringstream ss(statement.getIR());
    getline(ss,regVar,' ');
    getline(ss,regVar,' ');
    string reg = to_string(applyTmpRegister(regVar));
    string mips("move $a0,$" + reg);
    outMips(mips);
    mips = "li $v0,11";
    outMips(mips);
    mips = "syscall";
    outMips(mips);
}

void IRTranslator::printNewLineTranslator(IRStatement &statement) {
    string mips(R"(li $a0 '\n')");
    outMips(mips);
    mips = "li $v0,11";
    outMips(mips);
    mips = "syscall";
    outMips(mips);
}

//表达式计算中没有改变变量的数值
void IRTranslator::operationTranslator(IRStatement& statement) {
    string op,opnum1,opnum2,ansReg,ansSym;
    stringstream ss(statement.getIR());
    getline(ss,op,',');
    getline(ss,opnum1,',');
    getline(ss,opnum2,',');
    getline(ss,ansSym,',');
    ansReg = to_string(applyTmpRegister(ansSym));
    string mips;
    if (op == "[]") { // 取数组元素
        if (this->table->isPartialVariable(opnum2, this->curFunctionName)) {
            unsigned int addr = this->table->getPartialVarAddr(opnum2,this->curFunctionName);
            if (isConstantString(opnum1)) {//index不可能是单独的字符常量
                unsigned int indexnum;
                indexnum = stringToNumber(opnum1);
                addr += indexnum * 4;
                mips = "lw $" + ansReg + ",-" + to_string(addr) + "($sp)";
                outMips(mips);
            } else { //说明index是一个变量
                string regIndex = to_string(applyTmpRegister(opnum1));
                mips = "sll $" + regIndex + ",$" + regIndex + ",2";
                outMips(mips);
                mips = "addi $" + regIndex + ",$" + regIndex + "," +  to_string(addr);
                outMips(mips);
                mips = "sub $" + regIndex + ",$sp,$" + regIndex;
                outMips(mips);
                mips = "lw $" + ansReg + ",0($" + regIndex + ")";
                outMips(mips);
            }
        } else if (this->table->globalExist(opnum2)) {
            if (isConstantString(opnum1)) {
                unsigned int indexnum;
                indexnum = stringToNumber(opnum1);
                indexnum = indexnum * 4;
                mips = "lw $" + ansReg + "," + opnum2 + " + " + to_string(indexnum);
            } else {
                string regIndex = to_string(applyTmpRegister(opnum1));
                mips = "sll $" + regIndex + ",$" + regIndex + ",2";
                outMips(mips);
                mips = "lw $" + ansReg + "," + opnum2 + "(" + "$" + regIndex + ")";
            }
            outMips(mips);
        } else {
            mips = "error";
            outMips(mips);
        }
        return;
    }
    if (isConstantString(opnum1)) {
        opnum1 = to_string(stringToNumber(opnum1));
        mips = "li $" + ansReg + "," + opnum1;
    } else {
        opnum1 = to_string(applyTmpRegister(opnum1));
        mips = "move $" + ansReg + ",$" + opnum1;
    }
    outMips(mips);
    if (op == "+") mips = "add";
    else if (op == "-") mips = "sub";
    else if (op == "*") mips = "mul";
    else if (op == "/") mips = "div";
    else {
        mips = "error";
        outMips(mips);
    }
    if (isConstantString(opnum2)) {
        opnum1 = to_string(stringToNumber(opnum1));
        mips += " $" + ansReg + ",$" + ansReg + "," + opnum2;
    } else {
        opnum2 = to_string(applyTmpRegister(opnum2));
        mips += " $" + ansReg + ",$" + ansReg + ",$" + opnum2;
    }
    outMips(mips);
    flushVariable(ansSym, stringToNumber(ansReg));
}

void IRTranslator::assignTranslator(IRStatement& statement){//同时赋值和改变寄存器
    stringstream ss(statement.getIR());
    string left,right,mips;
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,right,' ');
    string regtmpl,regtmpr;
    regtmpr = to_string(applyTmpRegister(tmpr));//右边不可能是数组
    if (isConstantString(right)) {
        right = to_string(stringToNumber(right));
        regtmpr = to_string(applyTmpRegister(tmpr)); //右侧是纯数字
        mips = "li $" + regtmpr + "," + right; //把tmpr赋值
        outMips(mips);
    } else { // 右侧是变量
        regtmpr = to_string(applyTmpRegister(right));
    }
    //regtmpr存放的是右侧式子的数值
    if (left.find('[') != string::npos) {//左边是数组
        regtmpl = to_string(applyTmpRegister(tmpl));
        string array,index;
        stringstream ss1(left);
        getline(ss1,array,'[');
        getline(ss1,index,']');
        if (this->table->isPartialVariable(array, this->curFunctionName)) { //左侧数组在栈中有地址分配
            unsigned int addr = this->table->getPartialArrayAddr(array, this->curFunctionName);
            if (isConstantString(index)) { //index是常量
                unsigned int indexnum;
                indexnum = stringToNumber(index);
                addr += indexnum * 4;
                mips = "li $" + regtmpl + "," + to_string(addr);
                outMips(mips);
            } else { // index是变量
                string regIndex = to_string(applyTmpRegister(index));
                mips = "mul $" + regIndex + ",$" + regIndex + ",4";
                outMips(mips);
                mips = "addi $" + regIndex + ",$" + regIndex + "," + to_string(addr);
                outMips(mips);
                mips = "sub $" + regtmpl + ",$sp,$" + regIndex;
                outMips(mips);
            }
        } else if (this->table->globalExist(array)) {// 数组是全局变量声明了地址
            mips = "la $" + regtmpl + "," + array;
            outMips(mips);
            if (isConstantString(index)) {
                unsigned int indexnum;
                indexnum = stringToNumber(index);
                indexnum = indexnum * 4;
                mips = "addi $" + regtmpl +  ",$" + regtmpl + "," + to_string(indexnum);
                outMips(mips);
            } else {
                string regIndex = to_string(applyTmpRegister(index));
                mips = "mul $" + regIndex + ",$" + regIndex + ",4";
                outMips(mips);
                mips = "add $" + regtmpl +  ",$" + regtmpl + ",$" + regIndex;
                outMips(mips);
            }
        } else {
            mips = "error";
            outMips(mips);
        }
        mips = "sw $" + regtmpr + ",0(" + "$" + regtmpl + ")"; //数组向地址存储变量
        outMips(mips);
    } else {// 左侧是变量
        regtmpl = to_string(applyTmpRegister(left));
        mips = "move $" + regtmpl + ",$" + regtmpr;
        outMips(mips);
        flushVariable(left, stringToNumber(regtmpl)); //左侧变量如果不是临时变量那么更新地址中的数值
    }

}
void IRTranslator::geqJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    string regl,regr;
    regl = to_string(applyTmpRegister(left));
    regr = to_string(applyTmpRegister(right));
    string mips("bge $" + regl + ",$" + regr + "," + label);
    outMips(mips);
}

void IRTranslator::greJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    string regl,regr;
    regl = to_string(applyTmpRegister(left));
    regr = to_string(applyTmpRegister(right));
    string mips("bgt $" + regl + ",$" + regr + "," + label);
    outMips(mips);
}

void IRTranslator::lesJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    string regl,regr;
    regl = to_string(applyTmpRegister(left));
    regr = to_string(applyTmpRegister(right));
    string mips("blt $" + regl + ",$" + regr + "," + label);
    outMips(mips);
}

void IRTranslator::leqJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    string regl,regr;
    regl = to_string(applyTmpRegister(left));
    regr = to_string(applyTmpRegister(right));
    string mips("ble $" + regl + ",$" + regr + "," + label);
    outMips(mips);
}

void IRTranslator::eqlJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    string regl,regr;
    regl = to_string(applyTmpRegister(left));
    regr = to_string(applyTmpRegister(right));
    string mips("beq $" + regl + ",$" + regr + "," + label);
    outMips(mips);
}

void IRTranslator::neqJumpTranslator(IRStatement& statement) {
    string left,right,label;
    stringstream ss(statement.getIR());
    getline(ss,left,' ');
    getline(ss,left,' ');
    getline(ss,right,' ');
    getline(ss,label,' ');
    string regl,regr;
    regl = to_string(applyTmpRegister(left));
    regr = to_string(applyTmpRegister(right));
    string mips("bne $" + regl + ",$" + regr + "," + label);
    outMips(mips);
}

void IRTranslator::funcRetAddrSwTranslator() {
    string mips("sw $ra,0($sp)");//存储返回地址
    outMips(mips);
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
    this->table->setPartialVariableRefreshed(varName, this->curFunctionName);
    unsigned int addr = this->table->getPartialVarAddr(varName,this->curFunctionName);
    string mips("sw $" + to_string(valueReg) + ",-" + to_string(addr) + "($sp)");
    outMips(mips);
}

void IRTranslator::storeToMemory(string& varName, regNum valueReg) {
    this->table->setGlobalVariableRefreshed(varName);
    string mips("sw $" + to_string(valueReg) + "," + varName + "($0)");
    outMips(mips);
}

void IRTranslator::storeToMemory(regNum reg, unsigned int addr) {
    string mips("sw $" + to_string(reg) + "," + to_string(addr) + "($0)");
    outMips(mips);
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
    string mips("lw $" + to_string(valueReg) + ",-" + to_string(addr) + "($sp)");
    outMips(mips);
}

void IRTranslator::loadFromMemory(string &varName, regNum valueReg) { //这是已经全局声明内存的变量提取
    string mips("lw $" + to_string(valueReg) + "," + varName + "($0)");
    outMips(mips);
}

void IRTranslator::loadFromMemory(regNum reg, unsigned int addr) {
    string mips("lw $" + to_string(reg) + "," + to_string(addr) + "($0)");
    outMips(mips);
}

//保证已经分配过系统内存的变量不再临时申请内存空间，只有中间临时变量申请内存
regNum IRTranslator::applyTmpRegister(string& varName) {//这里是申请临时寄存器
    if (varName == "0") return 0;
    if (this->varRegisterMap.find(varName) != this->varRegisterMap.end()) { //该变量已经分配了寄存器
        regNum reg = this->varRegisterMap[varName];
        return reg;
    }
    if (this->regAllocMap.find(this->regAlloc) != this->regAllocMap.end()) {//如果当前寄存器被占用
        string name = this->regAllocMap[this->regAlloc];//当前占用寄存器的变量
        if (flushVariable(name, this->regAlloc)) { //如果该变量已经分配了内存并且更新成功了那么当前寄存器已经释放成功了
            this->table->setGlobalVariableRefreshed(varName);
            this->table->setPartialVariableRefreshed(varName, this->curFunctionName);
            this->varRegisterMap.erase(name);
            this->regAllocMap.erase(this->regAlloc);
        } else { //否则需要分配内存空间存放这个变量来释放寄存器
            unsigned int addr = this->memAlloc;
            this->memAlloc += 4;//分配新的内存
            this->curBlk->allocMem(4);//改基本块临时变量占用了内存
            storeToMemory(this->regAlloc, addr);
            this->varMemoryMap[name] = addr;
            this->varRegisterMap.erase(name);
            this->regAllocMap.erase(this->regAlloc);
        }
    }
    if (!loadVariable(varName,this->regAlloc)) { //如果是事先在内存中分配了地址的变量，提取数值，
        if (this->varMemoryMap.find(varName) != this->varMemoryMap.end()) { // 如果临时变量本身有内存分配
            unsigned int addr = this->varMemoryMap[varName];
            loadFromMemory(this->regAlloc,addr); // 从内存中提取出数值
        }
    }
    this->varRegisterMap[varName] = this->regAlloc;
    this->regAllocMap[this->regAlloc] = varName;
    this->varMemoryMap.erase(varName);
    unsigned int ans = this->regAlloc;
    registerRotate();
    return ans;
}

void IRTranslator::registerRotate() {
    if (this->regAlloc == 25) {
            this->regAlloc = regbegin;
    } else {
            this->regAlloc++;//寄存器轮转
    }

}

void IRTranslator::flushRegMap() {
    this->varRegisterMap.clear();
    this->varMemoryMap.clear();
    this->regAllocMap.clear();
    this->regAlloc = regbegin;
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
    if (needOptimal) {
        this->optimal->copyDiffuse(this->root);
    }
    this->classifier->outputIR(this->root);
    this->classifier->preProcessIR();
    if (needOptimal) {
        this->optimal->constantDiffuse(this->blks);
        this->optimal->optimizeIR(this->blks);
    }
    this->translator->translateIR();
}
