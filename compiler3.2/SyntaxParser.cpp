#include<cstdio>
#include<unordered_map>
#include "Parsers.h"
#include "SyntaxTree.h"
#include "Types.h"
#include "Signary.h"
using namespace std;
extern unordered_map<SymbolName,SymbolTableEntry*> functionTable;
SyntaxTree* SyntaxParser() {
    auto* root = new SyntaxNode(SyntaxModel);
    auto* tree = new SyntaxTree(root);
    programParser(root);
    return tree;
}

void programParser(SyntaxNode* node) {
    auto* program = new SyntaxNode(Non_Program);
    node->addKid(program);//程序语法树的根是非终结符“程序”
    string ans;
    streampos sp = cin.tellg();
    myType type = wordParser(ans);
    if (type == CONSTTK) {//处理程序中的全局常量说明
        cin.seekg(sp);
        constDeclarationParser(program);
        sp = cin.tellg();
    }

    cin.seekg(sp);
    type = wordParser(ans);
    if (type == INTTK || type == CHARTK) { //处理程序的全局变量说明
        wordParser(ans);
        type = wordParser(ans);
        if (type != LPARENT) {//向后读两个单词，遇到左括号说明是函数定义
            cin.seekg(sp);
            variableDeclarationParser(program);
            sp = cin.tellg();
        }
    }
    cin.seekg(sp);
    type = wordParser(ans);
    bool reachMain = false;
    while ((type == INTTK || type == CHARTK || type == VOIDTK) && !reachMain) { //处理程序中的函数说明
        if (type == INTTK || type == CHARTK) {
            cin.seekg(sp);
            returnFuncDefinitionParser(program);
            sp = cin.tellg();
        } else {
            type = wordParser(ans);
            if (type == MAINTK) {
                reachMain = true;
            } else {
                cin.seekg(sp);
                voidFuncDefinitionParser(program);
                sp = cin.tellg();
            }
        }
        type = wordParser(ans);
    }
    cin.seekg(sp);
    mainFuncParser(program);
}

void constDeclarationParser(SyntaxNode* node) {
    auto* conDeclaration = new SyntaxNode(Non_Const_Decl);
    node->addKid(conDeclaration);
    string ans;
    myType type = wordParser(ans);
    streampos sp = cin.tellg();
    while (type == CONSTTK) {
        conDeclaration->addKid(new SyntaxNode(type,ans));
        constDefinitionParser(conDeclaration);
        type = wordParser(ans);
        if (type != SEMICN) error();
        conDeclaration->addKid(new SyntaxNode(type,ans));
        sp = cin.tellg();
        type = wordParser(ans);
    }
    cin.seekg(sp);
}

void constDefinitionParser(SyntaxNode* node) {
    auto* conDefinition = new SyntaxNode(Non_Const_Def);
    node->addKid(conDefinition);
    string ans;
    streampos sp = cin.tellg();
    myType type = wordParser(ans);
    myType oritype;
    oritype = type;
    while (type != SEMICN) {
        conDefinition->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != IDENFR) error();
        conDefinition->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != ASSIGN) error();
        conDefinition->addKid(new SyntaxNode(type,ans));
        if (oritype == INTTK)
            integerParser(conDefinition);
        else  if (oritype == CHARTK) {
            type = wordParser(ans);
            if (type != CHARCON) error();
            char ch = ans[0];
            if (!isPlus(ch) && !isMinus(ch) && !isMult(ch) && !isDivid(ch) && !isAlpha(ch) && !isULine(ch) && !isDigit(ch)) error();
            conDefinition->addKid(new SyntaxNode(type,ans));
        } else error();
        sp = cin.tellg();
        type = wordParser(ans);
        if (type != COMMA && type != SEMICN) error();
    }
    cin.seekg(sp);
}

void variableDeclarationParser(SyntaxNode* node) {
    auto* varDeclaration = new SyntaxNode(Non_Var_Decl);
    node->addKid(varDeclaration);
    string ans;
    streampos sp = cin.tellg();
    myType type = wordParser(ans);
    while (type == INTTK || type == CHARTK) {
        wordParser(ans);
        type = wordParser(ans);
        if (type == LPARENT) {
            break;
        } else {
            cin.seekg(sp);
            variableDefinitionParser(varDeclaration);
            type = wordParser(ans);
            if (type != SEMICN) error();
            varDeclaration->addKid(new SyntaxNode(type,ans));
            sp = cin.tellg();
        }
        type = wordParser(ans);
    }
    cin.seekg(sp);
}

void variableDefinitionParser(SyntaxNode* node) {
    auto* varDefinition = new SyntaxNode(Non_Var_Def);
    node->addKid(varDefinition);
    string ans;
    myType type;
    streampos sp;
    type = wordParser(ans);//提取类型标识符
    while (type != SEMICN) {
        varDefinition->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != IDENFR) error();
        varDefinition->addKid(new SyntaxNode(type,ans));
        sp = cin.tellg();
        type = wordParser(ans);
        if (type == COMMA) {
            continue;
        }
        if (type == LBRACK) {//左中括号
            varDefinition->addKid(new SyntaxNode(type,ans));
            unsignedIntParser(varDefinition);
            type = wordParser(ans);
            if (type == RBRACK) {
                varDefinition->addKid(new SyntaxNode(type,ans));
            } else error();
            sp = cin.tellg();
            type = wordParser(ans);
        }
    }
    cin.seekg(sp);
}

void unsignedIntParser(SyntaxNode* node) {
    auto* unsignedInt = new SyntaxNode(Non_Unsigned_int);
    node->addKid(unsignedInt);
    string ans;
    myType type = wordParser(ans);
    if (type != INTCON) error();
    if (ans.size() > 1 && ans[0] == '0') error();
    unsignedInt->addKid(new SyntaxNode(type,ans));
}

void returnFuncDefinitionParser(SyntaxNode* node) {
    auto* returnFuncDef = new SyntaxNode(Non_Return_Func);
    node->addKid(returnFuncDef);
    funcHeadParser(returnFuncDef);
    funcStatementParser(returnFuncDef);
}

void voidFuncDefinitionParser(SyntaxNode* node) {
    auto* voidFuncDef = new SyntaxNode(Non_Void_Func);
    node->addKid(voidFuncDef);
    string ans;
    myType type = wordParser(ans);
    if (type != VOIDTK) error();
    voidFuncDef->addKid(new SyntaxNode(type,ans));
    type = wordParser(ans);
    if (type != IDENFR) error();
    voidFuncDef->addKid(new SyntaxNode(type,ans));
    addVoidFunc(ans);
    funcStatementParser(voidFuncDef);
}

void funcHeadParser(SyntaxNode* node) {
    auto* declHead = new SyntaxNode(Non_Func_Head);
    node->addKid(declHead);
    string ans;
    myType type = wordParser(ans);
    if (type != INTTK && type != CHARTK) error();
    declHead->addKid(new SyntaxNode(type,ans));
    myType type1 = wordParser(ans);
    if (type1 != IDENFR) error();
    declHead->addKid(new SyntaxNode(type1,ans));
    if (type == INTTK) addIntReturnFunc(ans);
    if (type == CHARTK) addCharReturnFunc(ans);
}

void funcStatementParser(SyntaxNode* node) {//主要是解析函数的参数表和执行语句
    string ans;
    myType type = wordParser(ans);
    if (type != LPARENT) error();
    node->addKid(new SyntaxNode(type,ans));
    funcParameterListParser(node);
    type = wordParser(ans);
    if (type != RPARENT) error();
    node->addKid(new SyntaxNode(type,ans));
    type = wordParser(ans);
    if (type != LBRACE) error();
    node->addKid(new SyntaxNode(type,ans));
    compoundStatementParser(node);
    type = wordParser(ans);
    if (type != RBRACE) error();
    node->addKid(new SyntaxNode(type,ans));
}

void funcParameterListParser(SyntaxNode* node) {
    auto* paraTable = new SyntaxNode(Non_Func_ParaList);
    node->addKid(paraTable);
    streampos sp = cin.tellg();
    string ans;
    myType type = wordParser(ans);
    if (type == RPARENT) {
        cin.seekg(sp);
    } else {
        for (;;) {
            if (type != INTTK && type != CHARTK) error();
            paraTable->addKid(new SyntaxNode(type,ans));
            type = wordParser(ans);
            if (type != IDENFR) error();
            paraTable->addKid(new SyntaxNode(type,ans));
            sp = cin.tellg();
            type = wordParser(ans);
            if (type == COMMA) {
                paraTable->addKid(new SyntaxNode(type,ans));
                type = wordParser(ans);
            } else if (type == RPARENT){
                break;
            } else {
                error();
                break;
            }
        }
        cin.seekg(sp);
    }
}

void compoundStatementParser(SyntaxNode* node) {
    auto* cmpdState = new SyntaxNode(Non_Cmpd_Stat);
    node->addKid(cmpdState);
    string ans;
    streampos sp = cin.tellg();
    myType type = wordParser(ans);
    if (type == CONSTTK) {//常量说明
        cin.seekg(sp);
        constDeclarationParser(cmpdState);
        sp = cin.tellg();
        type = wordParser(ans);
    }
    if (type == INTTK || type == CHARTK) {//变量说明
        cin.seekg(sp);
        variableDeclarationParser(cmpdState);
        sp = cin.tellg();
    }
    cin.seekg(sp);
    statementColumnParser(cmpdState);
}

void statementColumnParser(SyntaxNode* node) {
    auto* stateColumn = new SyntaxNode(Non_Stat_Column);
    node->addKid(stateColumn);
    streampos sp = cin.tellg();
    string ans;
    myType type = wordParser(ans);
    while (type != RBRACE) {
        cin.seekg(sp);
        statementParser(stateColumn);
        sp = cin.tellg();
        type = wordParser(ans);
    }
    cin.seekg(sp);
}

void statementParser(SyntaxNode* node) {
    auto* statement = new SyntaxNode(Non_Stat);
    node->addKid(statement);
    streampos sp = cin.tellg();
    string ans;
    myType type = wordParser(ans);
    if (type == RETURNTK) {
        cin.seekg(sp);
        returnStatementParser(statement);
    } else if (type == SCANFTK) {
        cin.seekg(sp);
        readStatementParser(statement);
    } else if (type == PRINTFTK) {
        cin.seekg(sp);
        printStatementParser(statement);
    } else if (type == IFTK) {
        cin.seekg(sp);
        conditionStatementParser(statement);
        goto end;
    } else if (type == WHILETK || type == FORTK || type == DOTK) {
        cin.seekg(sp);
        recycleStatementParser(statement);
        goto end;
    }  else if (type == LBRACE) {
        statement->addKid(new SyntaxNode(type,ans));
        statementColumnParser(statement);
        type = wordParser(ans);
        if (type != RBRACE) error();
        statement->addKid(new SyntaxNode(type,ans));
        goto end;
    } else if (type == IDENFR) {
        string ans2;
        myType type2 = wordParser(ans2);
        cin.seekg(sp);
        if (type2 == LPARENT) {
            if (functionTable.find(ans) != functionTable.end()) {
                if (isReturnFunc(ans)) {
                    returnFuncCallParser(statement);
                } else {
                    voidFuncCallParser(statement);
                }
            }
        } else {
            assignStatementParser(statement);
        }
    } else if (type == SEMICN) {
        cin.seekg(sp);
    } else error();

    type = wordParser(ans);
    if (type != SEMICN) error();
    statement->addKid(new SyntaxNode(type,ans));
    end:
        return;
}

void readStatementParser(SyntaxNode* node) {
    auto* read = new SyntaxNode(Non_Read_Stat);
    node->addKid(read);
    string ans;
    myType type = wordParser(ans);
    if (type != SCANFTK) error();
    read->addKid(new SyntaxNode(type,ans));
    type = wordParser(ans);
    if (type != LPARENT) error();
    read->addKid(new SyntaxNode(type,ans));
    while (type != RPARENT) {
        type = wordParser(ans);
        if (type != IDENFR) error();
        read->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != COMMA && type != RPARENT) error();
        read->addKid(new SyntaxNode(type,ans));
    }
}

void printStatementParser(SyntaxNode* node) {
    auto* print = new SyntaxNode(Non_Print_Stat);
    node->addKid(print);
    string ans;
    myType type = wordParser(ans);
    if (type != PRINTFTK) error();
    print->addKid(new SyntaxNode(type,ans));
    type = wordParser(ans);
    if (type != LPARENT) error();
    print->addKid(new SyntaxNode(type,ans));
    if (cin.peek() == '\"') {
        stringParser(print);
        streampos sp = cin.tellg();
        type = wordParser(ans);
        if (type == COMMA) {
            print->addKid(new SyntaxNode(type,ans));
            expressionParser(print);
        } else {
            cin.seekg(sp);
        }
    } else {
        expressionParser(print);
    }
    type = wordParser(ans);
    if (type != RPARENT) error();
    print->addKid(new SyntaxNode(type,ans));
}

void returnStatementParser(SyntaxNode* node) {
    auto* returnS = new SyntaxNode(Non_Return_Stat);
    node->addKid(returnS);
    string ans;
    myType type = wordParser(ans);
    if (type != RETURNTK) error();
    returnS->addKid(new SyntaxNode(type,ans));
    streampos sp = cin.tellg();
    type = wordParser(ans);
    if (type == LPARENT) {
        returnS->addKid(new SyntaxNode(type,ans));
        expressionParser(returnS);
        type = wordParser(ans);
        if (type != RPARENT) error();
        returnS->addKid(new SyntaxNode(type,ans));
    } else {
        cin.seekg(sp);
    }
}
//赋值语句
void assignStatementParser(SyntaxNode* node) {
    auto* assign = new SyntaxNode(Non_Assign);
    node->addKid(assign);
    string ans;
    myType type = wordParser(ans);
    if (type != IDENFR) error();
    assign->addKid(new SyntaxNode(type,ans));
    type = wordParser(ans);
    if (type != LBRACK && type != ASSIGN) error();
    assign->addKid(new SyntaxNode(type,ans));
    if (type == LBRACK) {
        expressionParser(assign);
        type = wordParser(ans);
        if (type != RBRACK) error();
        assign->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != ASSIGN) error();
        assign->addKid(new SyntaxNode(type,ans));
    }
    expressionParser(assign);
}
//条件语句
void conditionStatementParser(SyntaxNode* node) {
    auto* conditionStat = new SyntaxNode(Non_Condition_Stat);
    node->addKid(conditionStat);
    string ans;
    myType type = wordParser(ans);
    if (type != IFTK) error();
    conditionStat->addKid(new SyntaxNode(type,ans));
    type = wordParser(ans);
    if (type != LPARENT) error();
    conditionStat->addKid(new SyntaxNode(type,ans));
    conditionParser(conditionStat);
    type = wordParser(ans);
    if (type != RPARENT) error();
    conditionStat->addKid(new SyntaxNode(type,ans));
    statementParser(conditionStat);
    streampos sp = cin.tellg();
    type = wordParser(ans);
    if (type == ELSETK) {
        conditionStat->addKid(new SyntaxNode(type,ans));
        statementParser(conditionStat);
    }
    else cin.seekg(sp);
}

void conditionParser(SyntaxNode* node) {
    auto* condition = new SyntaxNode(Non_Condition);
    node->addKid(condition);
    expressionParser(condition);
    string ans;
    streampos sp = cin.tellg();
    myType type = wordParser(ans);
    if (type == RPARENT || type == SEMICN) {
        cin.seekg(sp);
        goto end;
    }
    if (type != LSS && type != GRE && type != GEQ && type != EQL && type != LEQ && type != NEQ) error();
    condition->addKid(new SyntaxNode(type,ans));
    expressionParser(condition);
    end:
        return;
}
//循环语句
void recycleStatementParser(SyntaxNode* node) {
    auto* recycle = new SyntaxNode(Non_Recycle_Stat);
    node->addKid(recycle);
    string ans;
    myType type = wordParser(ans);
    recycle->addKid(new SyntaxNode(type,ans));
    if (type == WHILETK) {
        type = wordParser(ans);
        if (type != LPARENT) error();
        recycle->addKid(new SyntaxNode(type,ans));
        conditionParser(recycle);
        type = wordParser(ans);
        if (type != RPARENT) error();
        recycle->addKid(new SyntaxNode(type,ans));
        statementParser(recycle);
    } else if (type == DOTK) {
        statementParser(recycle);
        type = wordParser(ans);
        if (type != WHILETK) error();
        recycle->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != LPARENT) error();
        recycle->addKid(new SyntaxNode(type,ans));
        conditionParser(recycle);
        type = wordParser(ans);
        if (type != RPARENT) error();
        recycle->addKid(new SyntaxNode(type,ans));
    } else if (type == FORTK) {
        type = wordParser(ans);
        if (type != LPARENT) error();
        recycle->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != IDENFR) error();
        recycle->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != ASSIGN) error();
        recycle->addKid(new SyntaxNode(type,ans));
        expressionParser(recycle);
        type = wordParser(ans);
        if (type != SEMICN) error();
        recycle->addKid(new SyntaxNode(type,ans));
        conditionParser(recycle);
        type = wordParser(ans);
        if (type != SEMICN) error();
        recycle->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != IDENFR) error();
        recycle->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != ASSIGN) error();
        recycle->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != IDENFR) error();
        recycle->addKid(new SyntaxNode(type,ans));
        type = wordParser(ans);
        if (type != PLUS && type != MINU) error();
        recycle->addKid(new SyntaxNode(type,ans));
        stepStatementParser(recycle);
        type = wordParser(ans);
        if (type != RPARENT) error();
        recycle->addKid(new SyntaxNode(type,ans));
        statementParser(recycle);
    } else error();
}

void stepStatementParser(SyntaxNode* node) {
    auto* step = new SyntaxNode(Non_Step_Length);
    node->addKid(step);
    unsignedIntParser(step);
}

void mainFuncParser(SyntaxNode* node) {
    auto* main = new SyntaxNode(Non_Main_Func);
    node->addKid(main);
    string ans;
    myType type = wordParser(ans);
    if (type != VOIDTK) error();
    main->addKid(new SyntaxNode(type,ans));
    type = wordParser(ans);
    if (type != MAINTK) error();
    main->addKid(new SyntaxNode(type,ans));
    type = wordParser(ans);
    if (type != LPARENT) error();
    main->addKid(new SyntaxNode(type,ans));
    type = wordParser(ans);
    if (type != RPARENT) error();
    main->addKid(new SyntaxNode(type,ans));
    type = wordParser(ans);
    if (type != LBRACE) error();
    main->addKid(new SyntaxNode(type,ans));
    compoundStatementParser(main);
    type = wordParser(ans);
    if (type != RBRACE) error();
    main->addKid(new SyntaxNode(type,ans));
}

void stringParser(SyntaxNode* node) {
    auto* str = new SyntaxNode(Non_String);
    node->addKid(str);
    string ans;
    myType  type = wordParser(ans);
    if (type != STRCON) error();
    str->addKid(new SyntaxNode(type,ans));
}

void integerParser(SyntaxNode* node) {
    auto* integer = new SyntaxNode(Non_Integer);
    node->addKid(integer);
    string ans;
    streampos sp = cin.tellg();
    myType type = wordParser(ans);
    if (type == PLUS || type == MINU) {
        integer->addKid(new SyntaxNode(type,ans));
        unsignedIntParser(integer);
    } else {
        cin.seekg(sp);
        unsignedIntParser(integer);
    }
}

void expressionParser(SyntaxNode* node) {
    auto* expr = new SyntaxNode(Non_Expr);
    node->addKid(expr);
    string ans;
    streampos sp = cin.tellg();
    myType type = wordParser(ans);
    if (type == PLUS || type == MINU)
        expr->addKid(new SyntaxNode(type,ans));
    else
        cin.seekg(sp);
    itemParser(expr);
    sp = cin.tellg();
    type = wordParser(ans);
    while (type == PLUS || type == MINU) {
        expr->addKid(new SyntaxNode(type,ans));
        itemParser(expr);
        sp = cin.tellg();
        type = wordParser(ans);
    }
    cin.seekg(sp);
}

void itemParser(SyntaxNode* node) {//项
    auto* item = new SyntaxNode(Non_Item);
    node->addKid(item);
    factorParser(item);
    string ans;
    streampos sp = cin.tellg();
    myType type = wordParser(ans);
    while (type == MULT || type == DIV) {
        item->addKid(new SyntaxNode(type,ans));
        factorParser(item);
        sp = cin.tellg();
        type = wordParser(ans);
    }
    cin.seekg(sp);
}

void factorParser(SyntaxNode* node) {
    auto* factor = new SyntaxNode(Non_Factor);
    node->addKid(factor);
    streampos sp = cin.tellg();
    string ans;
    myType type = wordParser(ans);
    if (type == IDENFR) {
        streampos sp2 = cin.tellg();
        string ans2;
        myType type2 = wordParser(ans2);
        if (type2 == LPARENT && isReturnFunc(ans)) {
            cin.seekg(sp);
            returnFuncCallParser(factor);//有返回值的函数调用
        } else {
            factor->addKid(new SyntaxNode(type,ans));
            if (type2 == LBRACK) {
                factor->addKid(new SyntaxNode(type2,ans2));
                expressionParser(factor);
                type = wordParser(ans);
                if (type != RBRACK) error();
                factor->addKid(new SyntaxNode(type,ans));
            } else {
                cin.seekg(sp2);
            }
        }
    } else if (type == LPARENT) {
        factor->addKid(new SyntaxNode(type,ans));
        expressionParser(factor);
        type = wordParser(ans);
        if (type != RPARENT) error();
        factor->addKid(new SyntaxNode(type,ans));
    } else if (type == PLUS || type == MINU || type == INTCON) {
        cin.seekg(sp);
        integerParser(factor);
    }  else if (type == CHARCON) {
        factor->addKid(new SyntaxNode(type,ans));
    }
}

void returnFuncCallParser(SyntaxNode* node) {
    auto* valFuncCall = new SyntaxNode(Non_ReturnFunc_Call);
    node->addKid(valFuncCall);
    funcCallParser(valFuncCall);
}

void voidFuncCallParser(SyntaxNode* node) {
    auto* voidFuncCall = new SyntaxNode(Non_VoidFunc_Call);
    node->addKid(voidFuncCall);
    funcCallParser(voidFuncCall);
}

void funcCallParser(SyntaxNode* node) {
    string ans;
    myType type = wordParser(ans);
    if (type != IDENFR) error();
    node->addKid(new SyntaxNode(type,ans));
    type = wordParser(ans);
    if (type != LPARENT) error();
    node->addKid(new SyntaxNode(type,ans));
    funcCallValueListParser(node);
    type = wordParser(ans);
    if (type != RPARENT) error();
    node->addKid(new SyntaxNode(type,ans));
}

void funcCallValueListParser(SyntaxNode* node) {
    auto* valParaTable = new SyntaxNode(Non_Value_Table);
    node->addKid(valParaTable);
    streampos sp = cin.tellg();
    string ans;
    myType type = wordParser(ans);
    if (type != RPARENT) {
        while (type != RPARENT) {
            cin.seekg(sp);
            expressionParser(valParaTable);
            sp = cin.tellg();
            type = wordParser(ans);
            if (type == COMMA) {
                sp = cin.tellg();
                valParaTable->addKid(new SyntaxNode(type,ans));
            }
        }
    }
    cin.seekg(sp);
}

void error() {
    printf("error\n");
}