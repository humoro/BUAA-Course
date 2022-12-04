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

}

void IROptimal::jumpLabelOptimal() {//消除不必要的跳转语句

}

void IROptimal::commenSubExpr() {

}

void IROptimal::firstRoundCopyDiffuse() {
    for (auto& blk : blks) {
        #ifdef optimaldebug
        cout << "this is optimal" << endl;
        #endif
        firstRoundCopyDiffuseFunction(blk);
    }
}

void IROptimal::secondRoundCopyDiffuse() {
    for (auto& blk : blks) {
        #ifdef optimaldebug
        cout << "this is optimal" << endl;
        #endif
        secondRoundCopyDiffuseFunction(blk);
    }
}

void IROptimal::constantDiffuse() {

}