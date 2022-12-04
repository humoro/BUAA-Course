#include "Mips.h"
#include <sstream>
string IROutputTransform(IRStatement* stmt) {
    string ans;
    string oriStmt = stmt->getIR();
    #ifdef irdebug
    cout << oriStmt << endl;
    #endif
    switch (stmt->getIRType()) {
        case ir_funcDefLabel:
        {
            oriStmt.pop_back();
            ans = oriStmt + "()";
        }
            break;
        case ir_voidReturnFuncCall:
        {
            string functionName;
            stringstream ss(oriStmt);
            getline(ss, functionName, ' ');
            getline(ss, functionName);
            ans = "call " + functionName;
        }
            break;
        case ir_valueReturnFuncCall:
        {
            string functionName, retVar;
            stringstream ss(oriStmt);
            getline(ss, functionName, ' ');
            getline(ss, functionName, ' ');
            getline(ss, retVar);
            ans = "call " + functionName + "\n" + retVar + " = RET";
        }
            break;
        case ir_funcParameterDef:
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
        case ir_funcCallParaPush:
        {
            ans = "push ";
            string symbol;
            stringstream ss(oriStmt);
            getline(ss, symbol, ' ');
            getline(ss, symbol, ' ');
            ans += symbol;
        }
            break;
        case ir_valueReturn:
        {
            stringstream ss(oriStmt);
            string retVar;
            getline(ss, retVar, ' ');
            getline(ss, retVar);
            ans = "ret " + retVar;
        }
            break;
        case ir_voidReturn:
            ans = "ret";
            break;
        case ir_stmtLabel:
            ans = oriStmt;
            break;
        case ir_greJump:
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
        case ir_geqJump:
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
        case ir_lesJump:
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
        case ir_leqJump:
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
        case ir_eqlJump:
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
        case ir_neqJmp:
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
        case ir_jump:
        {
            stringstream ss(oriStmt);
            string label;
            getline(ss, label, ' ');
            getline(ss, label, ' ');
            ans = "GOTO " + label;
        }
            break;
        case ir_readInt:
        case ir_readChar:
            ans = oriStmt;
            break;
        case ir_printStr:
        {
            string content;
            stringstream ss(oriStmt);
            int pos = oriStmt.find('\"');
            ans = "@print@string " + oriStmt.substr(pos, oriStmt.size() - pos);
        }
            break;
        case ir_printInt:
        case ir_printChar:
        case ir_printNewLine:
            ans = oriStmt;
            break;
        case ir_operation:
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
        case ir_assign:
            ans = oriStmt;
            break;
        default:
            break;
    }
    return ans;
}


void IRTranslator::BOptimizationIROutput(IRSyntaxNode *irSyntaxNode) { // 优化之前中间代码输出
    for (auto node : irSyntaxNode->getKids()) {
        BOptimizationIROutput(node);
    }
    vector<IRStatement*> stmts = irSyntaxNode->getInterCodes();
    string transform;
    for (auto& stmt : stmts) {
        this->statements.push_back(stmt);
        this->MBF << stmt->getIR() << endl;
        transform = IROutputTransform(stmt);
        if (!transform.empty())
            this->BF << transform << endl;
    }
}

void IRTranslator::AOptimizationIROutput() { // 优化之后中间代码输出
    string transform;
    for (auto& blk : blks) {
        for (unsigned int long long i = 0;i < blk->size();i++) {
            this->MAF << blk->operator[](i)->getIR() << endl;
            transform = IROutputTransform(blk->operator[](i));
            if (!transform.empty())
                this->AF << transform << endl;
        }
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

void IRTranslator::generateMipsCode(string str, mipsType type) {
    this->mipsCodes.push_back(new MipsCode(str,type));
}

