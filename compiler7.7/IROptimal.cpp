#include <sstream>
#include "Mips.h"
// 该文件中主要是赋值传播和常量传播的优化
bool isInnerVariable(string& variable) {
    return variable.find('@') != string::npos & variable.find('[') == string::npos;
}

void IROptimal::firstRoundCopyDiffuseFunction(BasicBlk* blk) { // 这是第一轮的赋值传播消除，消除中间变量中的赋值传播，不涉及用户定义变量
    vector<IRStatement*> stmts = blk->getInterCode();
    for (unsigned long long i = 0;i < stmts.size();i++) {
        if (stmts[i]->getIRType() == assign || stmts[i]->getIRType() == retFuncCall || stmts[i]->getIRType() == operation) {
            string left, right, oriLeft, oriRight, oriFunction, oriOpnum1, oriOpnum2, oriOp, printString;
            stringstream ss(stmts[i]->getIR());
            if (stmts[i]->getIRType() == assign) {
                getline(ss, oriLeft, ' ');
                getline(ss, oriRight, ' ');
                getline(ss, oriRight, ' ');
            } else if (stmts[i]->getIRType() == operation) {
                getline(ss, oriOp, ',');
                getline(ss, oriOpnum1, ',');
                getline(ss, oriOpnum2, ',');
                getline(ss, oriLeft);
            } else {
                getline(ss, oriFunction, ' ');
                getline(ss, oriFunction, ' ');
                getline(ss, oriLeft, ' ');
            }
            #ifdef optimaldebug
            cout << oriLeft << endl;
            #endif
            if (isInnerVariable(oriLeft)) { //是中间变量且不是数组
                unsigned long long j = i + 1;
                bool havePrintStr = false;
                while (stmts[j]->getIRType() == assign || stmts[j]->getIRType() == valueRet ||
                       stmts[j]->getIRType() == printInt || stmts[j]->getIRType() == printChar || stmts[j]->getIRType() == printStr) {
                    stringstream s(stmts[j]->getIR());
                    if (stmts[j]->getIRType() == printStr) {
                        printString = stmts[j]->getIR();
                        havePrintStr = true;
                        j++;
                        continue;
                    }
                    if (havePrintStr && !(stmts[j]->getIRType() == printInt || stmts[j]->getIRType() == printChar || stmts[j]->getIRType() == valueRet)) {
                        j--;
                        break;
                    }
                    if (stmts[j]->getIRType() == assign) {
                        getline(s, left, ' ');
                        getline(s, right, ' ');
                        getline(s, right, ' ');
                        #ifdef optimaldebug
                        cout << "left is : " << left  << endl;
                    cout << "right is : " << right << endl;
                        #endif
                        if (!(isInnerVariable(left) && isInnerVariable(right)) || (right != oriLeft)) {
                            break;
                        }
                        oriLeft = left;
                        j++;
                    } else {
                        getline(s, right, ' ');
                        getline(s, right, ' ');
                        if (right == oriLeft) j++;
                        else j--;
                        break;
                    }
                }
                if (j > i + 1) {
                    #ifdef optimaldebug
                    cout << "j is : " << j << endl;
                    #endif
                    j--;//j回退到上一个匹配的语句
                    stringstream s(stmts[j]->getIR());
                    string ans;
                    if (stmts[i]->getIRType() == assign) {
                        if (stmts[j]->getIRType() == assign) {
                            getline(s, left, ' ');
                            left += " = " + oriRight;
                            for (auto k = i;k <= j;k++) {
                                stmts.erase(stmts.begin() + i);
                            }
                            stmts.insert(stmts.begin() + i, new IRStatement(left,assign));
                        } else {
                            IRType type = valueRet;
                            if (stmts[j]->getIRType() == valueRet)
                                ans = "@ret@value " + oriRight;
                            else {
                                if (stmts[j]->getIRType() == printInt) {
                                    ans = "@print@int " + oriRight;
                                    type = printInt;
                                } else {
                                    ans = "@print@char " + oriRight;
                                    type = printChar;
                                }
                            }
                            for (auto k = i;k <= j;k++) {
                                stmts.erase(stmts.begin() + i);
                            }
                            stmts.insert(stmts.begin() + i, new IRStatement(ans,type));
                            if (havePrintStr) {
                                stmts.insert(stmts.begin() + i, new IRStatement(printString, printStr));
                                i++;
                            }
                        }
                    } else if (stmts[i]->getIRType() == operation) {
                        if (stmts[j]->getIRType() == assign) {
                            getline(s, left, ' ');
                            oriOp += "," + oriOpnum1 + "," + oriOpnum2 + "," + left;
                            for (auto k = i;k <= j;k++) {
                                stmts.erase(stmts.begin() + i);
                            }
                            stmts.insert(stmts.begin() + i, new IRStatement(oriOp,operation));
                        }
                    } else {
                        ans = "@call@retFunc " + oriFunction + " ";
                        getline(s, left, ' ');
                        if (stmts[j]->getIRType() == valueRet || stmts[j]->getIRType() == printInt || stmts[j]->getIRType() == printChar) {
                            getline(s, left, ' ');
                            j--;
                        }
                        ans += left;
                        for (auto k = i;k <= j;k++) {
                            stmts.erase(stmts.begin() + i);
                        }
                        stmts.insert(stmts.begin() + i,new IRStatement(ans,retFuncCall));
                        if (havePrintStr) {
                            stmts.insert(stmts.begin() + i + 1,new IRStatement(printString, printStr));
                            i++;
                        }
                    }
                }
            }
        }
    }
    blk->resetIRs(stmts);
}

void IROptimal::secondRoundCopyDiffuseFunction(BasicBlk* blk) { //消除表达式中不必要的中间变量申请
    unsigned long long len = 0;
    vector<IRStatement*> stmts;
    string left, right, decLeft, decRight, decAns, decOp, ir;
    for (unsigned long long i = 0;i < blk->size();i++) {
        IRStatement* stmt = blk->operator[](i);
        if (stmt->getIRType() == assign) {
            stringstream ss(stmt->getIR());
            getline(ss, left, ' ');
            getline(ss, right, ' ');
            getline(ss, right, ' ');
            if (isInnerVariable(left) && right.find('[') == string::npos) {
                len++;
            }
        }
    }
    for (unsigned long long i = 0;i < len;i++) {
        stmts.clear();
        unsigned long long j = 0;
        for (;j < blk->size();j++) {
            auto stmt = blk->operator[](j);
            if (stmt->getIRType() == assign) {
                stringstream ss(stmt->getIR());
                getline(ss, left, ' ');
                getline(ss, right, ' ');
                getline(ss, right, ' ');
                if (isInnerVariable(left) && right.find('[') == string::npos) { // 右侧数值不能是数组
                    j++;
                    break;
                } else {
                    stmts.push_back(stmt);
                }
            } else {
                stmts.push_back(stmt);
            }
        }
        while (j < blk->size()) {
            stringstream ss(blk->operator[](j)->getIR());
            if (blk->operator[](j)->getIRType() == assign) {
                getline(ss, decLeft, ' ');
                getline(ss, decRight, ' ');
                getline(ss, decRight, ' ');
                if (decRight == left) {
                    ir = decLeft + " = " + right;
                } else {
                    if (decLeft.find('[') != string::npos) {
                        stringstream s(decLeft);
                        string array,index;
                        getline(s, array, '[');
                        getline(s, index, ']');
                        if (index == left) {
                            ir = array + "[" + right + "]" + " = " + decRight;
                        } else {
                            ir = decLeft + " = " + decRight;
                        }
                    } else {
                        ir = decLeft + " = " + decRight;
                    }
                }
                stmts.push_back(new IRStatement(ir, assign));
                if (decLeft == left || decLeft == right) {
                    j++;
                    break;
                }
            } else if (blk->operator[](j)->getIRType() == operation) {
                getline(ss, decOp, ',');
                getline(ss, decLeft, ',');
                getline(ss, decRight, ',');
                getline(ss, decAns);
                ir = decOp + ",";
                if (decLeft == left) {
                    ir += right + ",";
                } else {
                    ir += decLeft + ",";
                }
                if (decRight == left) {
                    ir += right + ",";
                } else {
                    ir += decRight + ",";
                }
                ir += decAns;
                stmts.push_back(new IRStatement(ir, operation));
                if (decAns == left || decAns == right) {
                    j++;
                    break;
                }
            } else if(blk->operator[](j)->getIRType() == greJump || blk->operator[](j)->getIRType() == geqJump ||
                      blk->operator[](j)->getIRType() == lesJump || blk->operator[](j)->getIRType() == leqJump ||
                      blk->operator[](j)->getIRType() == eqlJump || blk->operator[](j)->getIRType() == neqJmp) {
                getline(ss, decOp, ' ');
                getline(ss, decLeft, ' ');
                getline(ss, decRight, ' ');
                getline(ss, decAns, ' ');
                ir = decOp + " ";
                if (decLeft == left) {
                    ir += right + " ";
                } else {
                    ir += decLeft + " ";
                }
                if (decRight == left) {
                    ir += right + " ";
                } else {
                    ir += decRight + " ";
                }
                ir += decAns;
                stmts.push_back(new IRStatement(ir, blk->operator[](j)->getIRType()));
            } else if (blk->operator[](j)->getIRType() == printInt || blk->operator[](j)->getIRType() == printChar ||
                       blk->operator[](j)->getIRType() == funcCallPara || blk->operator[](j)->getIRType() == valueRet) {
                getline(ss, decOp, ' ');
                getline(ss, decAns);
                ir = decOp;
                if (decAns == left) {
                    ir += " " + right;
                } else {
                    ir += " " + decAns;
                }
                stmts.push_back(new IRStatement(ir, blk->operator[](j)->getIRType()));
            }   else {
                stmts.push_back(blk->operator[](j));
            }
            j++;
        }
        while (j < blk->size()) {
            stmts.push_back(blk->operator[](j));
            j++;
        }
        blk->resetIRs(stmts);
    }
}

void IROptimal::thirdRoundCopyDiffuseFunction(BasicBlk* blk) { // 除去表达式中最终赋值给左侧变量的时候申请的不必要的中间变量
    vector<IRStatement*> stmts;
    for (unsigned long long i = 0;i < blk->size();i++) {
        IRStatement* stmt = blk->operator[](i);
        if (i < blk->size() - 1 && (stmt->getIRType() == assign || stmt->getIRType() == retFuncCall)) {
            IRStatement* nextStmt = blk->operator[](i + 1);
            if (nextStmt->getIRType() == assign) {
                string left, mid, right, nextLeft, nextRight;
                stringstream ss(stmt->getIR());
                stringstream ns(nextStmt->getIR());
                getline(ss, left, ' ');
                getline(ss, mid, ' ');
                getline(ss, right, ' ');
                getline(ns, nextLeft, ' ');
                getline(ns, nextRight, ' ');
                getline(ns, nextRight, ' ');
                if (nextLeft.find('[') == string::npos) {

                }
                if (stmt->getIRType() == assign && nextRight == left && nextLeft.find('[') == string::npos) {
                    nextLeft += " " + mid + " " + right;
                    stmts.push_back(new IRStatement(nextLeft, stmt->getIRType()));
                    i++;
                } else if (stmt->getIRType() == retFuncCall && nextRight == right && nextLeft.find('[') == string::npos) {
                    left += " " + mid + " " + nextLeft;
                    stmts.push_back(new IRStatement(left, stmt->getIRType()));
                    i++;
                } else {
                    stmts.push_back(stmt);
                }
            } else {
                stmts.push_back(stmt);
            }
        } else if (i < blk->size() - 1 && stmt->getIRType() == operation) {
            IRStatement* nextStmt = blk->operator[](i + 1);
            if (nextStmt->getIRType() == assign) {
                string op, opnum1, opnum2, ans, nextLeft, nextRight;
                stringstream ss(stmt->getIR());
                stringstream ns(nextStmt->getIR());
                getline(ss, op, ',');
                getline(ss, opnum1, ',');
                getline(ss, opnum2, ',');
                getline(ss, ans);
                getline(ns, nextLeft, ' ');
                getline(ns, nextRight, ' ');
                getline(ns, nextRight, ' ');
                if (nextRight == ans && nextLeft.find('[') == string::npos) {
                    op += "," + opnum1 + "," + opnum2 + "," + nextLeft;
                    stmts.push_back(new IRStatement(op, operation));
                    i++;
                } else {
                    stmts.push_back(stmt);
                }
            } else {
                stmts.push_back(stmt);
            }
        } else {
            stmts.push_back(stmt);
        }
    }
    blk->resetIRs(stmts);
}

void IROptimal::forthRoundCopyDiffuseFunction(BasicBlk* blk) {
    string left, right, copyleft, copyright, copyop, copyopnum1, copyopnum2, copyans, copystr;
    for (unsigned long long i = 0;i < blk->size();i++) {
        IRStatement* stmt = blk->operator[](i);
        if (stmt->getIRType() == assign) {
            stringstream ss(stmt->getIR());
            getline(ss, left, ' ');
            getline(ss, right, ' ');
            getline(ss, right, ' ');
            if (left.find('[') == string::npos) { // 左侧的变量保证不能是数组数值
                for (auto j = i + 1;j < blk->size();j++) { // 把所有引用左侧变量的引用处修改为引用右侧变量
                    stringstream ns(blk->operator[](j)->getIR());
                    if (blk->operator[](j)->getIRType() == assign ||
                        blk->operator[](j)->getIRType() == operation ||
                        blk->operator[](j)->getIRType() == readChar ||
                        blk->operator[](j)->getIRType() == readInt ||
                        blk->operator[](j)->getIRType() == retFuncCall) { // 所有可能重定义变量的语句
                        if (blk->operator[](j)->getIRType() == assign) {
                            getline(ns, copyleft, ' ');
                            getline(ns, copyright, ' ');
                            getline(ns, copyright, ' ');
                            if (copyleft == left || copyleft == right) break; // 对左侧的变量重定义或者对右边的变量重定义了
                            else {
                                if (copyright == left) {
                                    copyleft += " = " + right;
                                    auto* statement = new IRStatement(copyleft, assign, j);
                                    blk->resetIR(j, statement);
                                }
                            }
                        } else if (blk->operator[](j)->getIRType() == operation) {
                            getline(ns, copyop, ',');
                            getline(ns, copyopnum1, ',');
                            getline(ns, copyopnum2, ',');
                            getline(ns, copyans, ',');
                            if (copyans == left || copyans == right) break;
                            else {
                                if (copyopnum1 == left) copyopnum1 = right;
                                if (copyopnum2 == left) copyopnum2 = right;
                                copyop += "," + copyopnum1 + "," + copyopnum2 + "," + copyans;
                                auto* statement = new IRStatement(copyop, operation,j);
                                blk->resetIR(j, statement);
                            }
                        } else if (blk->operator[](j)->getIRType() == readChar ||
                                   blk->operator[](j)->getIRType() == readInt) {
                            getline(ns, copyleft, ' ');
                            getline(ns, copyright, ' ');
                            if (copyright == left || copyright == right) break; // 只会产生定义不会引用
                        } else {
                            getline(ns, copyleft, ' ');
                            getline(ns, copyright, ' ');
                            getline(ns, copystr, ' ');
                            if (copystr == left || copyright == right) break; // 只会产生定义不会引用
                        }
                    } else { // 只产生引用不会定义
                        if(blk->operator[](j)->getIRType() == greJump || blk->operator[](j)->getIRType() == geqJump ||
                           blk->operator[](j)->getIRType() == lesJump || blk->operator[](j)->getIRType() == leqJump ||
                           blk->operator[](j)->getIRType() == eqlJump || blk->operator[](j)->getIRType() == neqJmp) {
                            getline(ns, copyop, ' ');
                            getline(ns, copyopnum1, ' ');
                            getline(ns, copyopnum2, ' ');
                            getline(ns, copyans, ' ');
                            if (copyopnum1 == left) copyopnum1 = right;
                            if (copyopnum2 == left) copyopnum2 = right;
                            copyop += " " + copyopnum1 + " " + copyopnum2 + " " + copyans;
                            auto* statement = new IRStatement(copyop, blk->operator[](j)->getIRType(),j);
                            blk->resetIR(j, statement);
                        } else if (blk->operator[](j)->getIRType() == printInt || blk->operator[](j)->getIRType() == printChar ||
                                   blk->operator[](j)->getIRType() == funcCallPara || blk->operator[](j)->getIRType() == valueRet) {
                            getline(ns, copyleft, ' ');
                            getline(ns, copyright);
                            if (copyright == left) copyright = right;
                            copyleft += " " + copyright;
                            auto* statement = new IRStatement(copyleft, blk->operator[](j)->getIRType(),j);
                            blk->resetIR(j, statement);
                        }
                    }
                }
            }
        }
    }
}

void IROptimal::preCalculateFuncrtion(BasicBlk* blk) {
    vector<IRStatement*> stmts;
    string functionName = blk->getFunction();
    string op, opnum1, opnum2, ans;
    for (auto stmt : blk->getInterCode()) {
        if (stmt->getIRType() == operation) {
            stringstream ns(stmt->getIR());
            getline(ns, op, ',');
            getline(ns, opnum1, ',');
            getline(ns, opnum2, ',');
            getline(ns, ans, ',');
            if (isConstVariable(opnum1, functionName) && isConstVariable(opnum2, functionName)) {
                long long num1, num2;
                num1 = stringToNumber(opnum1, functionName);
                num2 = stringToNumber(opnum2, functionName);
                switch (op[0]) {
                    case '+':
                    {
                        num1 += num2;
                        ans += " = " + to_string(num1);
                        auto* statement =  new IRStatement(ans, assign);
                        stmts.push_back(statement);
                    }
                        break;
                    case '-':
                    {
                        num1 -= num2;
                        ans += " = " + to_string(num1);
                        auto* statement =  new IRStatement(ans, assign);
                        stmts.push_back(statement);
                    }
                        break;
                    case '*':
                    {
                        num1 *= num2;
                        ans += " = " + to_string(num1);
                        auto* statement =  new IRStatement(ans, assign);
                        stmts.push_back(statement);
                    }
                        break;
                    case '/':
                    {
                        num1 /= num2;
                        ans += " = " + to_string(num1);
                        auto* statement =  new IRStatement(ans, assign);
                        stmts.push_back(statement);
                    }
                        break;
                    default:
                        stmts.push_back(stmt);
                        break;
                }
            } else {
                stmts.push_back(stmt);
            }
        } else {
            stmts.push_back(stmt);
        }
    }
    blk->resetIRs(stmts);
}

void IROptimal::firstRoundCopyDiffuseOptimization() {
    for (auto& blk : blks) {
        #ifdef optimaldebug
        cout << "this is optimal" << endl;
        #endif
        firstRoundCopyDiffuseFunction(blk);
    }
}

void IROptimal::secondRoundCopyDiffuseOptimization() {
    for (auto& blk : blks) {
        #ifdef optimaldebug
        cout << "this is optimal" << endl;
        #endif
        secondRoundCopyDiffuseFunction(blk);
    }
}

void IROptimal::thirdRoundCopyDiffuseOptimization(){
    for (auto& blk : blks) {
        #ifdef optimaldebug
        cout << "this is optimal" << endl;
        #endif
        thirdRoundCopyDiffuseFunction(blk);
    }
}

void IROptimal::forthRoundCopyDiffuseOptimization() {
    for (auto& blk : blks) {
        #ifdef optimaldebug
        cout << "this is optimal" << endl;
        #endif
        forthRoundCopyDiffuseFunction(blk); // 把新生成的常数传播消除
        preCalculateFuncrtion(blk); // 把可以提前计算的表达式进行计算
        forthRoundCopyDiffuseFunction(blk); // 把新生成的常数传播消除
        secondRoundCopyDiffuseFunction(blk);
    }
}