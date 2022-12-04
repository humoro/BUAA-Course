#pragma once
#ifndef COMPILER6_2_SIGNARY_H
#define COMPILER6_2_SIGNARY_H
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
        symbolTableEntrys = new unordered_map<SymbolName,SymbolTableEntry*>;
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
    int strNum = 0;
    unordered_map<string,string> strNameMap;
    unordered_map<string,string> strMap;

    bool partialExist(SymbolName& name);
    Function* addVoidFunction(SymbolName& name, bool isAdd);
    Function* addIntReturnFunc(SymbolName& name, bool isAdd);
    Function* addCharReturnFunc(SymbolName& name, bool isAdd);
    SymbolTableEntry* createVariableEntry(TokenType varType, string& varName, bool isConst);
    bool addEntry(SymbolTableEntry* entry);
    bool functionExist(SymbolName& functionName);
public:
    void setGlobalConstAddr(SymbolName& name,unsigned int addr);
    void setGlobalVarAddr(SymbolName& name,unsigned int addr);
    void setGlobalArrayInfo(SymbolName& name,unsigned int size,unsigned int addr);
    void setPartialConstAddr(SymbolName& varName,SymbolName& functionName,unsigned int addr);
    void setPartialVarAddr(SymbolName& varName,SymbolName& functionName,unsigned int addr);
    void setPartialArrayInfo(SymbolName& arrayName, SymbolName& functionName, unsigned int size, unsigned int addr);
    bool entryExist(SymbolName& name);
    bool isReturnFunction(SymbolName& name);
    bool isVoidFunction(SymbolName& name);
    bool isIntReturnFunc(SymbolName& name);
    bool isCharReturnFunc(SymbolName& name);
    bool variableExist(SymbolName& name);
    bool isConstVariable(SymbolName& name);
    bool isIntVariable(SymbolName& name);
    bool isCharArray(SymbolName& name);
    bool isIntArray(SymbolName& name);
    bool isPartialVariable(SymbolName& varName, SymbolName& fatherName);
    bool globalExist(SymbolName& name);
    bool addFunction(TokenType type, SymbolName& funcName);
    bool addFunctionParameter(TokenType varType, string& varName);
    bool addVariableEntry(TokenType varType, SymbolName& varName, bool isConst);
    bool addArrayEntry(TokenType varType, SymbolName& varName,unsigned int size);
    bool getFunctionParaList(SymbolName& functionName,vector<SymbolTableEntry*>& ansParas);
    Function* getCurrentFunction();
    SymbolType getSymbolType(SymbolName& name);
    string applyStringName(string& printStr);
    unordered_map<string,string> getStrNameMap();
    unsigned int getPartialArrayAddr(SymbolName& arrayName, SymbolName& fatherName);
    unsigned int getPartialVarAddr(SymbolName& varName, SymbolName& fatherName);
    bool isPartialVarRefreshed(SymbolName &varName, SymbolName& fatherName);
    bool isGlobalVarRefreshed(SymbolName& varName);
    void setGlobalVariableRefreshed(SymbolName& varName);
    void setPartialVariableRefreshed(SymbolName& varName, SymbolName& fatherName);
};
#endif 
