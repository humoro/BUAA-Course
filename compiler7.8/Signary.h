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

	vector<string> getParameterNameList() {
        vector<string> ans;
        for (auto entry : this->parameters) {
            ans.push_back(entry->getName());
        }
        return ans;
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

    void addEntry(SymbolName& name, SymbolTableEntry* entry) {
        (*(this->symbolTableEntrys))[name] = entry;
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
    bool translateIR_isGlobalEntry(SymbolName& varName);
    bool translateIR_isPartialEntry(SymbolName& varName, SymbolName& fatherName);
    bool translateIR_isGlobalVariableRefreshed(SymbolName& varName);
    bool translateIR_isPartialVariableRefreshed(SymbolName &varName, SymbolName& fatherName);
    bool translateIR_isGlobalConstant(SymbolName& varName);
    bool translateIR_isPartialConstant(SymbolName& varName, SymbolName& fatherName);

    void translateIR_setGlobalArrayInfo(SymbolName& name, unsigned int size, unsigned int addr);
    void translateIR_setPartialArrayInfo(SymbolName& arrayName, SymbolName& functionName, unsigned int size, unsigned int addr);
    void translateIR_setGlobalConstantInfo(SymbolName &name, unsigned int addr, long long value);
    void translateIR_setPartialConstantInfo(SymbolName &varName, SymbolName &functionName, unsigned int addr, long long value);
    void tranlasteIR_setGlobalVariableInfo(SymbolName& name, unsigned int addr);
    void translateIR_setPartialVariableInfo(SymbolName& varName, SymbolName& functionName, unsigned int addr);
    void translateIR_setGlobalVariableRefreshed(SymbolName& varName);
    void translateIR_setPartialVariableRefreshed(SymbolName& varName, SymbolName& fatherName);
    void translateIR_setGlobalVariableLoaded(SymbolName& varName);
    void translateIR_setPartialVariableLoaded(SymbolName& varName, SymbolName& fatherName);

    unsigned int translateIR_getPartialVariableAddr(SymbolName& varName, SymbolName& fatherName);
    unsigned int translateIR_getPartialArrayAddr(SymbolName& varName, SymbolName& fatherName);
    long long translateIR_getPartialConstantValue(SymbolName &varName, SymbolName &fatherName);
    long long translateIR_getGlobalConstantValue(SymbolName &varName);
    vector<string> translateIR_getFunctionParameterList(SymbolName& functionName);
    unordered_map<string,string> translateIR_getStrNameMap();
    void translateIR_addInlineFunctionVariable(SymbolName& varName, SymbolName &functionName, const string& tokenType);
    void translateIR_addInlineFunctionConstant(SymbolName &varName, long long value, SymbolName& functionName, const string& tokenType);
    void translateIR_addInlineFunctionArray(SymbolName& varName, SymbolName &functionName, long long size, string tokenType);
};
#endif 
