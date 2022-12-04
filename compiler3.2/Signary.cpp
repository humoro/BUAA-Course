#include "Signary.h"
#include<unordered_map>
using namespace std;
unordered_map<SymbolName,SymbolTableEntry*> functionTable;
//functionQuery Functions
bool isFunction(SymbolName& name) {
    return functionTable.find(name) != functionTable.end();
}

bool isReturnFunc(SymbolName& name) {
    if (!isFunction(name)) return false;
    return functionTable[name]->getType() == intReturn || functionTable[name]->getType() == charReturn;
}
//functionOperation Functions
void addVoidFunc(SymbolName& name) {
    functionTable[name] = new functionEntry(name,voidReturn);
}

void addIntReturnFunc(SymbolName& name) {
    functionTable[name] = new functionEntry(name,intReturn);
}

void addCharReturnFunc(SymbolName& name) {
    functionTable[name] = new functionEntry(name,charReturn);
}