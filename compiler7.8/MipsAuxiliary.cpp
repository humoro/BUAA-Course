#include "Mips.h"
#include <sstream>
bool IRTranslator::isConditionalJumpIR(IRStatement* stmt) {
    return (stmt->getIRType() == ir_greJump ||
            stmt->getIRType() == ir_geqJump ||
            stmt->getIRType() == ir_leqJump ||
            stmt->getIRType() == ir_lesJump ||
            stmt->getIRType() == ir_eqlJump ||
            stmt->getIRType() == ir_neqJmp);
}

bool IRTranslator::isPrintSymbolIR(IRStatement *stmt) {
    return stmt->getIRType() == ir_printInt ||
           stmt->getIRType() == ir_printChar;
}

bool IRTranslator::isReadIR(IRStatement *stmt) {
    return stmt->getIRType() == ir_readInt ||
           stmt->getIRType() == ir_readChar;

}

bool IRTranslator::isReturnIR(IRStatement *stmt) {
    return stmt->getIRType() == ir_valueReturn ||
           stmt->getIRType() == ir_voidReturn;

}

bool IRTranslator::isUnconditionalJump(IRStatement* stmt) {
    return stmt->getIRType() == ir_jump;
}

long long IRTranslator::stoi(string& str) {
    return this->strTranformer->stoi(str, this->curFunctionName);
}

bool IRTranslator::isConstVariable(string &str) {
    return this->strTranformer->isConstant(str, this->curFunctionName);
}

bool IRTranslator::isConstVariable(string& str, string& functionName) {
    return this->strTranformer->isConstant(str, functionName);
}

long long IRTranslator::stoi(string& str, string& functionName) {
    return this->strTranformer->stoi(str, functionName);
}

bool IRTranslator::isInnerVariable(string& variable) {
    return variable.find('@') != string::npos && variable.find('[') == string::npos;
}

bool IRTranslator::isUserDefinitionVariable(string& variable) { // 必须满足：用户定义，变量（不是常量或者常数，不是数组）
    return !variable.empty() && variable.find('[') == string::npos &&
           !isConstVariable(variable) && variable.find('@') == string::npos; // 不是中间变量并且不是数组
}

void IRTranslator::getInstructionOperationItem(IRStatement *stmt, string &info1, string &info2,
                                               string &info3, string &info4) {
    stringstream ss(stmt->getIR());
    switch(stmt->getIRType()) { // 每条中间代码中有效信息最多有4项
        case ir_globalConstantDef:
        case ir_partialConstantDef:
        {
            getline(ss, info1, ' ');
            getline(ss, info1, ' '); // 得到type
            getline(ss, info2, ' '); // 得到标识符
            getline(ss, info3, ' ');
            getline(ss, info3, ' '); // 得到constant数值
        }
            break;
        case ir_funcParameterDef:
        case ir_globalVariableDef:
        case ir_partialVariableDef:
        {
            getline(ss, info1, ' ');
            getline(ss, info1, ' '); // 得到type
            getline(ss, info2, ' '); // 得到标识符
            getline(ss, info3, ' '); // 得到size
            if (info3.empty()) info3 = "1"; // 函数参数定义中默认是size为1
        }
            break;
        case ir_funcCallBegin:
        {
            getline(ss, info1, ' ');
            getline(ss, info1, ' '); // 得到即将开始函数调用所对应的被调用函数名称
        }
            break;
        case ir_funcDefLabel:
        {
            getline(ss, info1, ' '); // 得到函数定义的返回类型
            getline(ss, info2, ' '); // 得到定义函数的名称
            info2.pop_back();// 函数定义的时候多了个冒号
        }
            break;
        case ir_voidRetutnFuncCall:
        {
            getline(ss, info1, ' ');
            getline(ss, info1, ' '); // 得到被调用函数名称
        }
            break;
        case ir_valueReturnFuncCall:
        {
            getline(ss, info1, ' ');
            getline(ss, info1, ' '); // 得到被调用函数名称
            getline(ss, info2, ' '); // 得到存放函数返回值标识符
        }
            break;
        case ir_greJump:
        case ir_geqJump:
        case ir_lesJump:
        case ir_leqJump:
        case ir_eqlJump:
        case ir_neqJmp:
        {
            getline(ss, info1, ' ');
            getline(ss, info1, ' '); // 得到条件判断第一个操作数
            getline(ss, info2, ' '); // 得到条件判断第二个操作数
            getline(ss, info3, ' '); // 得到跳转label
        }
            break;
        case ir_jump:
        {
            getline(ss, info1, ' ');
            getline(ss, info1, ' '); // 得到跳转label
        }
            break;
        case ir_funcCallParaPush:
        case ir_valueReturn:
        case ir_printInt:
        case ir_printChar:
        case ir_readInt:
        case ir_readChar:
        {
            getline(ss, info1, ' ');
            getline(ss, info1, ' '); // 得到读取，打印，函数调用参数传递标识符或者函数返回的标识符
      }
            break;
        case ir_printStr:
        {
            getline(ss, info1, ' ');
            getline(ss, info1, ' '); // 得到打印的符号串名称
        }
            break;
        case ir_operation:
        {
            getline(ss, info1, ','); // 得到操作符
            getline(ss, info2, ','); // 得到第一个操作数
            getline(ss, info3, ','); // 得到第二个操作数
            getline(ss, info4, ','); // 得到结果赋值标识符
        }
            break;
        case ir_assign:
        {
            getline(ss, info1, ' '); // 得到左侧被赋值标识符
            getline(ss, info2, ' ');
            getline(ss, info2, ' '); // 得到右侧赋值标识符
        }
            break;
        default:
            cout << "error in get content of inter instruction" << endl;
            break;
    }
}

