#include<fstream>
#include<iostream>
#include "Parser.h"
using namespace std;
long long linenum = 1;
int main() {
    ifstream in("testfile.txt",ios::in|ios::binary);//二进制方式打开文件利于输入文件指针的移动
    ofstream out("error.txt",ios::out);
    cin.rdbuf(in.rdbuf());//重定向输入流
    cout.rdbuf(out.rdbuf());//重定向输出流
    SyntaxParser();
    return 0;
}