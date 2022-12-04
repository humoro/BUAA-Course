#ifndef COMPILER3_2_SIGNARY_H
#define COMPILER3_2_SIGNARY_H
//符号表相关
#include<iostream>
using namespace std;
typedef string SymbolName;

enum SymbolType{
    //funcType
    intReturn,
    charReturn,
    voidReturn
};

class SymbolTableEntry{//符号表的基类
protected:
    SymbolName sName;
    SymbolType sType;
public:
    SymbolTableEntry(SymbolName& name,SymbolType type):sName(name),sType(type){}

    SymbolName getName() {
        return this->sName;
    }

    SymbolType getType() {
        return this->sType;
    }
};

class functionEntry : public SymbolTableEntry {
public:
    functionEntry(SymbolName &name, SymbolType type) : SymbolTableEntry(name, type) {}
};
bool isFunction(SymbolName& name);
bool isReturnFunc(SymbolName& name);
void addVoidFunc(SymbolName& name);
void addIntReturnFunc(SymbolName& name);
void addCharReturnFunc(SymbolName& name);
#endif //COMPILER3_2_SIGNARY_H
