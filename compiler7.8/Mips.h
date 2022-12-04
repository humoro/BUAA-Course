#ifndef COMPILER7_2_MIPS_H
#define COMPILER7_2_MIPS_H
#include "Type.h"
#include "IR.h"
#include "Signary.h"
#include "StrTNum.h"
#include <iostream>
#include <unordered_set>
#include <vector>
#include <fstream>
using namespace std;
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
    unsigned int defNumber;
    unordered_set<unsigned int> chain;
public:
    DefUseChain(string& variable, unsigned int number):variable(variable),defNumber(number){}

    void addDefUseNode(unsigned int node) {
        chain.insert(node);
    }

    unordered_set<unsigned int> getChain() {
        return this->chain;
    }

    unsigned int getDefNode() {
        return this->defNumber;
    }
};

class ConflictNode{
    string variable;
    unordered_set<string> relevantNode;
public:
    explicit ConflictNode(string& name):variable(name){}

    void addRelevantNode(string& name) {
        this->relevantNode.insert(name);
    }

    unsigned int getDegree() {
        return this->relevantNode.size();
    }

    void eraseRelevant(string& name) {
        if (this->relevantNode.find(name) != this->relevantNode.end())
            this->relevantNode.erase(name);
    }
};

class ConflictGraph{
    unordered_map<string, ConflictNode*> graph;
    unordered_map<string, unordered_set<string>> edges;
public:
    void setGraph(unordered_map<string, ConflictNode*>& map) {
       for (const auto& it : map) {
           graph[it.first] = it.second;
       }
    }

    unordered_map<string, ConflictNode*> getGraph() {
        return this->graph;
    }

    void addEdge(string& node1, string& node2) {
        if (this->edges.find(node1) == this->edges.end()) {
            this->edges[node1] = unordered_set<string>(0);
        }
        if (this->edges.find(node2) == this->edges.end()) {
            this->edges[node2] = unordered_set<string>(0);
        }
        this->edges[node1].insert(node2);
        this->edges[node2].insert(node1);
    }

    bool haveEdge(string& node1, string& node2) {
        if (this->edges.find(node1) != this->edges.end()) {
            return this->edges[node1].find(node2) != this->edges[node1].end();
        }
        return false;
    }
};

class IRTranslator{//中间代码的翻译器
    ofstream& MBF;
    ofstream& BF;
    ofstream& MAF;
    ofstream& AF;
    vector<BasicBlk*> blks;
    vector<IRStatement*> statements;
    vector<MipsCode*>& mipsCodes;
    BasicBlk* globalBlk = nullptr;
    vector<string> functions;
    unsigned int tmpRegisterBegin = 8;
    unsigned int tmpRegisterEnd = 15;
    unsigned int glbRegisterBegin = 16;
    unsigned int glbRegisterEnd = 23;
    unsigned int tmpRegisterMemory = 32;
    unsigned int glbRegisterMemory = 32;
    unordered_map<string, vector<IRStatement*>> functionStatementsMap;
    unordered_map<string, bool> leafFunctionMap;
    unordered_map<string, unordered_set<string>> functionCallMap; // 该数据结构表示的是所有调用了其他函数的函数，并且标注调用其他函数的调用位置
    SymbolTable* entryTable;
    strTNum* strTranformer;
    unsigned int memAlloc = 0x10000000; // data区的起始地址
    unsigned int regAlloc = 8;
    bool switchText = false;
    unsigned long long sceneMemory = 0; // 函数调用现场存储所需要的栈空间
    vector<string> sceneNames; // 再函数调用的时候数据分析所得到的不需要存储的变量
    // 关于空间的分配原则：全局变量在data段申请空间，程序的分段内声明的变量在栈中存储
    // 其他在生成中间代码的时候产生的变量，使用寄存器和内存，寄存器不足再使用内存空间
    unordered_map<string, vector<BlockNumber>> functionBlks;
    unordered_map<BlockNumber, BasicBlk*> blockNumberBlkMap;
    unordered_map<BasicBlk*, BlockNumber> blkBlockNumberMap;
    unordered_set<RegisterNumber> loadedGlbRegister;
    unordered_set<string> loadedGlbVariable;
    unordered_map<string, RegisterNumber> tmpVariableRegisterMap;//标注变量分配的寄存器
    unordered_map<RegisterNumber, string> tmpRegisterVariableMap;
    unordered_map<string, unordered_map<string,RegisterNumber>> functionGlbRegisterMap;
    unordered_map<string, unsigned int> functionGlbNumber;
    unordered_map<string, RegisterNumber> glbVariableRegisterMap;
    unordered_map<RegisterNumber, string> glbRegisterVariableMap;
    unordered_map<string, MemoryNumber> variableMemoryMap;
    unordered_set<RegisterNumber> lockedRegister; // 申请变量的时候冲突的寄存器，不能同时申请到同一个寄存器
    unordered_map<unsigned int, string> defNodeVariableMap; // 变量的定义点 定义点一个点只能定义一个变量
    unordered_map<BlockNumber, vector<DefUseChain*>> blkDefUseChainMap; // 所有定义点所在的
    unordered_map<string,unordered_map<string, vector<DefUseChain*>>> defUseChains; // 所有变量的定义使用链
    unordered_map<string,ConflictGraph*> conflictGraph; // 变量冲突图
    BasicBlk* curBlk{};
    string curFunctionName = ".global.";
    unsigned int curStackAlloc = 0;
    unsigned int pushParaStack = 0;

    //----------------------------------------------------函数内联-----------------------------------------------------//
    IRStatement *inlineFunctionStatementTransform(IRStatement *stmt, string &function, string &retName); // 内联函数语句转换
    string inlineFunctionVariableTransform(string& function, string& varName); // 内联函数变量转换
    //--------------------------------------------------数据流分析-----------------------------------------------------//
    void blkPreProcess(); // 数据流分析和活跃变量分析的预处理函数
    static unordered_set<unsigned int> dataFlowUnion(const unordered_set<unsigned int>& left, const unordered_set<unsigned int>& right); // 集合的并运算
    static unordered_set<unsigned int> dataFlowDiff(const unordered_set<unsigned int>& left, const unordered_set<unsigned int>& right); // 集合的差运算
    void addDefNode(string& variable, IRStatement* stmt);
    void dataFlowInitial();
    void dataFlowAnalysis(); // 数据流分析
    //-------------------------------------------------活跃变量分析-----------------------------------------------------//
    static unordered_set<string> variableUnion(const unordered_set<string>& left, const unordered_set<string>& right);
    static unordered_set<string> variableDiff(const unordered_set<string>& left, const unordered_set<string>& right);
    void activeVariableInitial();
    void activeVariableAnalysis();
    void buildDefUseChain();
    static void buildConflictGraphBasedOnSet(unordered_set<string> &set,unordered_map<string, ConflictNode*>& nodemap,ConflictGraph* graph); // 在给定冲突几何上给给定图添加边
    void buildConflictGraph(); // 构建冲突图
    //----------------------------------------------中间代码的翻译函数--------------------------------------------------//
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
    void sceneStore(string &function);
    void sceneLoad(string &function);
    void sceneClear();
    //-------------------------------------寄存器的分配和申请以及寄存器管理-----------------------------------------------//
    vector<RegisterNumber> applyRegister(vector<string> &varNames, vector<bool> &needLoads);
    RegisterNumber applyRegister(SymbolName& varName, bool needLoad);
    RegisterNumber applyGlbRegister(SymbolName& varName, bool needLoad);
    RegisterNumber applyTmpRegister(SymbolName& varName, bool needLoad); // 临时寄存器池
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
    long long stoi(string& str);
    long long stoi(string& str, string& function);
    static bool isConditionalJumpIR(IRStatement* stmt);
    static bool isReadIR(IRStatement* stmt);
    static bool isPrintSymbolIR(IRStatement* stmt);
    static bool isReturnIR(IRStatement* stmt);
    static bool isUnconditionalJump(IRStatement* stmt);
    bool isConstVariable(string &str);
    bool isConstVariable(string& str, string& function);
    static bool isInnerVariable(string& variable);
    bool isUserDefinitionVariable(string& variable);
    void getInstructionOperationItem(IRStatement *stmt, string &info1, string &info2, string &info3, string &info4); // 获取之间代码有效信息函数
    //-------------------------------------------赋值传播和常量传播优化-------------------------------------------------//
    void firstRoundCopyDiffuseFunction(BasicBlk* blk);
    void secondRoundCopyDiffuseFunction(BasicBlk* blk);
    void thirdRoundCopyDiffuseFunction(BasicBlk* blk);
    void preCalculateFuncrtion(BasicBlk* blk); // 提前对全常数的计算式子进行计算
    void forthRoundCopyDiffuseFunction(BasicBlk* blk);
    //------------------------------------------------循环优化函数-----------------------------------------------------//
    void circulationStrengthWeakenOptimization();
    //---------------------------------------------------生成Mips-----------------------------------------------------//
    void generateMipsCode(string str, mipsType type);

public:
    explicit IRTranslator(vector<MipsCode*>& mips,
                          SymbolTable* table,
                          ofstream& MBF,
                          ofstream& BF,
                          ofstream& MAF,
                          ofstream& AF) : mipsCodes(mips), entryTable(table), MBF(MBF),BF(BF),MAF(MAF),AF(AF) {
        this->strTranformer = new strTNum(table);
    }
    void dataAnalysis();
    void deleteDeadCode();
    void functionInlineOptimal(); // 在做过死代码删除之后进行函数的内联，函数内联完成之后再进行数据流分析 在进行函数内联之后重新划分基本块的语句
    void translateIR();
    void variableAnalysis(); // 活跃变量分析主函数
    // 代码极小化优化
    void firstRoundCopyDiffuseOptimization();
    void secondRoundCopyDiffuseOptimization();
    void thirdRoundCopyDiffuseOptimization();
    // 赋值传播优化，常量传播优化
    void forthRoundCopyDiffuseOptimization();
    // 优化后的中间代码输出
    void AOptimizationIROutput();
    void BOptimizationIROutput(IRSyntaxNode *irSyntaxNode);
    void divideBasicBlk();
    void allocGlbRegister(bool needOptimal); // 依据构建的冲突图分配全局寄存器
    void circulationOptimization();
};

class MipsOptimal{
    vector<MipsCode*>& codes;
    ofstream& out;
public:
    explicit MipsOptimal(vector<MipsCode*>& codes,ofstream& out):codes(codes),out(out){}

    void peepholeOptimization();

    void outputMips();
};

class MipsGenerator{//mips代码生成器
    MipsOptimal* mipsOptimal; // 最终代码窥孔优化器
    IRTranslator* translator;//翻译中间代码为mips
    vector<MipsCode*> mipsCodes;
    IRSyntaxNode* root;

public:
    explicit MipsGenerator(ofstream &mipsFile,
                           ofstream &beforeFile, ofstream &afterFile,
                           ofstream &myBeforeFile, ofstream &myAfterFile,
                           SymbolTable *table, IRSyntaxNode *tree) {
        this->translator = new IRTranslator(this->mipsCodes, table, myBeforeFile, beforeFile, myAfterFile, afterFile);
        this->mipsOptimal = new MipsOptimal(this->mipsCodes,mipsFile);
        this->root = tree;
    }
    void generateMips(bool needOptimal);
};
#endif