#include <unordered_map>
#include "Signary.h"
#include "Error.h"
using namespace std;
bool SymbolTable::partialExist(SymbolName& name) {//�ڵ�ǰ�ķ��ű��в�ѯ�Ƿ���ڷ��ű���
    return !(curEntryTable->find(name) == curEntryTable->end());
}

bool SymbolTable::globalExist(SymbolName& name) {//�ڳ����ȫ�ַ��ű��ѯ�Ƿ���ڷ�����
    return (this->entryTable.find(name) != entryTable.end());
}

bool SymbolTable::entryExist(SymbolName& name) {
    return partialExist(name) || globalExist(name);
}

//-----------------------------------�������ű���Ĳ�ѯ����----------------------------------//
bool SymbolTable::isReturnFunc(SymbolName& name) {//ͨ�����Ʋ�ѯ�����Ƿ��Ƿ���ֵ����
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
    if (curEntryTable->find(name) != curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*curEntryTable)[name]->getType() == plantIntVar ||
               (*curEntryTable)[name]->getType() == constIntVar;
    } else if (entryTable.find(name) != entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable[name]->getType() == constIntVar ||
               entryTable[name]->getType() == plantIntVar;
    } else {
        return false;
    }
}

bool SymbolTable::isIntArray(SymbolName& name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*curEntryTable)[name]->getType() == intArray;
    } else if (entryTable.find(name) != entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable[name]->getType() == intArray;
    } else {
        return false;
    }
}

bool SymbolTable::isCharArray(SymbolName &name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*curEntryTable)[name]->getType() == charArray;
    } else if (entryTable.find(name) != entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable[name]->getType() == charArray;
    } else {
        return false;
    }
}

bool SymbolTable::isConstVar(SymbolName& name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*curEntryTable)[name]->getType() == constIntVar ||
               (*curEntryTable)[name]->getType() == constCharVar;
    } else if (entryTable.find(name) != entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
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

//-----------------------------------�������ű������Ӻ���----------------------------------//
FunctionEntry* SymbolTable::addVoidFunc(SymbolName& name,bool isAdd) {//ͨ�����Ʋ�ѯ�����Ƿ����޷���ֵ����
    auto* ansFunc = new FunctionEntry(name, voidFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//���ű�������޷���ֵ��������
    return ansFunc;
}

FunctionEntry* SymbolTable::addIntReturnFunc(SymbolName& name,bool isAdd) {//����ű�������з���ֵ�ĺ���
    auto* ansFunc = new FunctionEntry(name, intReturnFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//���ű������int���ͷ���ֵ��������
    return ansFunc;
}

FunctionEntry* SymbolTable::addCharReturnFunc(SymbolName& name,bool isAdd) {//����ű�������޷���ֵ�ĺ���
    auto* ansFunc = new FunctionEntry(name, charReturnFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//���ű������char���ͷ���ֵ��������
    return ansFunc;
}

bool SymbolTable::addFunction(TokenType type, SymbolName& funcName) { //��Ӳ��ɹ�˵������������
    bool isAdd = true;
    if (globalExist(funcName)) {//���ڸ����ֵķ��ű������Ҫ��ӣ��������ض������
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

//-----------------------------------�����������ű������Ӻ���----------------------------------//
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
    if (partialExist(name)) {//�ڵ�ǰ�ľֲ����ű��У��Ѿ����ڸ����Ƶķ��ű���
        return false;
    }
    (*curEntryTable)[entry->getName()] = entry;
    return true;
}

bool SymbolTable::addFunctionParameter(TokenType varType, string& varName) {
    auto* function = curFunction;
    auto* entry = createSymbolEntry(varType,varName,false);
    function->addParameter(entry);
    return addEntry(entry);//��������ҲҪ��ӵ���ǰ�����ķ��ű���
}

//-----------------------------------��ͨ��������Ӻ���-----------------------------------//
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
    if (curEntryTable->find(name) != curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        ansType = (*curEntryTable)[name]->getType();
    } else if (entryTable.find(name) != entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        ansType = entryTable[name]->getType();
    } else {
        ansType = errType;
    }
    return ansType;
}