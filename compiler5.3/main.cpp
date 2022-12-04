#include <fstream>
#include <iostream>
#include <cstdio>
#include "Mips.h"
#include "Parser.h"
using namespace std;
int main() {
    printf("Welcome using my compiler which supports the C0 grammar...\n");
    printf("Please input your source code file path (your source file should be named as 'testfile.txt') : \n");
    ifstream in("testfile.txt",ios::in|ios::binary);//二进制方式打开文件利于输入文件指针的移动
    ofstream error("error.txt",ios::out);
    ofstream outIR("irfile.txt", ios::out);
    cerr.rdbuf(error.rdbuf());//重定向错误流
    if (!in) {
        printf("OPEN ERROR, there something wrong happened when opening your file...\n");
        return 0;
    }
    printf("Now starting analyzing the syntax grammar...\n");
    auto* globalTable = new SymbolTable();//全局分析符号表
    auto* syntaxParser = new SyntaxParser(in, outIR, globalTable);
    syntaxParser->startProgramParser();//完成中间代码的生成
    if (syntaxParser->haveError()) {
        printf("Sorry, your program exists syntax error...\n");
        return 0;
    }
    printf("Congratulations for passing the syntax check...\n");
    printf("Now starting generating intermediate code...\n");
    outIR.close();
    printf("Succeed generating intermediate code...\n");
    printf("Now starting generating mips code...\n");
    ifstream irFile("irfile.txt",ios::in | ios::binary);
    ofstream mipsFile("mips.txt",ios::out);
    MipsGenerator generator(irFile,mipsFile,globalTable);
    generator.generateMips(true);
    printf("Succeed generating mips code...\n");
    printf("Compiling the source code successfully...\n");
    printf("Compiler exit successfully...\n");
    return 0;
}