#include <unordered_map>
#include "Signary.h"
#include "Error.h"
using namespace std;
/**********************************************************************************************************************
 *############################################�﷨��������Ҫ�ķ��ű���##################################################/
 **********************************************************************************************************************/
bool SymbolTable::syntaxParse_globalExist(SymbolName& name) {//�ڳ����ȫ�ַ��ű��ѯ�Ƿ���ڷ�����
    return (this->entryTable.find(name) != entryTable.end());
}

bool SymbolTable::syntaxParse_partialExist(SymbolName& name) {//�ڵ�ǰ�ķ��ű��в�ѯ�Ƿ���ڷ��ű���
    return !(curEntryTable->find(name) == curEntryTable->end());
}

bool SymbolTable::syntaxParse_functionExist(SymbolName& functionName) {
    if (!syntaxParse_globalExist(functionName)) return false;
    auto* function = entryTable[functionName];
    return !(function->getType() != voidFunc &&
             function->getType() != charReturnFunc &&
             function->getType() != intReturnFunc);
}

bool SymbolTable::syntaxParse_entryExist(SymbolName& name) {
    return syntaxParse_partialExist(name) || syntaxParse_globalExist(name);
}

bool SymbolTable::syntaxParse_variableExist(SymbolName& name) {
    return (curEntryTable->find(name) != curEntryTable->end() ||
            entryTable.find(name) != entryTable.end());
}
//-----------------------------------�������ű���Ĳ�ѯ����----------------------------------//

bool SymbolTable::syntaxParse_isReturnFunction(SymbolName& name) {//ͨ�����Ʋ�ѯ�����Ƿ��Ƿ���ֵ����
    return syntaxParse_isIntReturnFunc(name) || syntaxParse_isCharReturnFunc(name);
}

bool SymbolTable::syntaxParse_isVoidFunction(SymbolName &name) {
    if (!syntaxParse_globalExist(name)) return false;
    return entryTable[name]->getType() == voidFunc;
}

bool SymbolTable::syntaxParse_isIntReturnFunc(SymbolName& name) {
    if (!syntaxParse_globalExist(name)) return false;
    return entryTable[name]->getType() == intReturnFunc;
}

bool SymbolTable::syntaxParse_isCharReturnFunc(SymbolName& name) {
    if (!syntaxParse_globalExist(name)) return false;
    return entryTable[name]->getType() == charReturnFunc;
}

bool SymbolTable::syntaxParse_isConstVariable(SymbolName& name) {
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

bool SymbolTable::syntaxParse_isIntVariable(SymbolName& name) {
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

bool SymbolTable::syntaxParse_isIntArray(SymbolName& name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*curEntryTable)[name]->getType() == intArray;
    } else if (entryTable.find(name) != entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable[name]->getType() == intArray;
    } else {
        return false;
    }
}

bool SymbolTable::syntaxParse_isCharArray(SymbolName &name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*curEntryTable)[name]->getType() == charArray;
    } else if (entryTable.find(name) != entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable[name]->getType() == charArray;
    } else {
        return false;
    }
}

//-----------------------------------�������ű������Ӻ���----------------------------------//

Variable * SymbolTable::syntaxParse_createVariableEntry(TokenType varType, string& varName, bool isConst) {
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

bool SymbolTable::syntaxParse_addVariableEntry(TokenType varType, SymbolName& varName, bool isConst, long long value) {
    auto* entry = syntaxParse_createVariableEntry(varType, varName, isConst);
    entry->setValue(value);
    return syntaxParse_addEntry(entry);
}

bool SymbolTable::syntaxParse_addArrayEntry(TokenType varType, SymbolName& varName, unsigned int size) {
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
    return syntaxParse_addEntry(entry);
}


bool SymbolTable::syntaxParse_addEntry(SymbolTableEntry* entry) {
    string name = entry->getName();
    if (syntaxParse_partialExist(name)) {//�ڵ�ǰ�ľֲ����ű��У��Ѿ����ڸ����Ƶķ��ű���
        return false;
    }
    (*curEntryTable)[entry->getName()] = entry;
    return true;
}
Function* SymbolTable::syntaxParse_addVoidFunction(SymbolName& name, bool isAdd) {//ͨ�����Ʋ�ѯ�����Ƿ����޷���ֵ����
    auto* ansFunc = new Function(name, voidFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//���ű�������޷���ֵ��������
    return ansFunc;
}

Function* SymbolTable::syntaxParse_addIntReturnFunc(SymbolName& name, bool isAdd) {//����ű�������з���ֵ�ĺ���
    auto* ansFunc = new Function(name, intReturnFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//���ű������int���ͷ���ֵ��������
    return ansFunc;
}

Function* SymbolTable::syntaxParse_addCharReturnFunc(SymbolName& name, bool isAdd) {//����ű�������޷���ֵ�ĺ���
    auto* ansFunc = new Function(name, charReturnFunc, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//���ű������char���ͷ���ֵ��������
    return ansFunc;
}

bool SymbolTable::syntaxParse_addFunction(TokenType type, SymbolName& funcName) { //��Ӳ��ɹ�˵������������
    bool isAdd = true;
    if (syntaxParse_globalExist(funcName)) {//���ڸ����ֵķ��ű������Ҫ��ӣ��������ض������
        isAdd = false;
    }
    Function* functionEntry = nullptr;
    if (type == VOIDTK) {
        functionEntry = syntaxParse_addVoidFunction(funcName, isAdd);
    } else if (type == INTTK) {
        functionEntry = syntaxParse_addIntReturnFunc(funcName, isAdd);
    } else {// if (type == CHARTK)
        functionEntry = syntaxParse_addCharReturnFunc(funcName, isAdd);
    }
    curFunction = functionEntry;
    curEntryTable = functionEntry->getSymbolTableEntry();
    return isAdd;
}

bool SymbolTable::syntaxParse_addFunctionParameter(TokenType varType, string& varName) {
    auto* function = curFunction;
    auto* entry = syntaxParse_createVariableEntry(varType, varName, false);
    function->addParameter(entry);
    return syntaxParse_addEntry(entry);//��������ҲҪ��ӵ���ǰ�����ķ��ű���
}

SymbolType SymbolTable::syntaxParse_getSymbolType(SymbolName &name) {
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


Function* SymbolTable::syntaxParse_getCurrentFunction() {
    return this->curFunction;
}

bool  SymbolTable::syntaxParse_getFunctionParaList(SymbolName& functionName, vector<SymbolTableEntry*>& ansParas) {
    if (!syntaxParse_functionExist(functionName)) return false;
    auto* func = (Function*)entryTable[functionName];
    ansParas = func->getParameters();
    return true;
}

/**********************************************************************************************************************
 *#########################################�����м�����ʱ����Ҫ�ķ��ű���##############################################/
 **********************************************************************************************************************/

string SymbolTable::generateIR_applyStringName(string& printStr) {
    if (this->strMap.find(printStr) != this->strMap.end()) {
        return this->strMap[printStr];
    } else {
        string name("str" + to_string(this->strNum++));
        this->strNameMap[name] = printStr;
        this->strMap[printStr] = name;
        return name;
    }
}

/**********************************************************************************************************************
 *#########################################�����м�����ʱ����Ҫ�ķ��ű���##############################################/
 **********************************************************************************************************************/
//-----------------------------------���ű����־λ�����ú���----------------------------------//

bool SymbolTable::translateIR_isGlobalVariable(SymbolName & varName) {
    return (this->entryTable.find(varName) != entryTable.end());
}

bool SymbolTable::translateIR_isPartialVariable(SymbolName &varName, SymbolName &fatherName) {
    auto *function = (Function *) this->entryTable[fatherName];
    unordered_map<string, SymbolTableEntry *> table = (*(function->getSymbolTableEntry()));
    return table.find(varName) != table.end();
}

bool SymbolTable::translateIR_isGlobalVarRefreshed(SymbolName &varName) {
    if (this->entryTable.find(varName) != this->entryTable.end()) {
        auto* variable = (Variable*) this->entryTable[varName];
        return variable->hasBeenRefreshed();
    }
    return true; //����ѡ��
}

bool SymbolTable::translateIR_isPartialVarRefreshed(SymbolName &varName, SymbolName& fatherName) {
    if (this->entryTable.find(fatherName) == this->entryTable.end()) return true;
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    if (table.find(varName) == table.end()) return true; //����ѡ��
    auto* variable = (Variable*) table[varName];
    return variable->hasBeenRefreshed();
}

bool SymbolTable::translateIR_isGlobalConstVar(SymbolName & varName) {
    if (this->entryTable.find(varName) != this->entryTable.end()) {
        auto* variable = (Variable*) this->entryTable[varName];
        return variable->getType() == constIntVar || variable->getType() == constCharVar;
    }
    return false;
}

bool SymbolTable::translateIR_isPartialConstVar(SymbolName & varName, SymbolName & fatherName) {
    if (this->entryTable.find(fatherName) == this->entryTable.end()) return false;
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    if (table.find(varName) == table.end()) return false; //����ѡ��
    auto* variable = (Variable*) table[varName];
    return variable->getType() == constIntVar || variable->getType() == constCharVar;
}


void SymbolTable::translateIR_setGlobalConstInfo(SymbolName &name, unsigned int addr, long long value) {
    SymbolTableEntry* entry = this->entryTable[name];
    if (entry->getType() == constIntVar || entry->getType() == constCharVar) {
        auto* constVar = (Variable *) entry;
        constVar->allocAddr(addr);
        constVar->setValue(value);
    }
}

void SymbolTable::translateIR_setPartialConstInfo(SymbolName &varName, SymbolName &functionName, unsigned int addr, long long value) {
    SymbolTableEntry* entry = this->entryTable[functionName];
    auto* function = (Function*)entry;
    SymbolTableEntry* var = (*(function->getSymbolTableEntry()))[varName];
    if (var->getType() == constCharVar || var->getType() == constIntVar) {
        auto* constVar = (Variable*) var;
        constVar->allocAddr(addr);
        constVar->setValue(value);
    }
}

void SymbolTable::tranlasteIR_setGlobalVarAddr(SymbolName& name, unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[name];
    if (entry->getType() == plantIntVar || entry->getType() == plantCharVar) {
        auto* var = (Variable *) entry;
        var->allocAddr(addr);
    }
}

void SymbolTable::translateIR_setPartialVarAddr(SymbolName& varName, SymbolName& functionName, unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[functionName];
    auto* function = (Function*)entry;
    SymbolTableEntry* var = (*(function->getSymbolTableEntry()))[varName];
    if (var->getType() == plantCharVar || var->getType() == plantIntVar) {
        auto* plantVar = (Variable*) var;
        plantVar->allocAddr(addr);
    }
}

void SymbolTable::translateIR_setGlobalArrayInfo(SymbolName& name, unsigned int size, unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[name];
    if (entry->getType() == intArray || entry->getType() == charArray) {
        auto* array = (Array*)entry;
        array->allocAddr(addr);
        array->setSize(size);
    }
}

void SymbolTable::translateIR_setPartialArrayInfo(SymbolName& arrayName, SymbolName& functionName, unsigned int size, unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[functionName];
    auto* function = (Function*)entry;
    SymbolTableEntry* var = (*(function->getSymbolTableEntry()))[arrayName];
    if (var->getType() == intArray || var->getType() == charArray) {
        auto* array = (Array*) var;
        array->allocAddr(addr);
        array->setSize(size);
    }
}

void SymbolTable::translateIR_setGlobalVariableRefreshed(SymbolName &varName) {
    if (this->entryTable.find(varName) != this->entryTable.end()) {
        auto* variable = (Variable*) this->entryTable[varName];
        variable->setRefreshed();
    }
}

void SymbolTable::translateIR_setPartialVariableRefreshed(SymbolName &varName, SymbolName &fatherName) {
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    if (table.find(varName) != table.end()) {
        auto* variable = (Variable*) table[varName];
        variable->setRefreshed();
    }
}

void SymbolTable::translateIR_setGlobalVariableLoaded(SymbolName & varName) {
    if (this->entryTable.find(varName) != this->entryTable.end()) {
        auto* variable = (Variable*) this->entryTable[varName];
        variable->setLoaded();
    }
}

void SymbolTable::translateIR_setPartialVariableLoaded(SymbolName & varName, SymbolName & fatherName) {
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    if (table.find(varName) != table.end()) {
        auto* variable = (Variable*) table[varName];
        variable->setLoaded();
    }
}

//-----------------------------------���ű���������ȡ����----------------------------------//
unsigned int SymbolTable::translateIR_getPartialVarAddr(SymbolName &varName, SymbolName &fatherName) {
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    auto* var = (Variable*) table[varName];
    return var->getAddr();
}

unsigned int SymbolTable::translateIR_getPartialArrayAddr(SymbolName& varName, SymbolName& fatherName) {
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    auto* var = (Array*) table[varName];
    return var->getAddr();
}

long long SymbolTable::translateIR_getGlobalConstValue(SymbolName &varName) {
    auto* variable = (Variable*) this->entryTable[varName];
    return variable->getValue();
}

long long SymbolTable::translateIR_getPartialConstValue(SymbolName &varName, SymbolName &fatherName) {
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    auto* variable = (Variable*) table[varName];
    return variable->getValue();
}

unordered_map<string,string> SymbolTable::translateIR_getStrNameMap() {
    return this->strNameMap;
}