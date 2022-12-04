#include<fstream>
#include<cstdio>
#include<unordered_map>
#include "SyntaxParser.h"
#include "Types.h"
using namespace std;
unordered_map<string,funcType> symbolTable;
void SyntaxParser(ifstream& in,ofstream& out) {
    programParser(in,out);
}

void characterParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != CHARCON) error();
    char ch = ans[0];
    if (!isPlus(ch) && !isMinus(ch) && !isMult(ch) && !isDivid(ch) && !isAlpha(ch) && !isULine(ch) && !isDigit(ch)) error();
    printAns(type,ans,out);
}

void stringParser(ifstream& in,ofstream& out) {
    string ans;
    myType  type = wordAnalysis(in,ans);
    if (type != STRCON) error();
    printAns(type,ans,out);
    printType(out,Non_Str);
}


void constDeclarationParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    streampos sp = in.tellg();
    while (type == CONSTTK) {
        printAns(type,ans,out);
        constDefinitionParser(in,out);
        type = wordAnalysis(in,ans);
        if (type != SEMICN) error();
        printAns(type,ans,out);
        sp = in.tellg();
        type = wordAnalysis(in,ans);
    }
    printType(out,Non_Const_Decl);
    in.seekg(sp);
}

void constDefinitionParser(ifstream& in,ofstream& out) {
    string ans;
    streampos sp = in.tellg();
    myType type = wordAnalysis(in,ans);
    myType oritype;
    oritype = type;
    while (type != SEMICN) {
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != IDENFR) error();
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != ASSIGN) error();
        printAns(type,ans,out);
        if (oritype == INTTK)
            integerParser(in,out);
        else  if (oritype == CHARTK)
            characterParser(in,out);
        else error();
        sp = in.tellg();
        type = wordAnalysis(in,ans);
        if (type != COMMA && type != SEMICN) error();
    }
    printType(out,Non_Const_Def);
    in.seekg(sp);
}

void  variableDeclarationParser(ifstream& in,ofstream& out) {
    string ans;
    streampos sp = in.tellg();
    myType type = wordAnalysis(in,ans);
    while (type == INTTK || type == CHARTK) {
        wordAnalysis(in,ans);
        type = wordAnalysis(in,ans);
        if (type == LPARENT) {
            break;
        } else {
            in.seekg(sp);
            variableDefinitionParser(in,out);
            type = wordAnalysis(in,ans);
            if (type != SEMICN) error();
            printAns(type,ans,out);
            sp = in.tellg();
        }
        type = wordAnalysis(in,ans);
    }
    printType(out,Non_Var_Decl);
    in.seekg(sp);
}

void variableDefinitionParser(ifstream& in,ofstream& out) {
    string ans;
    myType type;
    streampos sp;
    type = wordAnalysis(in,ans);//提取类型标识符
    while (type != SEMICN) {
        printAns(type,ans,out);
        type = wordAnalysis(in,ans);
        if (type != IDENFR) error();
        printAns(type,ans,out);
        sp = in.tellg();
        type = wordAnalysis(in,ans);
        if (type == COMMA) {
            continue;
        }
        if (type == LBRACK) {//左中括号
            printAns(type,ans,out);
            unsignedIntgerParser(in,out);
            type = wordAnalysis(in,ans);
            if (type == RBRACK) {
                printAns(type,ans,out);
            } else error();
            sp = in.tellg();
            type = wordAnalysis(in,ans);
        }
    }
    printType(out,Non_Var_Def);
    in.seekg(sp);
}


void unsignedIntgerParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != INTCON) error();
    if (ans.size() > 1 && ans[0] == '0') error();
    printAns(type,ans,out);
    printType(out,Non_Unsigned_int);
}

void integerParser(ifstream& in,ofstream& out) {
    string ans;
    streampos sp = in.tellg();
    myType type = wordAnalysis(in,ans);
    if (type == PLUS || type == MINU) {
        printAns(type,ans,out);
        unsignedIntgerParser(in,out);
    } else {
        in.seekg(sp);
        unsignedIntgerParser(in,out);
    }
    printType(out,Non_Integer);
}

void declarationHeadParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != INTTK && type != CHARTK) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != IDENFR) error();
    printAns(type,ans,out);
    symbolTable[ans] = returnFunc;//该函数记录为有返回值函数
    printType(out,Non_Expl_Head);
}

void valReturnFuncDefinitionParser(ifstream& in,ofstream& out) {
    declarationHeadParser(in,out);
    funcStatementParser(in,out);
    printType(out,Non_Return_Func);
}

void nonReturnFuncDefinitionParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != VOIDTK) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != IDENFR) error();
    printAns(type,ans,out);
    symbolTable[ans] = voidFunc;
    funcStatementParser(in,out);
    printType(out,Non_Void_Func);

}

void funcStatementParser(ifstream& in,ofstream& out) {//主要是解析函数的参数表和执行语句
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != LPARENT) error();
    printAns(type,ans,out);
    parameterTableParser(in,out);
    type = wordAnalysis(in,ans);
    if (type != RPARENT) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != LBRACE) error();
    printAns(type,ans,out);
    compoundStatementParser(in,out);
    type = wordAnalysis(in,ans);
    if (type != RBRACE) error();
    printAns(type,ans,out);
}

void parameterTableParser(ifstream& in,ofstream& out) {
    streampos sp = in.tellg();
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type == RPARENT) {
        in.seekg(sp);
    } else {
        for (;;) {
            if (type != INTTK && type != CHARTK) error();
            printAns(type,ans,out);
            type = wordAnalysis(in,ans);
            if (type != IDENFR) error();
            printAns(type,ans,out);
            sp = in.tellg();
            type = wordAnalysis(in,ans);
            if (type == COMMA) {
                printAns(type,ans,out);
                type = wordAnalysis(in,ans);
            } else if (type == RPARENT){
                break;
            } else {
                error();
                break;
            }
        }
        in.seekg(sp);
    }
    printType(out,Non_Para_Table);
}

void mainFuncParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != VOIDTK) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != MAINTK) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != LPARENT) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != RPARENT) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != LBRACE) error();
    printAns(type,ans,out);
    compoundStatementParser(in,out);
    type = wordAnalysis(in,ans);
    if (type != RBRACE) error();
    printAns(type,ans,out);
    printType(out,Non_Main_Func);
}

void explainationParser(ifstream& in,ofstream& out) {
    string ans;
    streampos sp = in.tellg();
    myType type = wordAnalysis(in,ans);
    if (type == PLUS || type == MINU)
        printAns(type,ans,out);
    else
        in.seekg(sp);
    itemParser(in,out);
    sp = in.tellg();
    type = wordAnalysis(in,ans);
    while (type == PLUS || type == MINU) {
        printAns(type,ans,out);
        itemParser(in,out);
        sp = in.tellg();
        type = wordAnalysis(in,ans);
    }
    in.seekg(sp);
    printType(out,Non_Expr);
}

void itemParser(ifstream& in,ofstream& out) {//项
    factorParser(in,out);
    string ans;
    streampos sp = in.tellg();
    myType type = wordAnalysis(in,ans);
    while (type == MULT || type == DIV) {
        printAns(type,ans,out);
        factorParser(in,out);
        sp = in.tellg();
        type = wordAnalysis(in,ans);
    }
    in.seekg(sp);
    printType(out,Non_Item);
}

void factorParser(ifstream& in,ofstream& out) {
    streampos sp = in.tellg();
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type == IDENFR) {
        streampos sp2 = in.tellg();
        string ans2;
        myType type2 = wordAnalysis(in,ans2);
        if (type2 == LPARENT && symbolTable.find(ans) != symbolTable.end() && symbolTable[ans] == returnFunc) {
            in.seekg(sp);
            valFuncCallStatementParser(in,out);//有返回值的函数调用
        } else {
            printAns(type,ans,out);//输出标识符
            if (type2 == LBRACK) {
                printAns(type2,ans2,out);
                explainationParser(in,out);
                type = wordAnalysis(in,ans);
                if (type != RBRACK) error();
                printAns(type,ans,out);
            } else {
                in.seekg(sp2);
            }
        }
    } else if (type == LPARENT) {
        printAns(type,ans,out);
        explainationParser(in,out);
        type = wordAnalysis(in,ans);
        if (type != RPARENT) error();
        printAns(type,ans,out);
    } else if (type == PLUS || type == MINU || type == INTCON) {
        in.seekg(sp);
        integerParser(in,out);
    }  else if (type == CHARCON) {
        printAns(type,ans,out);
    }
    printType(out,Non_Factor);
}

void valFuncCallStatementParser(ifstream& in,ofstream& out) {
    funcCallParser(in,out);
    printType(out,Non_ReturnFunc_Call);
}

void nonFuncCallStatementParser(ifstream& in,ofstream& out) {
    funcCallParser(in,out);
    printType(out,Non_VoidFunc_Call);
}

void funcCallParser(ifstream& in,ofstream& out) {
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != IDENFR) error();
    printAns(type,ans,out);
    type = wordAnalysis(in,ans);
    if (type != LPARENT) error();
    printAns(type,ans,out);
    valueParameterTableParser(in,out);
    type = wordAnalysis(in,ans);
    if (type != RPARENT) error();
    printAns(type,ans,out);
}

void valueParameterTableParser(ifstream& in,ofstream& out) {
    streampos sp = in.tellg();
    string ans;
    myType type = wordAnalysis(in,ans);
    if (type != RPARENT) {
        while (type != RPARENT) {
            in.seekg(sp);
            explainationParser(in,out);
            sp = in.tellg();
            type = wordAnalysis(in,ans);
            if (type == COMMA) {
                sp = in.tellg();
                printAns(type,ans,out);
            }
        }
    }
    printType(out,Non_Value_Table);
    in.seekg(sp);
}

void programParser(ifstream& in,ofstream& out) {
    string ans;
    streampos sp = in.tellg();
    myType type = wordAnalysis(in,ans);
    if (type == CONSTTK) {//处理程序中的全局常量说明
        in.seekg(sp);
        constDeclarationParser(in,out);
        sp = in.tellg();
    }

    in.seekg(sp);
    type = wordAnalysis(in,ans);
    if (type == INTTK || type == CHARTK) { //处理程序的全局变量说明
        wordAnalysis(in,ans);
        type = wordAnalysis(in,ans);
        if (type != LPARENT) {//向后读两个单词，遇到左括号说明是函数定义
            in.seekg(sp);
            variableDeclarationParser(in,out);
            sp = in.tellg();
        }
    }

    in.seekg(sp);
    type = wordAnalysis(in,ans);
    bool reachMain = false;
    while ((type == INTTK || type == CHARTK || type == VOIDTK) && !reachMain) { //处理程序中的函数说明
        if (type == INTTK || type == CHARTK) {
            in.seekg(sp);
            valReturnFuncDefinitionParser(in,out);
            sp = in.tellg();
        } else {
            type = wordAnalysis(in,ans);
            if (type == MAINTK) {
                reachMain = true;
            } else {
                in.seekg(sp);
                nonReturnFuncDefinitionParser(in,out);
                sp = in.tellg();
            }
        }
        type = wordAnalysis(in,ans);
    }

    in.seekg(sp);
    mainFuncParser(in,out);

    printType(out,Non_Program);
}

void error() {
    printf("error\n");
}