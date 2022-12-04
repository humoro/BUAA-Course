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
    return !(function->getType() != symbol_voidReturnFunction &&
             function->getType() != symbol_charReturnFunction &&
             function->getType() != symbol_intReturnFunction);
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
    return entryTable[name]->getType() == symbol_voidReturnFunction;
}

bool SymbolTable::syntaxParse_isIntReturnFunc(SymbolName& name) {
    if (!syntaxParse_globalExist(name)) return false;
    return entryTable[name]->getType() == symbol_intReturnFunction;
}

bool SymbolTable::syntaxParse_isCharReturnFunc(SymbolName& name) {
    if (!syntaxParse_globalExist(name)) return false;
    return entryTable[name]->getType() == symbol_charReturnFunction;
}

bool SymbolTable::syntaxParse_isConstVariable(SymbolName& name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*curEntryTable)[name]->getType() == symbol_intConstant ||
               (*curEntryTable)[name]->getType() == symbol_charConstant;
    } else if (entryTable.find(name) != entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable[name]->getType() == symbol_intConstant ||
               entryTable[name]->getType() == symbol_charConstant;
    } else {
        return false;
    }
}

bool SymbolTable::syntaxParse_isIntVariable(SymbolName& name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*curEntryTable)[name]->getType() == symbol_intVariable ||
               (*curEntryTable)[name]->getType() == symbol_intConstant;
    } else if (entryTable.find(name) != entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable[name]->getType() == symbol_intConstant ||
               entryTable[name]->getType() == symbol_intVariable;
    } else {
        return false;
    }
}

bool SymbolTable::syntaxParse_isIntArray(SymbolName& name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*curEntryTable)[name]->getType() == symbol_intArray;
    } else if (entryTable.find(name) != entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable[name]->getType() == symbol_intArray;
    } else {
        return false;
    }
}

bool SymbolTable::syntaxParse_isCharArray(SymbolName &name) {
    if (curEntryTable->find(name) != curEntryTable->end()) {//���ҵ�ǰ������еĶ���
        return (*curEntryTable)[name]->getType() == symbol_charArray;
    } else if (entryTable.find(name) != entryTable.end()){//����ڵ�ǰ�������û���ҵ������ķ��ű�����ȥȫ�ֱ�������
        return entryTable[name]->getType() == symbol_charArray;
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
    SymbolType sType = symbol_intVariable;
    if (varType == token_intToken) {
        sType = symbol_intVariable;
        if (isConst) sType = symbol_intConstant;
    } else if (varType == token_charToken) {
        sType = symbol_charVariable;
        if (isConst) sType = symbol_charConstant;
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
    SymbolType sType = symbol_intArray;
    if (varType == token_intToken) {
        sType = symbol_intArray;
    } else if (varType == token_charToken) {
        sType = symbol_charArray;
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
    auto* ansFunc = new Function(name, symbol_voidReturnFunction, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//���ű�������޷���ֵ��������
    return ansFunc;
}

Function* SymbolTable::syntaxParse_addIntReturnFunc(SymbolName& name, bool isAdd) {//����ű�������з���ֵ�ĺ���
    auto* ansFunc = new Function(name, symbol_intReturnFunction, globalName);
    if (isAdd)
        entryTable[name] = ansFunc;//���ű������int���ͷ���ֵ��������
    return ansFunc;
}

Function* SymbolTable::syntaxParse_addCharReturnFunc(SymbolName& name, bool isAdd) {//����ű�������޷���ֵ�ĺ���
    auto* ansFunc = new Function(name, symbol_charReturnFunction, globalName);
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
    if (type == token_voidToken) {
        functionEntry = syntaxParse_addVoidFunction(funcName, isAdd);
    } else if (type == token_intToken) {
        functionEntry = syntaxParse_addIntReturnFunc(funcName, isAdd);
    } else {// if (type == token_charToken)
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
        ansType = symbol_error;
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

bool SymbolTable::translateIR_isGlobalEntry(SymbolName & varName) {
    return (this->entryTable.find(varName) != entryTable.end());
}

bool SymbolTable::translateIR_isPartialEntry(SymbolName &varName, SymbolName &fatherName) {
    unordered_map<string, SymbolTableEntry *> table;
    if (fatherName != this->globalName) {
        auto *function = ((Function *) this->entryTable[fatherName]);
        table = (*(function->getSymbolTableEntry()));
    } else {
        return false;
    }
    return table.find(varName) != table.end();
}

bool SymbolTable::translateIR_isGlobalVariableRefreshed(SymbolName &varName) {
    if (this->entryTable.find(varName) != this->entryTable.end()) {
        auto* variable = (Variable*) this->entryTable[varName];
        return variable->hasBeenRefreshed();
    }
    return true; //����ѡ��
}

bool SymbolTable::translateIR_isPartialVariableRefreshed(SymbolName &varName, SymbolName& fatherName) {
    if (this->entryTable.find(fatherName) == this->entryTable.end()) return true;
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    if (table.find(varName) == table.end()) return true; //����ѡ��
    auto* variable = (Variable*) table[varName];
    return variable->hasBeenRefreshed();
}

bool SymbolTable::translateIR_isGlobalConstant(SymbolName & varName) {
    if (this->entryTable.find(varName) != this->entryTable.end()) {
        auto* variable = (Variable*) this->entryTable[varName];
        return variable->getType() == symbol_intConstant || variable->getType() == symbol_charConstant;
    }
    return false;
}

bool SymbolTable::translateIR_isPartialConstant(SymbolName & varName, SymbolName & fatherName) {
    if (this->entryTable.find(fatherName) == this->entryTable.end()) return false;
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    if (table.find(varName) == table.end()) return false; //����ѡ��
    auto* variable = (Variable*) table[varName];
    return variable->getType() == symbol_intConstant || variable->getType() == symbol_charConstant;
}


void SymbolTable::translateIR_setGlobalConstantInfo(SymbolName &name, unsigned int addr, long long value) {
    SymbolTableEntry* entry = this->entryTable[name];
    if (entry->getType() == symbol_intConstant || entry->getType() == symbol_charConstant) {
        auto* constVar = (Variable *) entry;
        constVar->allocAddr(addr);
        constVar->setValue(value);
    }
}

void SymbolTable::translateIR_setPartialConstantInfo(SymbolName &varName, SymbolName &functionName, unsigned int addr, long long value) {
    SymbolTableEntry* entry = this->entryTable[functionName];
    auto* function = (Function*)entry;
    SymbolTableEntry* var = (*(function->getSymbolTableEntry()))[varName];
    if (var->getType() == symbol_charConstant || var->getType() == symbol_intConstant) {
        auto* constVar = (Variable*) var;
        constVar->allocAddr(addr);
        constVar->setValue(value);
    }
}

void SymbolTable::tranlasteIR_setGlobalVariableInfo(SymbolName& name, unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[name];
    if (entry->getType() == symbol_intVariable || entry->getType() == symbol_charVariable) {
        auto* var = (Variable *) entry;
        var->allocAddr(addr);
    }
}

void SymbolTable::translateIR_setPartialVariableInfo(SymbolName& varName, SymbolName& functionName, unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[functionName];
    auto* function = (Function*)entry;
    SymbolTableEntry* var = (*(function->getSymbolTableEntry()))[varName];
    if (var->getType() == symbol_charVariable || var->getType() == symbol_intVariable) {
        auto* plantVar = (Variable*) var;
        plantVar->allocAddr(addr);
    }
}

void SymbolTable::translateIR_setGlobalArrayInfo(SymbolName& name, unsigned int size, unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[name];
    if (entry->getType() == symbol_intArray || entry->getType() == symbol_charArray) {
        auto* array = (Array*)entry;
        array->allocAddr(addr);
        array->setSize(size);
    }
}

void SymbolTable::translateIR_setPartialArrayInfo(SymbolName& arrayName, SymbolName& functionName, unsigned int size, unsigned int addr) {
    SymbolTableEntry* entry = this->entryTable[functionName];
    auto* function = (Function*)entry;
    SymbolTableEntry* var = (*(function->getSymbolTableEntry()))[arrayName];
    if (var->getType() == symbol_intArray || var->getType() == symbol_charArray) {
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
unsigned int SymbolTable::translateIR_getPartialVariableAddr(SymbolName &varName, SymbolName &fatherName) {
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

long long SymbolTable::translateIR_getGlobalConstantValue(SymbolName &varName) {
    auto* variable = (Variable*) this->entryTable[varName];
    return variable->getValue();
}

long long SymbolTable::translateIR_getPartialConstantValue(SymbolName &varName, SymbolName &fatherName) {
    auto* function = (Function*) this->entryTable[fatherName];
    unordered_map<string,SymbolTableEntry*> table = (*(function->getSymbolTableEntry()));
    auto* variable = (Variable*) table[varName];
    return variable->getValue();
}

vector<string> SymbolTable::translateIR_getFunctionParameterList(SymbolName & functionName) {
    auto* func = (Function*)entryTable[functionName];
    return func->getParameterNameList();
}

void SymbolTable::translateIR_addInlineFunctionVariable(SymbolName& varName, SymbolName &functionName, const string& tokenType) {
    SymbolType sType = symbol_intVariable;
    if (tokenType == "int") {
        sType = symbol_intVariable;
    } else if (tokenType == "char") {
        sType = symbol_charVariable;
    }
    auto* entry = new Variable(varName,sType,functionName);
    auto* func = (Function*)entryTable[functionName];
    func->addEntry(varName, entry);
}

void SymbolTable::translateIR_addInlineFunctionConstant(SymbolName &varName, long long value, SymbolName& functionName, const string& tokenType) {
    SymbolType sType = symbol_intVariable;
    if (tokenType == "int") {
        sType = symbol_intConstant;
    } else if (tokenType == "char") {
        sType = symbol_charConstant;
    }
    auto* entry = new Variable(varName,sType,functionName);
    entry->setValue(value);
    auto* func = (Function*)entryTable[functionName];
    func->addEntry(varName, entry);
}
void SymbolTable::translateIR_addInlineFunctionArray(SymbolName& varName, SymbolName &functionName, long long size, string tokenType) {
    SymbolType sType = symbol_intVariable;
    if (tokenType == "int") {
        sType = symbol_intArray;
    } else if (tokenType == "char") {
        sType = symbol_charArray;
    }
    auto* entry = new Array(varName,sType,functionName, size);
    auto* func = (Function*)entryTable[functionName];
    func->addEntry(varName, entry);
}

unordered_map<string,string> SymbolTable::translateIR_getStrNameMap() {
    return this->strNameMap;
}