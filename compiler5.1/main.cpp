#include <fstream>
#include <iostream>
#include <cstdio>
#include "Parser.h"
using namespace std;
int main() {
    printf("Welcome using my compiler which supports the C0 grammar...\n");
    printf("Please input your source code file path (your source file should be named as 'testfile.txt') : \n");
    ifstream in("testfile.txt",ios::in|ios::binary);//�����Ʒ�ʽ���ļ����������ļ�ָ����ƶ�
    ofstream error("error.txt",ios::out);
    cerr.rdbuf(error.rdbuf());//�ض��������
    if (!in) {
        printf("OPEN ERROR, there something wrong happened when open your file...\n");
        return 0;
    }
    printf("Now starting analyzing the syntax grammar...\n");
    auto* syntaxParser = new SyntaxParser(in);
    syntaxParser->startProgramParser();
    if (syntaxParser->haveError()) {
        printf("Sorry, your program exists syntax error. Come on, you will succeed...\n");
        return 0;
    }
    printf("Congratulations for passing the syntax check...\n");
    printf("Now starting generating intermediate code...\n");
    return 0;
}