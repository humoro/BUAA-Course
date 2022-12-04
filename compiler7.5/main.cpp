#include <iostream>
#include <cstdio>
#include "Mips.h"
#include "Parse.h"
using namespace std;
int main() {
    printf("Welcome using my compiler which supports the C0 grammar...\n");
    printf("Please input your source code file path (your source file should be named as 'testfile.txt') : \n");
    ifstream in("testfile.txt",ios::in|ios::binary);//二进制方式打开文件利于输入文件指针的移动
    ofstream error("error.txt",ios::out);
    cerr.rdbuf(error.rdbuf());//重定向错误流
    if (!in) {
        printf("OPEN ERROR, there something wrong happened when opening your file...\n");
        return 0;
    }
    printf("Now starting analyzing the syntax grammar...\n");
    auto* globalTable = new SymbolTable();//全局分析符号表
    auto* syntaxParser = new SyntaxParser(in, globalTable);
    syntaxParser->startProgramParser();//完成中间代码的生成
    if (syntaxParser->haveSyntaxError()) {
        printf("Sorry, your program exists syntax error...\n");
        return 0;
    }
    printf("Congratulations for passing the syntax check...\n");
    printf("Now starting generating intermediate code...\n");
    printf("Succeed generating intermediate code...\n");
    printf("Now starting generating mips code...\n");
    ofstream beforeFile("17373020_李进_优化前中间代码.txt", ios::out);
    ofstream afterFile("17373020_李进_优化后中间代码.txt", ios::out);
    ofstream myBeforeIR("myBeforeIR.txt", ios::out);
    ofstream myAfterIR("myAfterIR.txt", ios::out);
    ofstream mipsFile("mips.txt",ios::out);//打开mips输出文件
    MipsGenerator generator(mipsFile, beforeFile, afterFile, myBeforeIR, myAfterIR, globalTable, syntaxParser->irNode());
    generator.generateMips(true);
    printf("Succeed generating mips code...\n");
    printf("Compiling the source code successfully...\n");
    printf("Compiler exit successfully...\n");
    return 0;
}