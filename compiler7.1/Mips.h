#ifndef COMPILER6_2_MIPS_H
#define COMPILER6_2_MIPS_H
#include <iostream>
#include <unordered_set>
#include <vector>
#include <fstream>
#include "Signary.h"
#include "IRGenerator.h"
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

    vector<IRStatement> getInterCode() {
        return this->statements;
    }

    void resetIRs(vector<IRStatement>& irs) {
        this->statements.clear();
        for (const auto& stmt : irs) {
            statements.push_back(stmt);
        }
    }
};

class BasicClassifier{
    vector<BasicBlk>& blks;
    vector<IRStatement> statements{};
    ofstream& beforeFile;
    ofstream& myBeforeFile;
public:
    explicit BasicClassifier(vector<BasicBlk>& blks,  ofstream& beforeFile, ofstream& myBeforeFile):blks(blks), beforeFile(beforeFile), myBeforeFile(myBeforeFile){}

    void nonOptimalIROutput(IRSyntaxNode *irSyntaxNode);

    void divideBasicBlk();

};

class IROptimal{//中间代码块优化
    SymbolTable* table;
    vector<BasicBlk>& blks;
    ofstream& afterFile;
    ofstream& myAfterFile;
public:
    explicit IROptimal(SymbolTable* table,vector<BasicBlk>& blks, ofstream& afterFile, ofstream& myAfterFile):table(table), blks(blks), afterFile(afterFile), myAfterFile(myAfterFile){}


    void firstRoundCopyDiffuse();

    void secondRoundCopyDiffuse();

    void constantDiffuse();

    void  commenSubExpr(); // 消除公共子表达式

    void jumpLabelOptimal();

    void optimizedIROutput();

};

class IRTranslator{//中间代码的翻译器
    vector<BasicBlk>& blks;
    ofstream& mipsFile;
    SymbolTable* table;
    unsigned int memAlloc = 0x10000000; // data区的起始地址
    unsigned int regAlloc = 8;
    bool switchText = false;
    // 关于空间的分配原则：全局变量在data段申请空间，程序的分段内声明的变量在栈中存储
    // 其他在生成中间代码的时候产生的变量，使用寄存器和内存，寄存器不足再使用内存空间
    unordered_map<string, regNum> varRegisterMap;//标注变量分配的寄存器
    unordered_map<string, unsigned int> varMemoryMap;
    unordered_map<regNum,string> regAllocMap;
    unordered_set<regNum> lockedReg;
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
    void outMips(const string& str);
    void funcCallBeginTranslator(IRStatement& statement);
    void funcRetAddrSwTranslator();
    bool flushVariable(string& name,regNum valueReg); // 更新原本已经分配了内存的变量
    void storeToStack(string& varName, regNum valueReg); // 原本在栈中声明空间的变量更新
    void storeToMemory(string& varName, regNum valueReg); // 原本在内存中声明内存的变量更新
    void storeToMemory(regNum reg, unsigned int addr);
    bool loadVariable(string& name,regNum valueReg);
    void loadFromStack(string& varName, regNum valueReg); // 原本在栈中声明空间的变量更新
    void loadFromMemory(string& varName, regNum valueReg); // 原本在内存中声明内存的变量更新
    void loadFromMemory(regNum reg, unsigned int addr);
    void flushRegMap();
    unsigned int sceneStore();
    void sceneLoad();
    vector<regNum> applyTmpRegister(vector<string> &varNames, vector<bool> &needLoads);
    regNum tmpRegisterPool(SymbolName &varName, bool needLoad);
    void tmpRegisterRotate();
public:
    explicit IRTranslator(vector<BasicBlk>& blks,ofstream& out,SymbolTable* table) : blks(blks),mipsFile(out),table(table){}
    void translateIR();
};

class MipsGenerator{//mips代码生成器
    BasicClassifier* classifier;//读取IR并且进行预处理(划分基本快)
    IROptimal* optimal;//对预处理过的中间代码进行优化
    IRTranslator* translator;//翻译中间代码为mips
    vector<BasicBlk> blks;
    IRSyntaxNode* root;

public:
    explicit MipsGenerator(ofstream &mipsFile,
                           ofstream &beforeFile, ofstream &afterFile,
                           ofstream &myBeforeFile, ofstream &myAfterFile,
                           SymbolTable *table, IRSyntaxNode *tree) {
        this->classifier = new BasicClassifier(blks, beforeFile, myBeforeFile);
        this->optimal = new IROptimal(table, blks, afterFile, myAfterFile);
        this->translator = new IRTranslator(blks, mipsFile, table);
        this->root = tree;
    }
    void generateMips(bool needOptimal);
};
#endif