// 本程序是词法分析的主程序,即打印结果
#include<bits/stdc++.h>
#include "WordType.h"
#include "WordAnalysis.h"
using namespace std;
int main() {
    ifstream inFile("testfile.txt",ios::in);
    ofstream outFile("output.txt",ios::out);
    while (inFile.peek() != EOF) {
        string ans;
        myType type = wordAnalysis(inFile,ans);
        printAns(type,ans,outFile);
    }
    inFile.close();
    outFile.close();
    return 0;
}