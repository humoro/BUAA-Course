#ifndef COMPILER4_1_SIGNARY_H
#define COMPILER4_1_SIGNARY_H
//符号表相关
#include<iostream>
#include<vector>
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
struct SymbolTable{
    unordered_map<SymbolName,SymbolTableEntry*> entryTable;
    unordered_map<SymbolName,SymbolTableEntry*>* curEntryTable = &entryTable;//该引用用来指向最近的复合语句块的符号表最初的默认是指向全局符号表
    FunctionEntry* curFunction;
    //由于在本文法中不会出现符号表的嵌套（复合语句中无法嵌套复合语句）所以采用这种符号表的组织方式！！！！！！！！！！！！！！
    //该组织方式要求在编译程序的时候要在一定的时间及时的更新当前的符号表引用的指向
};

//-----------------------------------相关函数声明-------------------------------//
bool entryExist(SymbolName& name);
bool isReturnFunc(SymbolName& name);
bool isVoidFunc(SymbolName& name);
bool isIntReturnFunc(SymbolName& name);
bool isCharReturnFunc(SymbolName& name);
bool isExistVar(SymbolName& name);
bool isConstVar(SymbolName& name);
bool isIntVar(SymbolName& name);
bool isIntArray(SymbolName& name);
void addFunction(tokenType type,SymbolName& funcName);
void matchFunctionParameter(string& functionName,vector<ExprType>& valueList);
void addFunctionParameter(tokenType varType, string& varName);
void checkFuntionReturn();
void addFunctionReturn(ExprType type);
void addVariableEntry(tokenType varType,SymbolName& varName,bool isConst);
void addArrayEntry(tokenType varType,SymbolName& varName);
#endif 
