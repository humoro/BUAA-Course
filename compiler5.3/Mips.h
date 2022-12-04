#ifndef COMPILER5_3_MIPS_H
#define COMPILER5_3_MIPS_H
#include<iostream>
#include <vector>
#include <fstream>
#include "Signary.h"

using namespace std;
class IRStatement{
    string IRstr;//中间代码内容

    bool isBreakPoint = false;
public:
    explicit IRStatement(string& str) : IRstr(str){}

    void setBreakPoint() {
        this->isBreakPoint = true;
    }//说明该语句是一个基本的块的入口或者出口

    bool isBPState() {
        return this->isBreakPoint;
    }
};

class BasicBlk{//基本快
    vector<IRStatement> statements;
public:
    IRStatement operator[] (const unsigned int index) {
        return this->statements[index];
    }

    void push_statement(IRStatement& statement) {
        this->statements.push_back(statement);
    }
};

class IRReader{
    vector<BasicBlk>& blks;
    ifstream& irFile;
public:
    explicit IRReader(vector<BasicBlk>& blks,ifstream& in) : blks(blks),irFile(in){}

    void readIR();
};

class IROptimal{//中间代码块优化
    vector<BasicBlk>& blks;
    SymbolTable* table;
public:
    explicit IROptimal(vector<BasicBlk>& blks,SymbolTable* table) : blks(blks),table(table){}

    void optimizeIR();
};

class IRTranslator{//中间代码的翻译器
    vector<BasicBlk>& blks;
    ofstream& mipsFile;
    SymbolTable* table;
public:
    explicit IRTranslator(vector<BasicBlk>& blks,ofstream& out,SymbolTable* table) : blks(blks),mipsFile(out),table(table){}

    void translateIR();
};

class MipsGenerator{//mips代码生成器
    IRReader* reader;//读取IR并且进行预处理(划分基本快)
    IROptimal* optimal;//对预处理过的中间代码进行优化
    IRTranslator* translator;//翻译中间代码为mips
    vector<BasicBlk> blks;
    SymbolTable* table;
public:
    explicit MipsGenerator(ifstream& irFile,ofstream& mipsFile,SymbolTable* table){
        this->reader = new IRReader(blks,irFile);
        this->optimal = new IROptimal(blks,table);
        this->translator = new IRTranslator(blks,mipsFile,table);
        this->table = table;
    }

    void generateMips(bool needOptimal);
};
#endif