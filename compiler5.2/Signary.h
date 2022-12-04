#pragma once
#ifndef COMPILER5_2_SIGNARY_H
#define COMPILER5_2_SIGNARY_H
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

class FunctionEntry : public SymbolTableEntry {//函数的符号表项
private:
	vector<SymbolTableEntry*> parameters;
	unordered_map<SymbolName,SymbolTableEntry*>* symbolTableEntrys;
	bool haveReturn = false;
public:
    FunctionEntry(SymbolName& name,
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

    void addReturnSatement() {
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
    FunctionEntry* curFunction = nullptr;


    bool partialExist(SymbolName& name);
    bool globalExist(SymbolName& name);
    FunctionEntry* addVoidFunc(SymbolName& name,bool isAdd);
    FunctionEntry* addIntReturnFunc(SymbolName& name,bool isAdd);
    FunctionEntry* addCharReturnFunc(SymbolName& name,bool isAdd);
    SymbolTableEntry* createSymbolEntry(TokenType varType, string& varName, bool isConst);
    bool addEntry(SymbolTableEntry* entry);
    bool isExistFunction(SymbolName& functionName);
public:
    bool entryExist(SymbolName& name);
    bool isReturnFunc(SymbolName& name);
    bool isVoidFunc(SymbolName& name);
    bool isIntReturnFunc(SymbolName& name);
    bool isCharReturnFunc(SymbolName& name);
    bool isExistVar(SymbolName& name);
    bool isConstVar(SymbolName& name);
    bool isIntVar(SymbolName& name);
    bool isCharArray(SymbolName& name);
    bool isIntArray(SymbolName& name);
    bool addFunction(TokenType type, SymbolName& funcName);
    bool addFunctionParameter(TokenType varType, string& varName);
    bool addVariableEntry(TokenType varType, SymbolName& varName, bool isConst);
    bool addArrayEntry(TokenType varType, SymbolName& varName);
    bool getFunctionParaList(SymbolName& functionName,vector<SymbolTableEntry*>& ansParas);
    FunctionEntry* getCurrentFunction();
    SymbolType getSymbolType(SymbolName& name);
};

//-----------------------------------相关函数声明-------------------------------//

#endif 
