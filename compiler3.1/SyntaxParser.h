#pragma once
#ifndef COMPILER_3_1_SYNTAX_PARSER
#define COMPILER_3_1_SYNTAX_PARSER
#include "Types.h"
using namespace std;
void characterParser(ifstream& in,ofstream& out);//字符
void stringParser(ifstream& in,ofstream& out);
void constDeclarationParser(ifstream& in,ofstream& out);
void constDefinitionParser(ifstream& in,ofstream& out);
void unsignedIntgerParser(ifstream& in,ofstream& out);
void integerParser(ifstream& in,ofstream& out);
void declarationHeadParser(ifstream& in,ofstream& out);
void variableDeclarationParser(ifstream& in,ofstream& out);
void variableDefinitionParser(ifstream& in,ofstream& out);
void valReturnFuncDefinitionParser(ifstream& in,ofstream& out);
void nonReturnFuncDefinitionParser(ifstream& in,ofstream& out);
void funcStatementParser(ifstream& in,ofstream& out);
void parameterTableParser(ifstream& in,ofstream& out);
void mainFuncParser(ifstream& in,ofstream& out);
void compoundStatementParser(ifstream& in,ofstream& out);
void explainationParser(ifstream& in,ofstream& out);
void itemParser(ifstream& in,ofstream& out);
void factorParser(ifstream& in,ofstream& out);
void assignStatementParser(ifstream& in,ofstream& out);
void conditionStatementParser(ifstream& in,ofstream& out);
void conditionParser(ifstream& in,ofstream& out);
void circleStatementParser(ifstream& in,ofstream& out);
void stepStatementParser(ifstream& in,ofstream& out);
void valFuncCallStatementParser(ifstream& in,ofstream& out);
void nonFuncCallStatementParser(ifstream& in,ofstream& out);
void funcCallParser(ifstream& in,ofstream& out);
void valueParameterTableParser(ifstream& in,ofstream& out);
void statementParser(ifstream& in,ofstream& out);
void statementColumnParser(ifstream& in,ofstream& out);
void readStatementParser(ifstream& in,ofstream& out);
void printStatementParser(ifstream& in,ofstream& out);
void returnStatementParser(ifstream& in,ofstream& out);
void programParser(ifstream& in,ofstream& out);
myType wordAnalysis(ifstream& in,string& ans);
string getTypeName(myType type);
void printType(ofstream& out,myType type);
void printAns(myType type,string& ans,ofstream& out);
void error();
# endif

