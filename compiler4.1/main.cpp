#include<fstream>
#include<iostream>
#include "Parser.h"
using namespace std;
long long linenum = 1;
int main() {
    ifstream in("testfile.txt",ios::in|ios::binary);//�����Ʒ�ʽ���ļ����������ļ�ָ����ƶ�
    ofstream out("error.txt",ios::out);
    cin.rdbuf(in.rdbuf());//�ض���������
    cout.rdbuf(out.rdbuf());//�ض��������
    SyntaxParser();
    return 0;
}