#include <unordered_map>
#include "Signary.h"
#include "Error.h"
using namespace std;
bool SymbolTable::partialExist(SymbolName& name) {//在当前的符号表中查询是否存在符号表项
    return !(curEntryTable->find(name) == curEntryTable->end());
}

bool SymbolTable::globalExist(SymbolName& name) {//在程序的全局符号表查询是否存在符号项
    return (this->entryTable.find(name) != entryTable.end());
}

bool SymbolTable::entryExist(SymbolName& name) {
    return partialExist(name) || globalExist(name);
}

//-----------------------------------函数符号表项的查询函数----------------------------------//
bool SymbolTable::isReturnFunc(SymbolName& name) {//通过名称查询函数是否是返回值函数
    return isIntReturnFunc(name) || isCharReturnFunc(name);
}

bool SymbolTable::isVoidFunc(SymbolName &name) {
    if (!globalExist(name)) return false;
    return entryTable[name]->getType() == voidFunc;
}

bool SymbolTable::isIntReturnFunc(SymbolName& name) {
    if (!globalExist(name)) return false;
    return entryTable[name]->getType() == intReturnFunc;
}

bool SymbolTable::isCharReturnFunc(SymbolName& name) {
    if (!globalExist(name)) return false;
    return entryTable[name]->getType() == charReturnFunc;
}

bool SymbolTable::isIntVar(SymbolName& name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//先找当前程序块中的定义
        return (*curEntryTable)[name]->getType() == plantIntVar ||
               (*curEntryTable)[name]->getType() == constIntVar;
    } else if (entryTable.find(name) != entryTable.end()){//如果在当前程序块中没有找到变量的符号表项在去全局变量中找
        return entryTable[name]->getType() == constIntVar ||
               entryTable[name]->getType() == plantIntVar;
    } else {
        return false;
    }
}

bool SymbolTable::isIntArray(SymbolName& name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//先找当前程序块中的定义
        return (*curEntryTable)[name]->getType() == intArray;
    } else if (entryTable.find(name) != entryTable.end()){//如果在当前程序块中没有找到变量的符号表项在去全局变量中找
        return entryTable[name]->getType() == intArray;
    } else {
        return false;
    }
}

bool SymbolTable::isCharArray(SymbolName &name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//先找当前程序块中的定义
        return (*curEntryTable)[name]->getType() == charArray;
    } else if (entryTable.find(name) != entryTable.end()){//如果在当前程序块中没有找到变量的符号表项在去全局变量中找
        return entryTable[name]->getType() == charArray;
    } else {
        return false;
    }
}

bool SymbolTable::isConstVar(SymbolName& name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//先找当前程序块中的定义
        return (*curEntryTable)[name]->getType() == constIntVar ||
               (*curEntryTable)[name]->getType() == constCharVar;
    } else if (entryTable.find(name) != entryTable.end()){//如果在当前程序块中没有找到变量的符号表项在去全局变量中找
        return entryTable[name]->getType() == constIntVar ||
               entryTable[name]->getType() == constCharVar;
    } else {
        return false;
    }
}

bool SymbolTable::isExistVar(SymbolName& name) {
    return (curEntryTable->find(name) != curEntryTable->end() ||
            entryTable.find(name) != entryTable.end());
}

bool SymbolTable::isExistFunction(SymbolName& functionName) {
    if (!globalExist(functionName)) return false;
    auto* function = entryTable[functionName];
    return !(function->getType() != voidFunc &&
             function->getType() != charReturnFunc &&
             function->getType() != intReturnFunc);
}

//-----------------------------------函数符号表项的添加函数----------------------------------//
FunctionEntry* SymbolTable::addVoidFunc(SymbolName& name,bool isAdd) {//通过名称查询函数是否是无返回值函数
    auto* ansFunc = new FunctionEntry(name, voidFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//符号表中添加无返回值函数表项
    return ansFunc;
}

FunctionEntry* SymbolTable::addIntReturnFunc(SymbolName& name,bool isAdd) {//向符号表中添加有返回值的函数
    auto* ansFunc = new FunctionEntry(name, intReturnFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//符号表中添加int类型返回值函数表项
    return ansFunc;
}

FunctionEntry* SymbolTable::addCharReturnFunc(SymbolName& name,bool isAdd) {//向符号表中添加无返回值的函数
    auto* ansFunc = new FunctionEntry(name, charReturnFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//符号表中添加char类型返回值函数表项
    return ansFunc;
}

bool SymbolTable::addFunction(TokenType type, SymbolName& funcName) { //添加不成功说明存在重命名
    bool isAdd = true;
    if (globalExist(funcName)) {//存在该名字的符号表项，不需要添加，报名字重定义错误
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
    curFunction = functionEntry;
    curEntryTable = functionEntry->getSymbolTableEntry();
    return isAdd;
}

bool  SymbolTable::getFunctionParaList(SymbolName& functionName,vector<SymbolTableEntry*>& ansParas) {
    if (!isExistFunction(functionName)) return false;
    auto* func = (FunctionEntry*)entryTable[functionName];
    ansParas = func->getParameters();
    return true;
}

//-----------------------------------函数参数符号表项的添加函数----------------------------------//
SymbolTableEntry* SymbolTable::createSymbolEntry(TokenType varType, string& varName, bool isConst) {
    auto* function = curFunction;
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

bool SymbolTable::addEntry(SymbolTableEntry* entry) {
    string name = entry->getName();
    if (partialExist(name)) {//在当前的局部符号表中，已经存在该名称的符号表项
        return false;
    }
    (*curEntryTable)[entry->getName()] = entry;
    return true;
}

bool SymbolTable::addFunctionParameter(TokenType varType, string& varName) {
    auto* function = curFunction;
    auto* entry = createSymbolEntry(varType,varName,false);
    function->addParameter(entry);
    return addEntry(entry);//函数参数也要添加到当前函数的符号表中
}

//-----------------------------------普通变量的添加函数-----------------------------------//
bool SymbolTable::addVariableEntry(TokenType varType, SymbolName& varName, bool isConst) {
    auto* entry = createSymbolEntry(varType,varName,isConst);
    return addEntry(entry);
}

bool SymbolTable::addArrayEntry(TokenType varType, SymbolName& varName) {
    auto* function = curFunction;
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
    return addEntry(entry);
}

FunctionEntry* SymbolTable::getCurrentFunction() {
    return this->curFunction;
}

SymbolType SymbolTable::getSymbolType(SymbolName &name) {
    SymbolType ansType;
    if (curEntryTable->find(name) != curEntryTable->end()) {//先找当前程序块中的定义
        ansType = (*curEntryTable)[name]->getType();
    } else if (entryTable.find(name) != entryTable.end()){//如果在当前程序块中没有找到变量的符号表项在去全局变量中找
        ansType = entryTable[name]->getType();
    } else {
        ansType = errType;
    }
    return ansType;
}