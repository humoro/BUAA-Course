#pragma once
#ifndef COMPILER7_2_SIGNARY_H
#define COMPILER7_2_SIGNARY_H
//符号表相关
#include <iostream>
#include <vector>
#include <unordered_map>
#include <stack>
#include "Type.h"
using namespace std;
typedef string SymbolName;
//----------------------------涉及到的符号类-------------------------------------//
class SymbolTableEntry{//符号表表项的基类
protected:
    SymbolName sName;
    SymbolType sType;
	SymbolName& fatherBlkName;//符号所属的复合语句块的名称（对于全局变量名称是".global."）

public:
    SymbolTableEntry(SymbolName& name,
                     SymbolType type,
                     SymbolName& fatherBlkName) :
                     sName(name),sType(type),fatherBlkName(fatherBlkName){
	}

    SymbolName getName() {
        return this->sName;
    }

    SymbolType getType() {
        return this->sType;
    }

};

class Array : public SymbolTableEntry {
    unsigned int addr = 0;
    unsigned int size = 0;
public:
    explicit Array(SymbolName& name,
          SymbolType type,
          SymbolName& fatherBlkName,unsigned int scale) :
          SymbolTableEntry(name,type,fatherBlkName),size(scale){}

    void allocAddr(unsigned int address) {
        this->addr = address;
    }

    void setSize(unsigned int scale) {
        this->size = scale;
    }

    unsigned int getAddr() {
        return this->addr;
    }
};

class Variable : public SymbolTableEntry {
    unsigned int addr = 0;
    long long value = 0;
    bool isRefreshed = true;
public:
    explicit Variable(SymbolName& name,
             SymbolType type,
             SymbolName& fatherBlkName) :
             SymbolTableEntry(name,type,fatherBlkName){}

    void allocAddr(unsigned int address) {
        this->addr = address;
    }

    unsigned int getAddr() {
        return this->addr;
    }

    void setRefreshed() {
        this->isRefreshed = true;
    }

    void setValue(long long v) {
        this->value = v;
    }

    long long getValue() {
        return this->value;
    }

    void setLoaded() {
        this->isRefreshed = false;
    }

    bool hasBeenRefreshed() {
        return this->isRefreshed;
    }
};

class Function : public SymbolTableEntry {//函数的符号表项
private:
	vector<SymbolTableEntry*> parameters;
	unordered_map<SymbolName,SymbolTableEntry*>* symbolTableEntrys;
	bool haveReturn = false;
public:
    Function(SymbolName& name,
             SymbolType type,
             SymbolName& fatherBlkName) :
                  SymbolTableEntry(name, type,fatherBlkName) {
        this->symbolTableEntrys = new unordered_map<SymbolName,SymbolTableEntry*>;
    }

	vector<SymbolTableEntry*> getParameters() {
		return this->parameters;
	}

	void addParameter(SymbolTableEntry* parameter) {
		this->parameters.push_back(parameter);
	}//给函数符号表项添加参数

    unordered_map<SymbolName,SymbolTableEntry*>* getSymbolTableEntry() {
        return this->symbolTableEntrys;
    }

    void addReturnStatement() {
        this->haveReturn = true;
    }

    bool haveReturnStatement() {
        return this->haveReturn;
    }
};

//------------------------------------符号表类----------------------------------//
class SymbolTable{
    SymbolName globalName = ".global.";
    unordered_map<SymbolName,SymbolTableEntry*> entryTable;
    unordered_map<SymbolName,SymbolTableEntry*>* curEntryTable = &entryTable;
    Function* curFunction = nullptr;
    unsigned int strNum = 0;
    unordered_map<string,string> strNameMap;
    unordered_map<string,string> strMap;

    // 语法分析需要的符号表函数
    bool syntaxParse_globalExist(SymbolName& name);
    bool syntaxParse_partialExist(SymbolName& name);
    bool syntaxParse_functionExist(SymbolName& functionName);

    Variable * syntaxParse_createVariableEntry(TokenType varType, string& varName, bool isConst);
    bool syntaxParse_addEntry(SymbolTableEntry* entry);

    Function* syntaxParse_addVoidFunction(SymbolName& name, bool isAdd);
    Function* syntaxParse_addIntReturnFunc(SymbolName& name, bool isAdd);
    Function* syntaxParse_addCharReturnFunc(SymbolName& name, bool isAdd);
public:
    // 语法分析需要的符号表函数
    bool syntaxParse_entryExist(SymbolName& name);
    bool syntaxParse_variableExist(SymbolName& name);

    bool syntaxParse_isReturnFunction(SymbolName& name);
    bool syntaxParse_isVoidFunction(SymbolName& name);
    bool syntaxParse_isIntReturnFunc(SymbolName& name);
    bool syntaxParse_isCharReturnFunc(SymbolName& name);
    bool syntaxParse_isConstVariable(SymbolName& name);
    bool syntaxParse_isIntVariable(SymbolName& name);
    bool syntaxParse_isCharArray(SymbolName& name);
    bool syntaxParse_isIntArray(SymbolName& name);

    bool syntaxParse_addVariableEntry(TokenType varType, SymbolName& varName, bool isConst, long long value);
    bool syntaxParse_addArrayEntry(TokenType varType, SymbolName& varName, unsigned int size);
    bool syntaxParse_addFunction(TokenType type, SymbolName& funcName);
    bool syntaxParse_addFunctionParameter(TokenType varType, string& varName);

    Function* syntaxParse_getCurrentFunction();
    SymbolType syntaxParse_getSymbolType(SymbolName& name);
    bool syntaxParse_getFunctionParaList(SymbolName& functionName, vector<SymbolTableEntry*>& ansParas);

    // 生成中间代码的符号表函数
    string generateIR_applyStringName(string& printStr);

    // 翻译中间代码的符号表函数
    bool translateIR_isGlobalVariable(SymbolName& varName);
    bool translateIR_isPartialVariable(SymbolName& varName, SymbolName& fatherName);
    bool translateIR_isGlobalVarRefreshed(SymbolName& varName);
    bool translateIR_isPartialVarRefreshed(SymbolName &varName, SymbolName& fatherName);
    bool translateIR_isGlobalConstVar(SymbolName& varName);
    bool translateIR_isPartialConstVar(SymbolName& varName, SymbolName& fatherName);

    void translateIR_setGlobalArrayInfo(SymbolName& name, unsigned int size, unsigned int addr);
    void translateIR_setPartialArrayInfo(SymbolName& arrayName, SymbolName& functionName, unsigned int size, unsigned int addr);
    void translateIR_setGlobalConstInfo(SymbolName &name, unsigned int addr, long long value);
    void translateIR_setPartialConstInfo(SymbolName &varName, SymbolName &functionName, unsigned int addr, long long value);
    void tranlasteIR_setGlobalVarAddr(SymbolName& name, unsigned int addr);
    void translateIR_setPartialVarAddr(SymbolName& varName, SymbolName& functionName, unsigned int addr);
    void translateIR_setGlobalVariableRefreshed(SymbolName& varName);
    void translateIR_setPartialVariableRefreshed(SymbolName& varName, SymbolName& fatherName);
    void translateIR_setGlobalVariableLoaded(SymbolName& varName);
    void translateIR_setPartialVariableLoaded(SymbolName& varName, SymbolName& fatherName);

    unsigned int translateIR_getPartialVarAddr(SymbolName& varName, SymbolName& fatherName);
    unsigned int translateIR_getPartialArrayAddr(SymbolName& varName, SymbolName& fatherName);
    long long translateIR_getPartialConstValue(SymbolName &varName, SymbolName &fatherName);
    long long translateIR_getGlobalConstValue(SymbolName &varName);
    unordered_map<string,string> translateIR_getStrNameMap();
};
#endif 
