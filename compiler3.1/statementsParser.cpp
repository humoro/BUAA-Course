//本文件中主要包含的是语法分析中各种类型的语句的语法分析函数
#include<fstream>
#include <unordered_map>
#include "SyntaxParser.h"
#include"Types.h"
extern unordered_map<string,funcType> symbolTable;
void compoundStatementParser(ifstream& in,ofstream& out) {
    string ans;
    streampos sp = in.tellg();
    myType type = wordAnalysis(in,ans);
    if (type == CONSTTK) {//常量说明
        in.seekg(sp);
        constDeclarationParser(in,out);
        sp = in.tellg();
        type = wordAnalysis(in,ans);
    }
    if (type == INTTK || type == CHARTK) {//变量说明
        in.seekg(sp);
        variableDeclarationParser(in,out);
        sp = in.tellg();
    }
    in.seekg(sp);
    statementColumnParser(in,out);
    printType(out,Non_Cmpd_Stat);
}

void statementParser(ifstream& in,ofstream& out) {
    streampos sp = in.tellg();
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type == RETURNTK) {
        in.seekg(sp);
        returnStatementParser(in,out);
    } else if (type == SCANFTK) {
        in.seekg(sp);
        readStatementParser(in,out);
    } else if (type == PRINTFTK) {
        in.seekg(sp);
        printStatementParser(in,out);
    } else if (type == IFTK) {
        in.seekg(sp);
        conditionStatementParser(in,out);
        goto end;
    } else if (type == WHILETK || type == FORTK || type == DOTK) {
        in.seekg(sp);
        circleStatementParser(in,out);
        goto end;
    }  else if (type == LBRACE) {
        printAns(type,ans,out);
        statementColumnParser(in,out);
        type = wordAnalysis(in,ans);
        if (type != RBRACE) error();
        printAns(type,ans,out);
        goto end;
    } else if (type == IDENFR) {
        string ans2;
        myType type2 = wordAnalysis(in,ans2);
        in.seekg(sp);
        if (type2 == LPARENT) {
            if (symbolTable.find(ans) != symbolTable.end()) {
                if (symbolTable[ans] == returnFunc) {
                    valFuncCallStatementParser(in, out);
                } else {
                    nonFuncCallStatementParser(in, out);
                }
            }
        } else {
            assignStatementParser(in,out);
        }
    } else if (type == SEMICN) {
        in.seekg(sp);
    } else error();

    type = wordAnalysis(in,ans);
    if (type != SEMICN) error();
    printAns(type,ans,out);
    end:
    printType(out,Non_Stat);
}

void statementColumnParser(ifstream& in,ofstream& out) {
    streampos sp = in.tellg();
    string ans;
    myType type = wordAnalysis(in,ans);
    while (type != RBRACE) {
        in.seekg(sp);
        statementParser(in,out);
        sp = in.tellg();
        type = wordAnalysis(in,ans);
    }
    in.seekg(sp);
    printType(out,Non_Stat_Column);
}

void readStatementParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != SCANFTK) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != LPARENT) error();
    printAns(type,ans,out);
    while (type != RPARENT) {
        type = wordAnalysis(in,ans);
        if (type != IDENFR) error();
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != COMMA && type != RPARENT) error();
        printAns(type,ans,out);
    }

    printType(out,Non_Read_Stat);
}

void printStatementParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != PRINTFTK) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != LPARENT) error();
    printAns(type,ans,out);
    if (in.peek() == '\"') {
        stringParser(in,out);
        streampos sp = in.tellg();
        type = wordAnalysis(in,ans);
        if (type == COMMA) {
            printAns(type,ans,out);
            explainationParser(in,out);
        } else {
            in.seekg(sp);
        }
    } else {
        explainationParser(in,out);
    }
    type = wordAnalysis(in,ans);
    if (type != RPARENT) error();
    printAns(type,ans,out);

    printType(out,Non_Wirte_Stat);
}

void returnStatementParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != RETURNTK) error();
    printAns(type,ans,out);
    streampos sp = in.tellg();
    type = wordAnalysis(in,ans);
    if (type == LPARENT) {
        printAns(type,ans,out);
        explainationParser(in,out);
        type = wordAnalysis(in,ans);
        if (type != RPARENT) error();
        printAns(type,ans,out);
    } else {
        in.seekg(sp);
    }

    printType(out,Non_Return_Stat);
}

//赋值语句

void assignStatementParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != IDENFR) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != LBRACK && type != ASSIGN) error();
    printAns(type,ans,out);
    if (type == LBRACK) {
        explainationParser(in,out);
        type = wordAnalysis(in,ans);
        if (type != RBRACK) error();
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != ASSIGN) error();
        printAns(type,ans,out);
    }
    explainationParser(in,out);
    printType(out,Non_Assign);
}
//条件语句

void conditionStatementParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != IFTK) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != LPARENT) error();
    printAns(type,ans,out);
    conditionParser(in,out);
    type = wordAnalysis(in,ans);
    if (type != RPARENT) error();
    printAns(type,ans,out);
    statementParser(in,out);
    streampos sp = in.tellg();
    type = wordAnalysis(in,ans);
    if (type == ELSETK) {
        printAns(type,ans,out);
        statementParser(in,out);
    }
    else in.seekg(sp);
    printType(out,Non_Condition_Stat);
}

void conditionParser(ifstream& in,ofstream& out) {
    explainationParser(in,out);
    string ans;
    streampos sp = in.tellg();
    myType type = wordAnalysis(in,ans);
    if (type == RPARENT) {
        in.seekg(sp);
        goto end;
    }
    if (type != LSS && type != GRE && type != GEQ && type != EQL && type != LEQ && type != NEQ) error();
    printAns(type,ans,out);
    explainationParser(in,out);
    end:
    printType(out,Non_Condition);
}

//循环语句

void circleStatementParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    printAns(type,ans,out);
    if (type == WHILETK) {
        type = wordAnalysis(in,ans);
        if (type != LPARENT) error();
        printAns(type,ans,out);
        conditionParser(in,out);
        type = wordAnalysis(in,ans);
        if (type != RPARENT) error();
        printAns(type,ans,out);
        statementParser(in,out);
    } else if (type == DOTK) {
        statementParser(in,out);
        type = wordAnalysis(in,ans);
        if (type != WHILETK) error();
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != LPARENT) error();
        printAns(type,ans,out);
        conditionParser(in,out);
        type = wordAnalysis(in,ans);
        if (type != RPARENT) error();
        printAns(type,ans,out);
    } else if (type == FORTK) {
        type = wordAnalysis(in,ans);
        if (type != LPARENT) error();
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != IDENFR) error();
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != ASSIGN) error();
        printAns(type,ans,out);
        explainationParser(in,out);
        type = wordAnalysis(in,ans);
        if (type != SEMICN) error();
        printAns(type,ans,out);
        conditionParser(in,out);
        type = wordAnalysis(in,ans);
        if (type != SEMICN) error();
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != IDENFR) error();
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != ASSIGN) error();
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != IDENFR) error();
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != PLUS && type != MINU) error();
        printAns(type,ans,out);
        stepStatementParser(in,out);
        type = wordAnalysis(in,ans);
        if (type != RPARENT) error();
        printAns(type,ans,out);
        statementParser(in,out);
    } else error();
    printType(out,Non_Circle_Stat);
}

void stepStatementParser(ifstream& in,ofstream& out) {
    unsignedIntgerParser(in,out);
    printType(out,Non_Step_Length);
}