#include <iostream>
#include <cstdio>
#include "Mips.h"
#include "Parse.h"
using namespace std;
int main() {
    printf("Welcome using my compiler which supports the C0 grammar...\n");
    printf("Please input your source code file path (your source file should be named as 'testfile.txt') : \n");
    ifstream in("testfile.txt",ios::in|ios::binary);//�����Ʒ�ʽ���ļ����������ļ�ָ����ƶ�
    ofstream error("error.txt",ios::out);
    cerr.rdbuf(error.rdbuf());//�ض��������
    if (!in) {
        printf("OPEN ERROR, there something wrong happened when opening your file...\n");
        return 0;
    }
    printf("Now starting analyzing the syntax grammar...\n");
    auto* globalTable = new SymbolTable();//ȫ�ַ������ű�
    auto* syntaxParser = new SyntaxParser(in, globalTable);
    syntaxParser->startProgramParser();//����м���������
    if (syntaxParser->haveSyntaxError()) {
        printf("Sorry, your program exists syntax error...\n");
        return 0;
    }
    printf("Congratulations for passing the syntax check...\n");
    printf("Now starting generating intermediate code...\n");
    printf("Succeed generating intermediate code...\n");
    printf("Now starting generating mips code...\n");
    ofstream beforeFile("17373020_���_�Ż�ǰ�м����.txt", ios::out);
    ofstream afterFile("17373020_���_�Ż����м����.txt", ios::out);
    ofstream myBeforeIR("myBeforeIR.txt", ios::out);
    ofstream myAfterIR("myAfterIR.txt", ios::out);
    ofstream mipsFile("mips.txt",ios::out);//��mips����ļ�
    MipsGenerator generator(mipsFile, beforeFile, afterFile, myBeforeIR, myAfterIR, globalTable, syntaxParser->irNode());
    generator.generateMips(true);
    printf("Succeed generating mips code...\n");
    printf("Compiling the source code successfully...\n");
    printf("Compiler exit successfully...\n");
    return 0;
}