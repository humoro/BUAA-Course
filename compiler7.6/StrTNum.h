#ifndef COMPILER7_6_STRTNUM_H
#define COMPILER7_6_STRTNUM_H
#include <iostream>
#include "Signary.h"

using namespace std;

class strTNum{
    SymbolTable* entryTable;
    static bool isCharConstantString(const string& str) {
        return str.size() == 3 && (str[0] == str[2]) && (str[0] == '\'');//字符型的常量
    }

    static bool isNumberConstantString(string str) {
        if (str[0] == '+' || str[0] == '-') {
            str.erase(str.begin());
        }
        for (auto c : str) {
            if (!isdigit(c))return false;
        }
        return true;
    }
public:
    explicit strTNum(SymbolTable* table):entryTable(table){}

    bool isConstVariable(string &str, string& functionName) {//查看字符串是否代表常量
        return (isCharConstantString(str) ||
                isNumberConstantString(str) ||
                this->entryTable->translateIR_isGlobalConstVar(str) ||
                this->entryTable->translateIR_isPartialConstVar(str, functionName));
    }

    long long stringToNumber(string& str, string& functionName) {
        if (str.empty()) return 0;
        long long value;
        if (str.size() == 3 && str[0] == '\'' && str[2] == str[0]) {
            value = (long long)str[1];
            return value;
        } else if (this->entryTable->translateIR_isPartialConstVar(str, functionName)) { // 先找部分的当前程序块的const
            value = this->entryTable->translateIR_getPartialConstValue(str, functionName);
            return value;
        } else if (this->entryTable->translateIR_isGlobalConstVar(str)) { // 再找部分的当前程序块的const
            value = this->entryTable->translateIR_getGlobalConstValue(str);
            return value;
        } else {
            value = stoi(str);
            return value;
        }
    }
};
#endif //COMPILER7_6_STRTNUM_H
