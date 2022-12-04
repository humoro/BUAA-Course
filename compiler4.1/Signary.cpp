#include <unordered_map>
#include <cstdio>
#include "Signary.h"
#include "Error.h"
using namespace std;
SymbolTable entryTable;
extern long long linenum;
SymbolName globalName = ".global.";
bool partialExist(SymbolName& name) {//在当前的符号表中查询是否存在符号表项
    return !(entryTable.curEntryTable->find(name) == entryTable.curEntryTable->end());
}

bool globalExist(SymbolName& name) {//在程序的全局符号表查询是否存在符号项
    return (entryTable.entryTable.find(name) != entryTable.entryTable.end());
}

bool entryExist(SymbolName& name) {
    return partialExist(name) || globalExist(name);
}

//-----------------------------------函数符号表项的查询函数----------------------------------//
bool isReturnFunc(SymbolName& name) {//通过名称查询函数是否是返回值函数
    return isIntReturnFunc(name) || isCharReturnFunc(name);
}

bool isIntReturnFunc(SymbolName& name) {
    if (!globalExist(name)) return false;
    return entryTable.entryTable[name]->getType() == intReturnFunc;
}

bool isCharReturnFunc(SymbolName& name) {
    if (!globalExist(name)) return false;
    return entryTable.entryTable[name]->getType() == charReturnFunc;
}

bool isVoidFunc(SymbolName& name) {//通过名字查询是否存在对应名称的无返回值函数
	if (!globalExist(name)) return false;
	return entryTable.entryTable[name]->getType() == voidFunc;
}

bool isExistVar(SymbolName& name) {
    return (entryTable.curEntryTable->find(name) != entryTable.curEntryTable->end() ||
            entryTable.entryTable.find(name) != entryTable.entryTable.end());
}

bool isConstVar(SymbolName& name) {
    if (entryTable.curEntryTable->find(name) != entryTable.curEntryTable->end()) {//先找当前程序块中的定义
        return (*entryTable.curEntryTable)[name]->getType() == constIntVar ||
                (*entryTable.curEntryTable)[name]->getType() == constCharVar;
    } else if (entryTable.entryTable.find(name) != entryTable.entryTable.end()){//如果在当前程序块中没有找到变量的符号表项在去全局变量中找
        return entryTable.entryTable[name]->getType() == constIntVar ||
               entryTable.entryTable[name]->getType() == constCharVar;
    } else {
        return false;
    }
}

bool isIntVar(SymbolName& name) {
    if (entryTable.curEntryTable->find(name) != entryTable.curEntryTable->end()) {//先找当前程序块中的定义
        return (*entryTable.curEntryTable)[name]->getType() == plantIntVar ||
                (*entryTable.curEntryTable)[name]->getType() == constIntVar;
    } else if (entryTable.entryTable.find(name) != entryTable.entryTable.end()){//如果在当前程序块中没有找到变量的符号表项在去全局变量中找
        return entryTable.entryTable[name]->getType() == constIntVar ||
               entryTable.entryTable[name]->getType() == plantIntVar;
    } else {
        return false;
    }
}

bool isIntArray(SymbolName& name) {
    if (entryTable.curEntryTable->find(name) != entryTable.curEntryTable->end()) {//先找当前程序块中的定义
        return (*entryTable.curEntryTable)[name]->getType() == intArray;
    } else if (entryTable.entryTable.find(name) != entryTable.entryTable.end()){//如果在当前程序块中没有找到变量的符号表项在去全局变量中找
        return entryTable.entryTable[name]->getType() == intArray;
    } else {
        return false;
    }
}

//-----------------------------------函数符号表项的添加函数----------------------------------//
//@private@//
FunctionEntry* addVoidFunc(SymbolName& name,bool isAdd) {//通过名称查询函数是否是无返回值函数
    auto* ansFunc = new FunctionEntry(name, voidFunc, globalName);
    if (isAdd)
        entryTable.entryTable[name] = ansFunc;//符号表中添加无返回值函数表项
    return ansFunc;
}
//@private@//
FunctionEntry* addIntReturnFunc(SymbolName& name,bool isAdd) {//向符号表中添加有返回值的函数
    auto* ansFunc = new FunctionEntry(name, intReturnFunc, globalName);
    if (isAdd)
        entryTable.entryTable[name] = ansFunc;//符号表中添加int类型返回值函数表项
    return ansFunc;
}
//@private@//
FunctionEntry* addCharReturnFunc(SymbolName& name,bool isAdd) {//向符号表中添加无返回值的函数
    auto* ansFunc = new FunctionEntry(name, charReturnFunc, globalName);
    if (isAdd)
        entryTable.entryTable[name] = ansFunc;//符号表中添加char类型返回值函数表项
    return ansFunc;
}

void addFunction(tokenType type,SymbolName& funcName) {
    bool isAdd = true;
    if (globalExist(funcName)) {//存在该名字的符号表项，不需要添加，报名字重定义错误
        error(b,linenum);
        isAdd = false;
    }
    FunctionEntry* functionEntry = nullptr;
    if (type == VOIDTK) {
        functionEntry = addVoidFunc(funcName,isAdd);
    } else if (type == INTTK) {
        functionEntry = addIntReturnFunc(funcName,isAdd);
    } else {// if (type == CHARTK)
        functionEntry = addCharReturnFunc(funcName,isAdd);
    }
    entryTable.curFunction = functionEntry;
    entryTable.curEntryTable = functionEntry->getSymbolTableEntry();
}

void matchFunctionParameter(string& functionName,vector<ExprType>& valueList) {
    if (entryTable.entryTable.find(functionName) != entryTable.entryTable.end()) {
        auto* function = entryTable.entryTable[functionName];
        if (function->getType() != voidFunc && function->getType() != charReturnFunc && function->getType() != intReturnFunc) {
            return;
        }
        auto* func = (FunctionEntry*) function;//强制向下转型
        vector<SymbolTableEntry*> paras = func->getParameters();
        if (paras.size() != valueList.size()) {
            error(d,linenum);
            return;
        }
        for (int i = 0;i < paras.size();i++) {
            if (!((valueList[i] == IntExp && paras[i]->getType() == plantIntVar) ||
                  (valueList[i] == CharExp && paras[i]->getType() == plantCharVar))) {
                error(e,linenum);//参数类型不匹配
                return;
            }
        }
    }
}

//-----------------------------------函数参数符号表项的添加函数----------------------------------//
//@private@//
SymbolTableEntry* createSymbolEntry(tokenType varType, string& varName,bool isConst = false) {
    auto* function = entryTable.curFunction;
    string functionName;
    if (!function) functionName = globalName;
    else functionName = function->getName();
    SymbolType sType = plantIntVar;
    if (varType == INTTK) {
        sType = plantIntVar;
        if (isConst) sType = constIntVar;
    } else if (varType == CHARTK) {
        sType = plantCharVar;
        if (isConst) sType = constCharVar;
    }
    auto* entry = new SymbolTableEntry(varName,sType,functionName);
    return entry;
}

//@private@//
void addEntry(SymbolTableEntry* entry) {
    string name = entry->getName();
    if (partialExist(name)) {//在当前的局部符号表中，已经存在该名称的符号表项
        error(b,linenum);
        return;
    }
    (*entryTable.curEntryTable)[entry->getName()] = entry;
}

void addFunctionParameter(tokenType varType, string& varName) {
    auto* function = entryTable.curFunction;
    auto* entry = createSymbolEntry(varType,varName);
    function->addParameter(entry);
    addEntry(entry);//函数参数也要添加到当前函数的符号表中
}

void addFunctionReturn(ExprType type) {
    SymbolType sType = entryTable.curFunction->getType();
    if (type == None && (sType == intReturnFunc || sType == charReturnFunc)) {
        printf("lack of return statement or exist unmatched return (function name is: %s\n)",entryTable.curFunction->getName().c_str());
        error(h,linenum);//缺少返回语句
    }
    if (type == IntExp && (sType == charReturnFunc)) {
        printf("lack of return statement or exist unmatched return (function name is: %s\n)",entryTable.curFunction->getName().c_str());
        error(h,linenum);//缺少返回语句
    }
    if (type == CharExp && (sType == intReturnFunc)) {
        printf("lack of return statement or exist unmatched return (function name is: %s\n)",entryTable.curFunction->getName().c_str());
        error(h,linenum);//缺少返回语句
    }
    if ((type == IntExp || type == CharExp) && sType == voidFunc) {
        printf("void return function exists unmatched return (function name is: %s\n)",entryTable.curFunction->getName().c_str());
        error(g,linenum);
    }
    entryTable.curFunction->addReturnSatement();
}

void checkFuntionReturn() {//检查返回函数是否有返回语句
    auto* function = entryTable.curFunction;
    if (function->getType() != intReturnFunc && function->getType() != intReturnFunc) return;
    if (!function->haveReturnStatement() && (function->getType() == intReturnFunc || function->getType() == charReturnFunc)) {
        error(h,linenum);//缺少返回语句
    }
}

//-----------------------------------普通变量的添加函数-----------------------------------//
void addVariableEntry(tokenType varType,SymbolName& varName,bool isConst) {
    auto* entry = createSymbolEntry(varType,varName,isConst);
    addEntry(entry);
}

void addArrayEntry(tokenType varType,SymbolName& varName) {
    auto* function = entryTable.curFunction;
    string functionName;
	if (!function) functionName = globalName;
	else functionName = function->getName();
    SymbolType sType = intArray;
    if (varType == INTTK) {
        sType = intArray;
    } else if (varType == CHARTK) {
        sType = charArray;
    }
    auto* entry = new SymbolTableEntry(varName,sType,functionName);
    addEntry(entry);
}