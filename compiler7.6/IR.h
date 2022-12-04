#pragma once
#ifndef COMPILER7_2_IR_H
#define COMPILER7_2_IR_H
#include "Type.h"
#include "Signary.h"
#include "StrTNum.h"
#include <utility>
#include <vector>
#include <fstream>
#include <unordered_set>
using namespace std;
class IRStatement{
    string IRstr;//中间代码内容
    IRType type;
    bool isBreakPoint = false;
    unsigned int code = 0;
    unordered_set<unsigned int> gen;
    unordered_set<unsigned int> kill;
public:
    IRStatement(string& str,IRType type) : IRstr(str),type(type){}

    IRStatement(string& str,IRType type, unsigned int number) : IRstr(str),type(type),code(number){}

    IRStatement(IRStatement* stmt,string& ir) {
        this->IRstr = ir;
        this->type = stmt->type;
        this->code = stmt->code;
    }

    IRStatement(IRStatement* stmt,unsigned int code) {
        this->IRstr = stmt->IRstr;
        this->type = stmt->type;
        this->code = code;
    }

    IRStatement(IRStatement& stmt,unsigned int code) {
        this->IRstr = stmt.IRstr;
        this->type = stmt.type;
        this->code = code;
    }

    void setBreakPoint(){
        this->isBreakPoint = true;
    }

    bool isBPStatement() {
        return this->isBreakPoint;
    }

    IRType getIRType() {
        return this->type;
    }

    string getIR() {
        return this->IRstr;
    }

    void setCodeNumber(unsigned int number) {
        this->code = number;
    }

    unsigned int getCodeNumber() {
        return this->code;
    }
};

class IRSyntaxNode{
    TokenType type;
    vector<IRSyntaxNode*> kids;
    vector<IRStatement> interCodes{}; //该语法树节点所对应生成的中间代码
public:
    explicit IRSyntaxNode(TokenType type) : type(type){
        this->kids = vector<IRSyntaxNode*>(0, nullptr);
    }

    void addKid(IRSyntaxNode* node) {
        this->kids.push_back(node);
    }

    void addInterStatement(IRStatement& stmt) {
        this->interCodes.push_back(stmt);
    }

    vector<IRSyntaxNode*> getKids() {
        return this->kids;
    }

    vector<IRStatement> getInterCodes() {
        return this->interCodes;
    }
};

class IRGenerator{
    int forLabelNum = 0;//for循环
    int doLabelNum = 0;//do-while循环
    int whileLabelNum = 0;//while循环
    int opSymNum = 0;//变量标签
    int codeLabelNum = 0;
    const string partialConstDef = "@const ";
    const string partialVariableDef = "@var ";
    const string globalConstDef = "@const ";
    const string globalVariableDef = "@var ";
    const string functionDefParaIR = "@paraDef ";
    const string functionCallParaIR = "@pushPara ";
    const string readIR = "@input@";
    const string retFuncCallIR = "@call@retFunc ";
    const string voidFuncCallIR = "@call@voidFunc ";
    const string voidFuncReturnIR = "@ret@void ";
    const string returnFuncReturnIR = "@ret@value ";
    const string printIR = "@print@";
    const string varLabel = "var@";
    const string forLabel = "__for";
    const string doLabel = "__do";
    const string whileLabel = "__while";
    const string codeLabel = "__codeBlk";
    const string spareRetAddr = "@para@retaddr";

    static void generateIR(string &str, IRType type, bool isBP, IRSyntaxNode *node);

public:
    explicit IRGenerator() = default;
    void partialConstDefIR(string &name, string &typeString, int value, IRSyntaxNode *node);
    void globalConstDefIR(string &name, string &typeString, int value, IRSyntaxNode *node);
    void partialVariableDefIR(string &name, string &typeString, unsigned int size, IRSyntaxNode *node);
    void globalVariableDefIR(string &name, string &typeString, unsigned int size, IRSyntaxNode *node);
    void functionDef(string &funcName, string &typeString, IRSyntaxNode *node);
    void functionParaDef(string &paraName, string &typeString, IRSyntaxNode *node);
    void functionCallPara(string &paraName, IRSyntaxNode *node);
    void read(SymbolType type, string &rName, IRSyntaxNode *node);
    void operationIR(string &op, string &opSym1, string &opSym2, string &ans, IRSyntaxNode *node);
    void assignIR(string &left, string &right, IRSyntaxNode *node);
    void retFunctionCall(string &leftSymbol, string &funcName, IRSyntaxNode *node);
    void voidFunctionCall(string &funcName, IRSyntaxNode *node);
    void valueReturn(string &name, IRSyntaxNode *node);
    void voidReturn(IRSyntaxNode *node);
    void printString(string &str, string &content, IRSyntaxNode *node);
    void printExpr(string &name, ExprType type, IRSyntaxNode *node);
    void printNewline(IRSyntaxNode *node);
    void conditionalJump(string &label, string &condition, IRSyntaxNode *node);
    void unconditionalJump(string &label, IRSyntaxNode *node);
    void setDownLabel(string &label, IRSyntaxNode *node);
    void spareRetStackAddr(string &funcName, IRSyntaxNode *node);
    void endProgram(IRSyntaxNode *node);
    string applyVariableName();
    string applyForCycleLabel();
    string applyDoCycleLabel();
    string applyWhileCycleLabel();
    string applyCodeBlkLabel();
};
//基本块
class BasicBlk{
    vector<IRStatement*> statements;
    string functionName;
    unsigned int blkNumber = 0;
    unsigned int memSize = 0;//记录基本块中的临时变量占用的内存
    unordered_set<unsigned int> kids;
    unordered_set<unsigned int> parents;
    unordered_set<unsigned int> gen;
    unordered_set<unsigned int> kill;
    unordered_set<string> in;
    unordered_set<string> out;
    unordered_set<string> use;
    unordered_set<string> def;
public:
    explicit BasicBlk(string& function):functionName(function){}
    IRStatement* operator[] (const unsigned int index) {
        return this->statements[index];
    }

    IRStatement* back() {
        return this->statements.back();
    }

    void push_statement(IRStatement* statement) {
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

    vector<IRStatement*> getInterCode() {
        return this->statements;
    }

    void resetIRs(vector<IRStatement*>& irs) {
        this->statements.clear();
        for (unsigned long long i = 0;i < irs.size();i++) {
            auto stmt = irs[i];
            statements.push_back(new IRStatement(stmt,i));
        }
    }

    void resetIR(unsigned long long index, IRStatement* statement) {
        this->statements[index] = statement;
    }

    string getFunction() {
        return this->functionName;
    }

    void setBlkNumber(unsigned int number) {
        this->blkNumber = number;
    }

    void addKid(unsigned int kid) {
        this->kids.insert(kid);
    }

    void addParent(unsigned int parent) {
        this->parents.insert(parent);
    }

    unordered_set<unsigned int> getKids() {
        return this->kids;
    }

    unordered_set<unsigned int> getParents() {
        return this->parents;
    }

    unsigned int getNumber() {
        return this->blkNumber;
    }
};

// 基本快的划分
class BasicClassifier{
    vector<BasicBlk*>& blks;
    vector<IRStatement> statements{};
    ofstream& beforeFile;
    ofstream& myBeforeFile;
public:
    explicit BasicClassifier(vector<BasicBlk*>& blks,  ofstream& beforeFile, ofstream& myBeforeFile):blks(blks), beforeFile(beforeFile), myBeforeFile(myBeforeFile){}
    void nonOptimalIROutput(IRSyntaxNode *irSyntaxNode);
    void divideBasicBlk();

};

class IROptimal{//中间代码块优化
    SymbolTable* entryTable;
    strTNum* strTransformer;
    vector<BasicBlk*>& blks;
    ofstream& afterFile;
    ofstream& myAfterFile;

    void firstRoundCopyDiffuseFunction(BasicBlk* blk);
    void secondRoundCopyDiffuseFunction(BasicBlk* blk);
    void thirdRoundCopyDiffuseFunction(BasicBlk* blk);
    void preCalculateFuncrtion(BasicBlk* blk); // 提前对全常数的计算式子进行计算
    void forthRoundCopyDiffuseFunction(BasicBlk* blk);

    bool isConstVariable(string& str, string& functionName) {
        return this->strTransformer->isConstVariable(str, functionName);
    }

    long long stringToNumber(string& str, string& functionName) {
        return this->strTransformer->stringToNumber(str, functionName);
    }

public:
    explicit IROptimal(SymbolTable* table,vector<BasicBlk*>& blks, ofstream& afterFile, ofstream& myAfterFile): entryTable(table), blks(blks), afterFile(afterFile), myAfterFile(myAfterFile) {
        this->strTransformer = new strTNum(table);
    }

    // 代码极小化优化
    void firstRoundCopyDiffuseOptimization();
    void secondRoundCopyDiffuseOptimization();
    void thirdRoundCopyDiffuseOptimization();
    // 赋值传播优化，常量传播优化
    void forthRoundCopyDiffuseOptimization();
    // 优化后的中间代码输出
    void optimizedIROutput();

};

#endif
