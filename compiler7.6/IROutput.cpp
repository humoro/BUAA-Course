#include <sstream>
#include "Mips.h"
string IROutputTransform(IRStatement* stmt) {
    string ans;
    string oriStmt = stmt->getIR();
    #ifdef irdebug
    cout << oriStmt << endl;
    #endif
    switch (stmt->getIRType()) {
        case funcDefLabel:
        {
            oriStmt.pop_back();
            ans = oriStmt + "()";
        }
            break;
        case nonRetFuncCall:
        {
            string functionName;
            stringstream ss(oriStmt);
            getline(ss, functionName, ' ');
            getline(ss, functionName);
            ans = "call " + functionName;
        }
            break;
        case retFuncCall:
        {
            string functionName, retVar;
            stringstream ss(oriStmt);
            getline(ss, functionName, ' ');
            getline(ss, functionName, ' ');
            getline(ss, retVar);
            ans = "call " + functionName + "\n" + retVar + " = RET";
        }
            break;
        case funcDefPara:
        {
            ans = "para ";
            string typeString, symbol;
            stringstream ss(oriStmt);
            getline(ss, typeString, ' ');
            getline(ss, typeString, ' ');
            getline(ss, symbol, ' ');
            ans += typeString + " " + symbol;
        }
            break;
        case funcCallPara:
        {
            ans = "push ";
            string symbol;
            stringstream ss(oriStmt);
            getline(ss, symbol, ' ');
            getline(ss, symbol, ' ');
            ans += symbol;
        }
            break;
        case valueRet:
        {
            stringstream ss(oriStmt);
            string retVar;
            getline(ss, retVar, ' ');
            getline(ss, retVar);
            ans = "ret " + retVar;
        }
            break;
        case voidRet:
            ans = "ret";
            break;
        case statLabel:
            ans = oriStmt;
            break;
        case greJump:
        {
            string condition = " > ", left, right, label;
            stringstream ss(oriStmt);
            getline(ss, left, ' ');
            getline(ss, left, ' ');
            getline(ss, right, ' ');
            getline(ss, label, ' ');
            ans = "BNZ " + left + condition + right + " " + label;
        }
            break;
        case geqJump:
        {
            string condition = " >= ", left, right, label;
            stringstream ss(oriStmt);
            getline(ss, left, ' ');
            getline(ss, left, ' ');
            getline(ss, right, ' ');
            getline(ss, label, ' ');
            ans = "BNZ " + left + condition + right + " " + label;
        }
            break;
        case lesJump:
        {
            string condition = " < ", left, right, label;
            stringstream ss(oriStmt);
            getline(ss, left, ' ');
            getline(ss, left, ' ');
            getline(ss, right, ' ');
            getline(ss, label, ' ');
            ans = "BNZ " + left + condition + right + " " + label;
        }
            break;
        case leqJump:
        {
            string condition = " <= ", left, right, label;
            stringstream ss(oriStmt);
            getline(ss, left, ' ');
            getline(ss, left, ' ');
            getline(ss, right, ' ');
            getline(ss, label, ' ');
            ans = "BNZ " + left + condition + right + " " + label;
        }
            break;
        case eqlJump:
        {
            string condition = " == ", left, right, label;
            stringstream ss(oriStmt);
            getline(ss, left, ' ');
            getline(ss, left, ' ');
            getline(ss, right, ' ');
            getline(ss, label, ' ');
            ans = "BNZ " + left + condition + right + " " + label;
        }
            break;
        case neqJmp:
        {
            string condition = " != ", left, right, label;
            stringstream ss(oriStmt);
            getline(ss, left, ' ');
            getline(ss, left, ' ');
            getline(ss, right, ' ');
            getline(ss, label, ' ');
            ans = "BNZ " + left + condition + right + " " + label;
        }
            break;
        case jump:
        {
            stringstream ss(oriStmt);
            string label;
            getline(ss, label, ' ');
            getline(ss, label, ' ');
            ans = "GOTO " + label;
        }
            break;
        case readInt:
        case readChar:
            ans = oriStmt;
            break;
        case printStr:
        {
            string content;
            stringstream ss(oriStmt);
            int pos = oriStmt.find('\"');
            ans = "@print@string " + oriStmt.substr(pos, oriStmt.size() - pos);
        }
            break;
        case printInt:
        case printChar:
        case printNewLine:
            ans = oriStmt;
            break;
        case operation:
        {
            string opnum1, opnum2, op;
            stringstream ss(oriStmt);
            getline(ss, op, ',');
            getline(ss, opnum1, ',');
            getline(ss, opnum2, ',');
            getline(ss, ans, ',');
            ans += " = ";
            if (op == "[]") {
                ans += opnum2 +"[" + opnum1 + "]";
            } else {
                if (opnum1.empty()) {
                    ans += op + " " + opnum2;
                } else {
                    ans += opnum1 + " " +  op + " " + opnum2;
                }
            }
        }
            break;
        case assign:
            ans = oriStmt;
            break;
        default:
            break;
    }
    return ans;
}

void BasicClassifier::nonOptimalIROutput(IRSyntaxNode *irSyntaxNode) {
    for (auto node : irSyntaxNode->getKids()) {
        nonOptimalIROutput(node);
    }
    vector<IRStatement> stmts = irSyntaxNode->getInterCodes();
    string transform;
    for (auto& stmt : stmts) {
        this->statements.push_back(stmt);
        this->myBeforeFile << stmt.getIR() << endl;
        transform = IROutputTransform(&stmt);
        if (!transform.empty())
            this->beforeFile << transform << endl;
    }
}

void BasicClassifier::divideBasicBlk() {//读取中间代码指令并且划分基本快
    unsigned int codeNum = 0;
    string function = ".global.";
    this->blks.push_back(new BasicBlk(function));
    for (auto& irStat : this->statements) {
        if (irStat.isBPStatement()) { // 是一个断点语句
            if (irStat.getIRType() == funcDefLabel) {
                stringstream ss(irStat.getIR());
                getline(ss, function, ' ');
                getline(ss, function, ' ');
                function.pop_back(); // 函数名切换
                this->blks.push_back(new BasicBlk(function));
                codeNum = 1;
                this->blks.back()->push_statement(new IRStatement(irStat,0));
            } else if (irStat.getIRType() == jump ||
                       irStat.getIRType() == geqJump ||
                       irStat.getIRType() == greJump ||
                       irStat.getIRType() == leqJump ||
                       irStat.getIRType() == lesJump ||
                       irStat.getIRType() == eqlJump ||
                       irStat.getIRType() == neqJmp ||
                       irStat.getIRType() == voidRet ||
                       irStat.getIRType() == valueRet) {
                this->blks.back()->push_statement(new IRStatement(irStat,codeNum));
                this->blks.push_back(new BasicBlk(function));
                codeNum = 0;
            } else {
                this->blks.push_back(new BasicBlk(function));
                codeNum = 1;
                this->blks.back()->push_statement(new IRStatement(irStat,0));
            }
        } else {
            this->blks.back()->push_statement(new IRStatement(irStat,codeNum));
            codeNum += 1;
        }
    }
    #ifdef debug
    for (auto& blk : blks) {
        for (auto& stmt : blk.getInterCode()) {
            cout << (stmt->getCodeNumber()) << " : "<< stmt->getIR() << endl;
        }

        cout << endl;
        cout << "---------"<< endl;
        cout << endl;
    }
    #endif
}

void IROptimal::optimizedIROutput() {
    string transform;
    for (auto& blk : blks) {
        for (unsigned int long long i = 0;i < blk->size();i++) {
            this->myAfterFile << blk->operator[](i)->getIR() << endl;
            transform = IROutputTransform(blk->operator[](i));
            if (!transform.empty())
                this->afterFile << transform << endl;
        }
    }
}
