#include <unordered_map>
#include <cstdio>
#include "Signary.h"
#include "Error.h"
using namespace std;
SymbolTable entryTable;
extern long long linenum;
SymbolName globalName = ".global.";
bool partialExist(SymbolName& name) {//�ڵ�ǰ�ķ��ű��в�ѯ�Ƿ���ڷ��ű���
    return !(entryTable.curEntryTable->find(name) == entryTable.curEntryTable->end());
}

bool globalExist(SymbolName& name) {//�ڳ����ȫ�ַ��ű��ѯ�Ƿ���ڷ�����
    return (entryTable.entryTable.find(name) != entryTable.entryTable.end());
}

bool entryExist(SymbolName& name) {
    return partialExist(name) || globalExist(name);
}

//-----------------------------------�������ű���Ĳ�ѯ����----------------------------------//
bool isReturnFunc(SymbolName& name) {//ͨ�����Ʋ�ѯ�����Ƿ��Ƿ���ֵ����
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

bool isVoidFunc(SymbolName& name) {//ͨ�����ֲ�ѯ�Ƿ���ڶ�Ӧ���Ƶ��޷���ֵ����
	if (!globalExist(name)) return false;
	return entryTable.entryTable[name]->getType() == voidFunc;
}

bool isExistVar(SymbolName& name) {
    return (entryTable.curEntryTable->find(name) != entryTable.curEntryTable->end() ||
            entryTable.entryTable.find(name) != entryTable.entryTable.end());
}

bool isConstVar(SymbolName& name) {
    if (entryTable.curEntryTable->find(name) != entryTable.curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*entryTable.curEntryTable)[name]->getType() == constIntVar ||
                (*entryTable.curEntryTable)[name]->getType() == constCharVar;
    } else if (entryTable.entryTable.find(name) != entryTable.entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable.entryTable[name]->getType() == constIntVar ||
               entryTable.entryTable[name]->getType() == constCharVar;
    } else {
        return false;
    }
}

bool isIntVar(SymbolName& name) {
    if (entryTable.curEntryTable->find(name) != entryTable.curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*entryTable.curEntryTable)[name]->getType() == plantIntVar ||
                (*entryTable.curEntryTable)[name]->getType() == constIntVar;
    } else if (entryTable.entryTable.find(name) != entryTable.entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable.entryTable[name]->getType() == constIntVar ||
               entryTable.entryTable[name]->getType() == plantIntVar;
    } else {
        return false;
    }
}

bool isIntArray(SymbolName& name) {
    if (entryTable.curEntryTable->find(name) != entryTable.curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*entryTable.curEntryTable)[name]->getType() == intArray;
    } else if (entryTable.entryTable.find(name) != entryTable.entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable.entryTable[name]->getType() == intArray;
    } else {
        return false;
    }
}

//-----------------------------------�������ű������Ӻ���----------------------------------//
//@private@//
FunctionEntry* addVoidFunc(SymbolName& name,bool isAdd) {//ͨ�����Ʋ�ѯ�����Ƿ����޷���ֵ����
    auto* ansFunc = new FunctionEntry(name, voidFunc, globalName);
    if (isAdd)
        entryTable.entryTable[name] = ansFunc;//���ű�������޷���ֵ��������
    return ansFunc;
}
//@private@//
FunctionEntry* addIntReturnFunc(SymbolName& name,bool isAdd) {//����ű�������з���ֵ�ĺ���
    auto* ansFunc = new FunctionEntry(name, intReturnFunc, globalName);
    if (isAdd)
        entryTable.entryTable[name] = ansFunc;//���ű������int���ͷ���ֵ��������
    return ansFunc;
}
//@private@//
FunctionEntry* addCharReturnFunc(SymbolName& name,bool isAdd) {//����ű�������޷���ֵ�ĺ���
    auto* ansFunc = new FunctionEntry(name, charReturnFunc, globalName);
    if (isAdd)
        entryTable.entryTable[name] = ansFunc;//���ű������char���ͷ���ֵ��������
    return ansFunc;
}

void addFunction(tokenType type,SymbolName& funcName) {
    bool isAdd = true;
    if (globalExist(funcName)) {//���ڸ����ֵķ��ű������Ҫ��ӣ��������ض������
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
        auto* func = (FunctionEntry*) function;//ǿ������ת��
        vector<SymbolTableEntry*> paras = func->getParameters();
        if (paras.size() != valueList.size()) {
            error(d,linenum);
            return;
        }
        for (int i = 0;i < paras.size();i++) {
            if (!((valueList[i] == IntExp && paras[i]->getType() == plantIntVar) ||
                  (valueList[i] == CharExp && paras[i]->getType() == plantCharVar))) {
                error(e,linenum);//�������Ͳ�ƥ��
                return;
            }
        }
    }
}

//-----------------------------------�����������ű������Ӻ���----------------------------------//
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
    if (partialExist(name)) {//�ڵ�ǰ�ľֲ����ű��У��Ѿ����ڸ����Ƶķ��ű���
        error(b,linenum);
        return;
    }
    (*entryTable.curEntryTable)[entry->getName()] = entry;
}

void addFunctionParameter(tokenType varType, string& varName) {
    auto* function = entryTable.curFunction;
    auto* entry = createSymbolEntry(varType,varName);
    function->addParameter(entry);
    addEntry(entry);//��������ҲҪ��ӵ���ǰ�����ķ��ű���
}

void addFunctionReturn(ExprType type) {
    SymbolType sType = entryTable.curFunction->getType();
    if (type == None && (sType == intReturnFunc || sType == charReturnFunc)) {
        printf("lack of return statement or exist unmatched return (function name is: %s\n)",entryTable.curFunction->getName().c_str());
        error(h,linenum);//ȱ�ٷ������
    }
    if (type == IntExp && (sType == charReturnFunc)) {
        printf("lack of return statement or exist unmatched return (function name is: %s\n)",entryTable.curFunction->getName().c_str());
        error(h,linenum);//ȱ�ٷ������
    }
    if (type == CharExp && (sType == intReturnFunc)) {
        printf("lack of return statement or exist unmatched return (function name is: %s\n)",entryTable.curFunction->getName().c_str());
        error(h,linenum);//ȱ�ٷ������
    }
    if ((type == IntExp || type == CharExp) && sType == voidFunc) {
        printf("void return function exists unmatched return (function name is: %s\n)",entryTable.curFunction->getName().c_str());
        error(g,linenum);
    }
    entryTable.curFunction->addReturnSatement();
}

void checkFuntionReturn() {//��鷵�غ����Ƿ��з������
    auto* function = entryTable.curFunction;
    if (function->getType() != intReturnFunc && function->getType() != intReturnFunc) return;
    if (!function->haveReturnStatement() && (function->getType() == intReturnFunc || function->getType() == charReturnFunc)) {
        error(h,linenum);//ȱ�ٷ������
    }
}

//-----------------------------------��ͨ��������Ӻ���-----------------------------------//
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