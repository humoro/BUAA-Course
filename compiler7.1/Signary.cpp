#include <unordered_map>
#include "Signary.h"
#include "Error.h"
using namespace std;
bool SymbolTable::globalExist(SymbolName& name) {//在程序的全局符号表查询是否存在符号项
    return (this->entryTable.find(name) != entryTable.end());
}

bool SymbolTable::partialExist(SymbolName& name) {//在当前的符号表中查询是否存在符号表项
    return !(curEntryTable->find(name) == curEntryTable->end());
}


bool SymbolTable::entryExist(SymbolName& name) {
    return partialExist(name) || globalExist(name);
}

bool SymbolTable::variableExist(SymbolName& name) {
    return (curEntryTable->find(name) != curEntryTable->end() ||
            entryTable.find(name) != entryTable.end());
}

bool SymbolTable::functionExist(SymbolName& functionName) {
    if (!globalExist(functionName)) return false;
    auto* function = entryTable[functionName];
    return !(function->getType() != voidFunc &&
             function->getType() != charReturnFunc &&
             function->getType() != intReturnFunc);
}
//-----------------------------------函数符号表项的查询函数----------------------------------//

bool SymbolTable::isPartialVarRefreshed(SymbolName &varName, SymbolName& fatherName) {
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    if (table.find(varName) != table.end()) return true; //保守选择
    auto* variable = (Variable*) table[varName];
    return variable->hasBeenRefreshed();
}

bool SymbolTable::isGlobalVarRefreshed(SymbolName &varName) {
    if (this->entryTable.find(varName) != this->entryTable.end()) {
        auto* variable = (Variable*) this->entryTable[varName];
        return variable->hasBeenRefreshed();
    }
    return true;
}

bool SymbolTable::isPartialVariable(SymbolName &varName, SymbolName &fatherName) {
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    return table.find(varName) != table.end();
}

bool SymbolTable::isReturnFunction(SymbolName& name) {//通过名称查询函数是否是返回值函数
    return isIntReturnFunc(name) || isCharReturnFunc(name);
}

bool SymbolTable::isVoidFunction(SymbolName &name) {
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

bool SymbolTable::isConstVariable(SymbolName& name) {
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

bool SymbolTable::isIntVariable(SymbolName& name) {
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

//-----------------------------------函数符号表项的添加函数----------------------------------//
Function* SymbolTable::addVoidFunction(SymbolName& name, bool isAdd) {//通过名称查询函数是否是无返回值函数
    auto* ansFunc = new Function(name, voidFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//符号表中添加无返回值函数表项
    return ansFunc;
}

Function* SymbolTable::addIntReturnFunc(SymbolName& name, bool isAdd) {//向符号表中添加有返回值的函数
    auto* ansFunc = new Function(name, intReturnFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//符号表中添加int类型返回值函数表项
    return ansFunc;
}

Function* SymbolTable::addCharReturnFunc(SymbolName& name, bool isAdd) {//向符号表中添加无返回值的函数
    auto* ansFunc = new Function(name, charReturnFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//符号表中添加char类型返回值函数表项
    return ansFunc;
}

bool SymbolTable::addFunction(TokenType type, SymbolName& funcName) { //添加不成功说明存在重命名
    bool isAdd = true;
    if (globalExist(funcName)) {//存在该名字的符号表项，不需要添加，报名字重定义错误
        isAdd = false;
    }
    Function* functionEntry = nullptr;
    if (type == VOIDTK) {
        functionEntry = addVoidFunction(funcName, isAdd);
    } else if (type == INTTK) {
        functionEntry = addIntReturnFunc(funcName,isAdd);
    } else {// if (type == CHARTK)
        functionEntry = addCharReturnFunc(funcName,isAdd);
    }
    curFunction = functionEntry;
    curEntryTable = functionEntry->getSymbolTableEntry();
    return isAdd;
}

SymbolTableEntry* SymbolTable::createVariableEntry(TokenType varType, string& varName, bool isConst) {
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
    auto* entry = new Variable(varName,sType,functionName);
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
    auto* entry = createVariableEntry(varType, varName, false);
    function->addParameter(entry);
    return addEntry(entry);//函数参数也要添加到当前函数的符号表中
}

bool SymbolTable::addVariableEntry(TokenType varType, SymbolName& varName, bool isConst) {
    auto* entry = createVariableEntry(varType, varName, isConst);
    return addEntry(entry);
}

bool SymbolTable::addArrayEntry(TokenType varType, SymbolName& varName,unsigned int size) {
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
    auto* entry = new Array(varName,sType,functionName,size);
    return addEntry(entry);
}

//-----------------------------------符号表项标志位的设置函数----------------------------------//
void SymbolTable::setGlobalConstAddr(SymbolName& name,unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[name];
    if (entry->getType() == constIntVar || entry->getType() == constCharVar) {
        auto* constVar = (Variable *) entry;
        constVar->allocAddr(addr);
    }
}

void SymbolTable::setGlobalVarAddr(SymbolName& name,unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[name];
    if (entry->getType() == plantIntVar || entry->getType() == plantCharVar) {
        auto* var = (Variable *) entry;
        var->allocAddr(addr);
    }
}

void SymbolTable::setGlobalArrayInfo(SymbolName& name,unsigned int size,unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[name];
    if (entry->getType() == intArray || entry->getType() == charArray) {
        auto* array = (Array*)entry;
        array->allocAddr(addr);
        array->setSize(size);
    }
}

void SymbolTable::setPartialConstAddr(SymbolName& varName,SymbolName& functionName,unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[functionName];
    auto* function = (Function*)entry;
    SymbolTableEntry* var = (*(function->getSymbolTableEntry()))[varName];
    if (var->getType() == constCharVar || var->getType() == constIntVar) {
        auto* constVar = (Variable*) var;
        constVar->allocAddr(addr);
    }
}

void SymbolTable::setPartialVarAddr(SymbolName& varName,SymbolName& functionName,unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[functionName];
    auto* function = (Function*)entry;
    SymbolTableEntry* var = (*(function->getSymbolTableEntry()))[varName];
    if (var->getType() == plantCharVar || var->getType() == plantIntVar) {
        auto* plantVar = (Variable*) var;
        plantVar->allocAddr(addr);
    }
}

void SymbolTable::setPartialArrayInfo(SymbolName& arrayName, SymbolName& functionName, unsigned int size, unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[functionName];
    auto* function = (Function*)entry;
    SymbolTableEntry* var = (*(function->getSymbolTableEntry()))[arrayName];
    if (var->getType() == intArray || var->getType() == charArray) {
        auto* array = (Array*) var;
        array->allocAddr(addr);
        array->setSize(size);
    }
}

void SymbolTable::setPartialVariableRefreshed(SymbolName &varName, SymbolName &fatherName) {
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    if (table.find(varName) != table.end()) {
        auto* variable = (Variable*) table[varName];
        variable->setRefreshed();
    }
}

void SymbolTable::setGlobalVariableRefreshed(SymbolName &varName) {
    if (this->entryTable.find(varName) != this->entryTable.end()) {
        auto* variable = (Variable*) this->entryTable[varName];
        variable->setRefreshed();
    }
}

//-----------------------------------符号表项内容提取函数----------------------------------//
unsigned int SymbolTable::getPartialVarAddr(SymbolName &varName, SymbolName &fatherName) {
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    auto* var = (Variable*) table[varName];
    return var->getAddr();
}

unsigned int SymbolTable::getPartialArrayAddr(SymbolName& arrayName, SymbolName& fatherName) {
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    auto* array = (Array*) table[arrayName];
    return array->getAddr();
}

bool  SymbolTable::getFunctionParaList(SymbolName& functionName,vector<SymbolTableEntry*>& ansParas) {
    if (!functionExist(functionName)) return false;
    auto* func = (Function*)entryTable[functionName];
    ansParas = func->getParameters();
    return true;
}

Function* SymbolTable::getCurrentFunction() {
    return this->curFunction;
}

unordered_map<string,string> SymbolTable::getStrNameMap() {
    return this->strNameMap;
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

string SymbolTable::applyStringName(string& printStr) {
    if (this->strMap.find(printStr) != this->strMap.end()) {
        return this->strMap[printStr];
    } else {
        string name("str" + to_string(this->strNum++));
        this->strNameMap[name] = printStr;
        this->strMap[printStr] = name;
        return name;
    }
}