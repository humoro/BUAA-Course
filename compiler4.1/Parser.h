#ifndef COMPILER4_1_PARSER_H
#define COMPILER4_1_PARSER_H
#include "Type.h"
using namespace std;
//语法分析函数
void SyntaxParser();
void programParser();
void constDeclarationParser();
void constDefinitionParser();
void variableDeclarationParser();
void variableDefinitionParser();
void returnFuncDefinitionParser();
void voidFuncDefinitionParser();
void mainFuncParser();
void integerParser();
void unsignedIntParser();
void funcStatementParser(string& functionName);
void funcParameterListParser();
void compoundStatementParser(string& functionName);
void statementColumnParser();
void statementParser();
void assignStatementParser();
void conditionStatementParser();
void conditionParser();
void recycleStatementParser();
void stepStatementParser();
void readStatementParser();
void printStatementParser();
void returnStatementParser();
void returnFuncCallParser();
void voidFuncCallParser();
void stringParser();
ExprType expressionParser();
ExprType itemParser();
ExprType factorParser();
void funcCallParser();
void funcCallValueListParser(string& functionName);
void error();
//词法分析函数
tokenType wordParser(string& ans);
#endif //COMPILER3_2_PARSERS_H
