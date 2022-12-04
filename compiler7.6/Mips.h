#ifndef COMPILER7_2_MIPS_H
#define COMPILER7_2_MIPS_H
#include "IR.h"
#include "Signary.h"
#include "StrTNum.h"
#include <iostream>
#include <unordered_set>
#include <vector>
#include <fstream>
using namespace std;
typedef unsigned int RegisterNumber;
typedef unsigned int MemoryNumber;
typedef unsigned int BlockNumber;
class MipsCode{
    string mips;
    mipsType type;
public:
    explicit MipsCode(string& code,mipsType type):mips(code),type(type) {};

    string getCode() {
        return this->mips;
    }

    mipsType getType() {
        return this->type;
    }
};

class DefUseChain{ // 定义使用链
    string variable;
    unsigned int chainNumber;
    unordered_set<unsigned int> chain;
public:
    DefUseChain(string& variable, unsigned int number):variable(variable),chainNumber(number){}

    void addDefUseNode(unsigned int node) {
        chain.insert(node);
    }

    unordered_set<unsigned int> getChain() {
        return this->chain;
    }
};

class DefUseNet{ // 定义使用网
    string variable;
    unsigned int netNumber;
    unordered_set<DefUseChain*> net;
public:
    DefUseNet(string& variable, unsigned int number):variable(variable),netNumber(number){}

    void addDefUseChain(DefUseChain* chain) {
        this->net.insert(chain);
    }

    unordered_set<DefUseChain*> getNet() {
        return this->net;
    }
};

class ConflictGraphNode{
    string variable;
    unsigned int glbRegister = 0;
    unordered_set<ConflictGraphNode*> relevantNodes;
public:
    explicit ConflictGraphNode(string& variable):variable(variable){}

    void addRelevantNode(ConflictGraphNode* node) {
        this->relevantNodes.insert(node);
    }

    void allocGlbRegister(unsigned int reg) {
        this->glbRegister = reg;
    }

    unordered_set<ConflictGraphNode*> getRelevantNode() {
        return this->relevantNodes;
    }
};

class IRTranslator{//中间代码的翻译器
    vector<BasicBlk*>& blks;
    vector<MipsCode>& mipsCodes;
    SymbolTable* entryTable;
    strTNum* strTranformer;
    unsigned int memAlloc = 0x10000000; // data区的起始地址
    unsigned int regAlloc = 8;
    bool switchText = false;
    unsigned long long sceneMemory = 0; // 函数调用现场存储所需要的栈空间
    vector<string> sceneNames; // 再函数调用的时候数据分析所得到的不需要存储的变量
    // 关于空间的分配原则：全局变量在data段申请空间，程序的分段内声明的变量在栈中存储
    // 其他在生成中间代码的时候产生的变量，使用寄存器和内存，寄存器不足再使用内存空间
    unordered_map<BlockNumber, BasicBlk> blockMap;
    unordered_map<string, RegisterNumber> tmpVariableRegisterMap;//标注变量分配的寄存器
    unordered_map<RegisterNumber, string> tmpRegisterVariableMap;
    unordered_map<string, RegisterNumber> glbVariableRegisterMap;
    unordered_map<RegisterNumber, string> glbRegisterVariableMap;
    unordered_map<string, MemoryNumber> variableMemoryMap;
    unordered_set<RegisterNumber> lockedRegister; // 申请变量的时候冲突的寄存器，不能同时申请到同一个寄存器
    unordered_map<string, vector<DefUseNet*>> DefUseNets;
    unordered_set<ConflictGraphNode*> conflictGraph;
    BasicBlk* curBlk{};
    string curFunctionName = ".global.";
    unsigned int curStackAlloc = 0;
    unsigned int pushParaStack = 0;
    //----------------------------------------------中间代码的翻译函数--------------------------------------------------//
    void blkCodePreProcess(); // 数据流分析的预处理函数
    void dataFlowInitial();
    void dataFlowAnalysis(); // 数据流分析
    void activeVariableInitial();
    void activeVariableAnalysis(); // 活跃变量分析
    void buildDefUseNet();
    void buildConflictGraph(); // 构建冲突图
    void allocGlbRegister(); // 分配全局寄存器
    // 翻译语句的分发
    void translateBlk(BasicBlk* blk);
    // 常量和变量定义翻译
    void constantDefinitionTranslator(IRStatement* statement);
    void variableDefinitionTranslator(IRStatement* statement);
    // 代码块标签翻译
    void programLabelSetTranslator(IRStatement* statement);
    // 函数定义的翻译
    void funcDefLabelTranslator(IRStatement* statement);
    void funcDefParaTranslator(IRStatement* statement);
    // 函数调用翻译
    void funcCallBeginTranslator();
    void funcRetAddrSwTranslator();
    void funcCallParaTranslator(IRStatement* statement);
    void funcCallTranslator(IRStatement* statement, unsigned int beginloc);
    // 返回语句翻译
    void retStmtTranslator(IRStatement* statement);
    // 读语句翻译
    void readFunctionTranslator(IRStatement* statement);
    // 写语句翻译
    void printFunctionTranslator(IRStatement* statement);
    // 运算式翻译
    void operationTranslator(IRStatement* statement);
    // 赋值翻译
    void assignTranslator(IRStatement* statement);
    // 跳转语句翻译
    void conditionJumpTranslator(IRStatement* statement);
    void jumpTranslator(IRStatement* statement);
    //------------------------------------函数调用时候的现场的分析存储和恢复----------------------------------------------//
    unsigned int sceneMemoryAlloc();
    void sceneAnalysis(unsigned int beginloc);
    void sceneStore();
    void sceneLoad();
    void sceneClear();
    //-------------------------------------寄存器的分配和申请以及寄存器管理-----------------------------------------------//
    vector<RegisterNumber> applyTmpRegister(vector<string> &varNames, vector<bool> &needLoads);
    RegisterNumber tmpRegisterPool(SymbolName &varName, bool needLoad);
    void tmpRegisterRotate();
    void flushRegisterMap();
    //---------------------------------------和内存交互更新和加载变量数值------------------------------------------------//
    bool flushVariable(string& name, RegisterNumber valueReg); // 更新原本已经分配了内存的变量
    void storeToStack(string& varName, RegisterNumber valueReg); // 原本在栈中声明空间的变量更新
    void storeToMemory(string& varName, RegisterNumber valueReg); // 原本在内存中声明内存的变量更新
    void storeToMemory(RegisterNumber reg, unsigned int addr);
    bool loadVariable(string& name, RegisterNumber valueReg);
    void loadFromStack(string& varName, RegisterNumber valueReg); // 原本在栈中声明空间的变量更新
    void loadFromMemory(string& varName, RegisterNumber valueReg); // 原本在内存中声明内存的变量更新
    void loadFromMemory(RegisterNumber reg, unsigned int addr);
    //-------------------------------------------翻译中间代码的辅助函数-------------------------------------------------//
    long long stringToNumber(string& str) {
        return this->strTranformer->stringToNumber(str, this->curFunctionName);
    }

    bool isConstVariable(string &str) {
        return this->strTranformer->isConstVariable(str, this->curFunctionName);
    }

    void generateMipsCode(string str, mipsType type) {

        this->mipsCodes.emplace_back(str,type);
    }
public:
    explicit IRTranslator(vector<BasicBlk*>& blks,vector<MipsCode>& mips, SymbolTable* table) : blks(blks), mipsCodes(mips), entryTable(table) {
        this->strTranformer = new strTNum(table);
    }
    void translateIR();
};

class MipsOptimal{
    vector<MipsCode>& codes;
    ofstream& out;
public:
    explicit MipsOptimal(vector<MipsCode>& codes,ofstream& out):codes(codes),out(out){}

    void peepholeOptimization();

    void outputMips();
};

class MipsGenerator{//mips代码生成器
    BasicClassifier* classifier;//读取IR并且进行预处理(划分基本快)
    IROptimal* irOptimal;//对预处理过的中间代码进行优化
    MipsOptimal* mipsOptimal; // 最终代码窥孔优化器
    IRTranslator* translator;//翻译中间代码为mips
    vector<BasicBlk*> blks;
    vector<MipsCode> mipsCodes;
    IRSyntaxNode* root;

public:
    explicit MipsGenerator(ofstream &mipsFile,
                           ofstream &beforeFile, ofstream &afterFile,
                           ofstream &myBeforeFile, ofstream &myAfterFile,
                           SymbolTable *table, IRSyntaxNode *tree) {
        this->classifier = new BasicClassifier(blks, beforeFile, myBeforeFile);
        this->irOptimal = new IROptimal(table, blks, afterFile, myAfterFile);
        this->translator = new IRTranslator(blks, this->mipsCodes, table);
        this->mipsOptimal = new MipsOptimal(this->mipsCodes,mipsFile);
        this->root = tree;
    }
    void generateMips(bool needOptimal);
};
#endif