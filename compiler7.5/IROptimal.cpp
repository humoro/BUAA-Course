#include <sstream>
#include "Mips.h"
bool isInnerVariable(string& variable) {
    return variable.find('@') != string::npos & variable.find('[') == string::npos;
}

void firstRoundCopyDiffuseFunction(BasicBlk& blk) { // 这是第一轮的赋值传播消除，消除中间变量中的赋值传播，不涉及用户定义变量
    vector<IRStatement> stmts = blk.getInterCode();
    for (unsigned long long i = 0;i < stmts.size();i++) {
        if (stmts[i].getIRType() == assign || stmts[i].getIRType() == retFuncCall || stmts[i].getIRType() == operation) {
            string left, right, oriLeft, oriRight, oriFunction, oriOpnum1, oriOpnum2, oriOp, printString;
            stringstream ss(stmts[i].getIR());
            if (stmts[i].getIRType() == assign) {
                getline(ss, oriLeft, ' ');
                getline(ss, oriRight, ' ');
                getline(ss, oriRight, ' ');
            } else if (stmts[i].getIRType() == operation) {
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
                while (stmts[j].getIRType() == assign || stmts[j].getIRType() == valueRet ||
                       stmts[j].getIRType() == printInt || stmts[j].getIRType() == printChar || stmts[j].getIRType() == printStr) {
                    stringstream s(stmts[j].getIR());
                    if (stmts[j].getIRType() == printStr) {
                        printString = stmts[j].getIR();
                        havePrintStr = true;
                        j++;
                        continue;
                    }
                    if (havePrintStr && !(stmts[j].getIRType() == printInt || stmts[j].getIRType() == printChar || stmts[j].getIRType() == valueRet)) {
                        j--;
                        break;
                    }
                    if (stmts[j].getIRType() == assign) {
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
                    stringstream s(stmts[j].getIR());
                    string ans;
                    if (stmts[i].getIRType() == assign) {
                        if (stmts[j].getIRType() == assign) {
                            getline(s, left, ' ');
                            left += " = " + oriRight;
                            for (auto k = i;k <= j;k++) {
                                stmts.erase(stmts.begin() + i);
                            }
                            stmts.insert(stmts.begin() + i, IRStatement(left,assign));
                        } else {
                            IRType type = valueRet;
                            if (stmts[j].getIRType() == valueRet)
                                ans = "@ret@value " + oriRight;
                            else {
                                if (stmts[j].getIRType() == printInt) {
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
                            stmts.insert(stmts.begin() + i, IRStatement(ans,type));
                            if (havePrintStr) {
                                stmts.insert(stmts.begin() + i, IRStatement(printString, printStr));
                                i++;
                            }
                        }
                    } else if (stmts[i].getIRType() == operation) {
                        if (stmts[j].getIRType() == assign) {
                            getline(s, left, ' ');
                            oriOp += "," + oriOpnum1 + "," + oriOpnum2 + "," + left;
                            for (auto k = i;k <= j;k++) {
                                stmts.erase(stmts.begin() + i);
                            }
                            stmts.insert(stmts.begin() + i, IRStatement(oriOp,operation));
                        }
                    } else {
                        ans = "@call@retFunc " + oriFunction + " ";
                        getline(s, left, ' ');
                        if (stmts[j].getIRType() == valueRet || stmts[j].getIRType() == printInt || stmts[j].getIRType() == printChar) {
                            getline(s, left, ' ');
                            j--;
                        }
                        ans += left;
                        for (auto k = i;k <= j;k++) {
                            stmts.erase(stmts.begin() + i);
                        }
                        stmts.insert(stmts.begin() + i, IRStatement(ans,retFuncCall));
                        if (havePrintStr) {
                            stmts.insert(stmts.begin() + i + 1, IRStatement(printString, printStr));
                            i++;
                        }
                    }
                }
            }
        }
    }
    blk.resetIRs(stmts);
}

void secondRoundCopyDiffuseFunction(BasicBlk& blk) {
    unsigned long long len = 0;
    vector<IRStatement> stmts;
    string left, right, decLeft, decRight, decAns, decOp, ir;
    for (unsigned long long i = 0;i < blk.size();i++) {
        IRStatement stmt = blk[i];
        if (stmt.getIRType() == assign) {
            stringstream ss(stmt.getIR());
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
        for (;j < blk.size();j++) {
            auto stmt = blk[j];
            if (stmt.getIRType() == assign) {
                stringstream ss(stmt.getIR());
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
        while (j < blk.size()) {
            stringstream ss(blk[j].getIR());
            if (blk[j].getIRType() == assign) {
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
                stmts.emplace_back(ir, assign);
                if (decLeft == left || decLeft == right) {
                    j++;
                    break;
                }
            } else if (blk[j].getIRType() == operation) {
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
                stmts.emplace_back(ir, operation);
                if (decAns == left || decAns == right) {
                    j++;
                    break;
                }
            } else if(blk[j].getIRType() == greJump || blk[j].getIRType() == geqJump ||
                      blk[j].getIRType() == lesJump || blk[j].getIRType() == leqJump ||
                      blk[j].getIRType() == eqlJump || blk[j].getIRType() == neqJmp) {
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
                stmts.emplace_back(ir, blk[j].getIRType());
            } else if (blk[j].getIRType() == printInt || blk[j].getIRType() == printChar ||
                       blk[j].getIRType() == funcCallPara || blk[j].getIRType() == valueRet) {
                getline(ss, decOp, ' ');
                getline(ss, decAns);
                ir = decOp;
                if (decAns == left) {
                    ir += " " + right;
                } else {
                    ir += " " + decAns;
                }
                stmts.emplace_back(ir, blk[j].getIRType());
            }   else {
                stmts.push_back(blk[j]);
            }
            j++;
        }
        while (j < blk.size()) {
            stmts.push_back(blk[j]);
            j++;
        }
        blk.resetIRs(stmts);
    }
}

void thirdRoundCopyDiffuseFunction(BasicBlk& blk) { // 出去表达式中最终的不必要的赋值语句
    vector<IRStatement> stmts;
    for (unsigned long long i = 0;i < blk.size();i++) {
        IRStatement stmt = blk[i];
        if (i < blk.size() - 1 && (stmt.getIRType() == assign || stmt.getIRType() == retFuncCall)) {
            IRStatement nextStmt = blk[i + 1];
            if (nextStmt.getIRType() == assign) {
                string left, mid, right, nextLeft, nextRight;
                stringstream ss(stmt.getIR());
                stringstream ns(nextStmt.getIR());
                getline(ss, left, ' ');
                getline(ss, mid, ' ');
                getline(ss, right, ' ');
                getline(ns, nextLeft, ' ');
                getline(ns, nextRight, ' ');
                getline(ns, nextRight, ' ');
                if (nextLeft.find('[') == string::npos) {

                }
                if (stmt.getIRType() == assign && nextRight == left && nextLeft.find('[') == string::npos) {
                    nextLeft += " " + mid + " " + right;
                    stmts.emplace_back(nextLeft, stmt.getIRType());
                    i++;
                } else if (stmt.getIRType() == retFuncCall && nextRight == right && nextLeft.find('[') == string::npos) {
                    left += " " + mid + " " + nextLeft;
                    stmts.emplace_back(left, stmt.getIRType());
                    i++;
                } else {
                    stmts.push_back(stmt);
                }
            } else {
                stmts.push_back(stmt);
            }
        } else if (i < blk.size() - 1 && stmt.getIRType() == operation) {
            IRStatement nextStmt = blk[i + 1];
            if (nextStmt.getIRType() == assign) {
                string op, opnum1, opnum2, ans, nextLeft, nextRight;
                stringstream ss(stmt.getIR());
                stringstream ns(nextStmt.getIR());
                getline(ss, op, ',');
                getline(ss, opnum1, ',');
                getline(ss, opnum2, ',');
                getline(ss, ans);
                getline(ns, nextLeft, ' ');
                getline(ns, nextRight, ' ');
                getline(ns, nextRight, ' ');
                if (nextRight == ans && nextLeft.find('[') == string::npos) {
                    op += "," + opnum1 + "," + opnum2 + "," + nextLeft;
                    stmts.emplace_back(op, operation);
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
    blk.resetIRs(stmts);
}

void DAGFunction(BasicBlk& blk) {

}

void IROptimal::jumpLabelOptimization() {//消除不必要的跳转语句

}

void IROptimal::firstRoundCopyDiffuseOptimization() {
    for (auto& blk : blks) {
        #ifdef optimaldebug
        cout << "this is optimal" << endl;
        #endif
        firstRoundCopyDiffuseFunction(blk);
    }
}

void IROptimal::commenSubExprOptimization() {
    for (auto blk : blks) {
        DAGFunction(blk);
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
};

void IROptimal::peepholeOptimization() {

}