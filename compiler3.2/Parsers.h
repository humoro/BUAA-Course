#ifndef COMPILER3_2_PARSERS_H
#define COMPILER3_2_PARSERS_H
#include <fstream>
#include <iostream>
#include "SyntaxTree.h"
//语法分析函数
void programParser(SyntaxNode* tree);
void constDeclarationParser(SyntaxNode* node);
void constDefinitionParser(SyntaxNode* node);
void variableDeclarationParser(SyntaxNode* node);
void variableDefinitionParser(SyntaxNode* node);
void returnFuncDefinitionParser(SyntaxNode* node);
void voidFuncDefinitionParser(SyntaxNode* node);
void mainFuncParser(SyntaxNode* node);
void integerParser(SyntaxNode* node);
void unsignedIntParser(SyntaxNode* node);
void funcStatementParser(SyntaxNode* node);
void funcHeadParser(SyntaxNode* node);
void funcParameterListParser(SyntaxNode* node);
void compoundStatementParser(SyntaxNode* node);
void statementColumnParser(SyntaxNode* node);
void statementParser(SyntaxNode* node);
void assignStatementParser(SyntaxNode* node);
void conditionStatementParser(SyntaxNode* node);
void conditionParser(SyntaxNode* node);
void recycleStatementParser(SyntaxNode* node);
void stepStatementParser(SyntaxNode* node);
void readStatementParser(SyntaxNode* node);
void printStatementParser(SyntaxNode* node);
void returnStatementParser(SyntaxNode* node);
void returnFuncCallParser(SyntaxNode* node);
void voidFuncCallParser(SyntaxNode* node);
void stringParser(SyntaxNode* node);
void expressionParser(SyntaxNode* node);
void itemParser(SyntaxNode* node);
void factorParser(SyntaxNode* node);
void funcCallParser(SyntaxNode* node);
void funcCallValueListParser(SyntaxNode* node);
void error();
//词法分析函数
myType wordParser(string& ans);
#endif //COMPILER3_2_PARSERS_H
