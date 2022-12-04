#ifndef COMPILER6_1_MIPS_H
#define COMPILER6_1_MIPS_H
#include <iostream>
#include <unordered_set>
#include <vector>
#include <fstream>
#include "Signary.h"
#include "IR.h"
using namespace std;
typedef unsigned int regNum;
class BasicBlk{//基本快
    vector<IRStatement> statements;

    unsigned int memSize = 0;//记录基本块中的临时变量占用的内存
public:
    IRStatement operator[] (const unsigned int index) {
        return this->statements[index];
    }

    void push_statement(const IRStatement& statement) {
        this->statements.push_back(statement);
    }

    unsigned int size() {
        return this->statements.size();
    }

    void allocMem(unsigned int size) {
        this->memSize += size;
    }

    unsigned int allocedMem() {
        return this->memSize;
    }
};

class BasicClassifier{
    vector<IRStatement>& statements;
    vector<BasicBlk>& blks;
public:
    explicit BasicClassifier(vector<BasicBlk>& blks,vector<IRStatement>& statements) : blks(blks),statements(statements){}

    void preProcessIR();
};

class IROptimal{//中间代码块优化
    vector<BasicBlk>& blks;
    SymbolTable* table;
public:
    explicit IROptimal(vector<BasicBlk>& blks,SymbolTable* table) : blks(blks),table(table){}

    void optimizeIR();
};

class tmpVarAddr{
    bool isInMem;
    unsigned int memAddr = 0;
    unsigned int regNum = 0;

public:
    explicit tmpVarAddr(bool isInMem):isInMem(isInMem){}

    void setMemAddr(unsigned int addr) {
        this->isInMem = true;
        this->memAddr = addr;
    }

    void loadOutMem(unsigned int num) {
        this->isInMem = false;
        this->regNum = num;
    }

    void setRegNum(unsigned int num) {
        this->isInMem = false;
        this->regNum = num;
    }

    unsigned int getAddr() {//在内存中返回内存中位置否则返回当前寄存器
        if (isInMem) {
            return this->memAddr;
        }
        return this->regNum;
    }

    bool isStoredInMem() {
        return this->isInMem;
    }
};

class IRTranslator{//中间代码的翻译器
    vector<BasicBlk>& blks;
    ofstream& mipsFile;
    SymbolTable* table;
    unsigned int memAlloc = 0;
    unsigned int regAlloc = 8;
    bool switchText = false;
    // 关于空间的分配原则：全局变量在data段申请空间，程序的分段内声明的变量在栈中存储
    // 其他在生成中间代码的时候产生的变量，使用寄存器和内存，寄存器不足再使用内存空间
    unordered_map<string, regNum> varRegisterMap;//标注变量分配的寄存器
    unordered_map<string, unsigned int> varMemoryMap;
    unordered_map<regNum,string> regAllocMap;
    BasicBlk* curBlk{};
    string curFunctionName = ".global.";
    unsigned int curStackAlloc = 0;
    unsigned int pushParaStack = 0;

    void translateBlk(BasicBlk& blk);
    void gConstDefTranslator(IRStatement& statement);
    void gVarDefTranslator(IRStatement& statement);
    void pConstDefTranslator(IRStatement& statement);
    void pVarDefTranslator(IRStatement& statement);
    void funcDefLabelTranslator(IRStatement& statement);
    void nonRetFuncCallTranslator(IRStatement& statement);
    void retFuncCallTranslator(IRStatement& statement);
    void funcDefParaTranslator(IRStatement& statement);
    void funcCallParaTranslator(IRStatement& statement);
    void voidRetTranslator(IRStatement& statement);
    void valueRetTranslator(IRStatement& statement);
    void statLabelTranslator(IRStatement& statement);
    void jumpTranslator(IRStatement& statement);
    void readIntTranslator(IRStatement& statement);
    void readCharTranslator(IRStatement& statement);
    void printStringTranslator(IRStatement& statement);
    void printIntTranslator(IRStatement& statement);
    void printCharTranslator(IRStatement& statement);
    void printNewLineTranslator(IRStatement& statement);
    void operationTranslator(IRStatement& statement);
    void assignTranslator(IRStatement& statement);
    void geqJumpTranslator(IRStatement& statement);
    void greJumpTranslator(IRStatement& statement);
    void lesJumpTranslator(IRStatement& statement);
    void leqJumpTranslator(IRStatement& statement);
    void eqlJumpTranslator(IRStatement& statement);
    void neqJumpTranslator(IRStatement& statement);
    void outMips(string& str);
    void funcCallBeginTranslator(IRStatement& statement);
    void funcRetAddrSwTranslator();
    regNum applyTmpRegister(string& varName);
    void flushRegMap();
    unsigned int sceneStore();
    void sceneLoad();
    void registerRotate();
    bool flushVariable(string& name,regNum valueReg); // 更新原本已经分配了内存的变量
    void storeToStack(string& varName, regNum valueReg); // 原本在栈中声明空间的变量更新
    void storeToMemory(string& varName, regNum valueReg); // 原本在内存中声明内存的变量更新
    void storeToMemory(regNum reg, unsigned int addr);
    bool loadVariable(string& name,regNum valueReg);
    void loadFromStack(string& varName, regNum valueReg); // 原本在栈中声明空间的变量更新
    void loadFromMemory(string& varName, regNum valueReg); // 原本在内存中声明内存的变量更新
    void loadFromMemory(regNum reg, unsigned int addr);
public:
    explicit IRTranslator(vector<BasicBlk>& blks,ofstream& out,SymbolTable* table) : blks(blks),mipsFile(out),table(table){}
    void translateIR();
};

class MipsGenerator{//mips代码生成器
    BasicClassifier* classifier;//读取IR并且进行预处理(划分基本快)
    IROptimal* optimal;//对预处理过的中间代码进行优化
    IRTranslator* translator;//翻译中间代码为mips
    vector<BasicBlk> blks;
public:
    explicit MipsGenerator(vector<IRStatement>& statements,ofstream& mipsFile,SymbolTable* table){
        this->classifier = new BasicClassifier(blks,statements);
        this->optimal = new IROptimal(blks,table);
        this->translator = new IRTranslator(blks,mipsFile,table);
    }
    void generateMips(bool needOptimal);
};
#endif