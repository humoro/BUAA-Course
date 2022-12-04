// 本程序是词法分析的主程序,即打印结果
#include<iostream>
#include<fstream>
#include "WordType.h"
using namespace std;
extern myType wordAnalysis(ifstream& in,string& ans);
int main() {
    unordered_map<int,string> typeName {
            {IDENFR, "IDENFR"},
            { INTCON, "INTCON" },
            { CHARCON,"CHARCON" },
            { STRCON,"STRCON" },
            { CONSTTK, "CONSTTK" },
            { INTTK,"INTTK" },
            { CHARTK,"CHARTK" },
            {VOIDTK, "VOIDTK"},
            { MAINTK,"MAINTK" },
            {IFTK, "IFTK"},
            { ELSETK," ELSETK" },
            { DOTK,"DOTK" },
            { WHILETK, "WHILETK"},
            { FORTK,"FORTK" },
            {SCANFTK, "SCANFTK"},
            {PRINTFTK, "PRINTFTK"},
            {RETURNTK, "RETURNTK"},
            { PLUS,"PLUS" },
            {MINU, "MINU"},
            { MULT,"MULT" },
            {DIV,"DIV"},
            {LSS,"LSS"},
            {LEQ,"LEQ"},
            {GRE,"GRE"},
            {GEQ,"GEQ"},
            {EQL,"EQL"},
            {NEQ,"NEQ"},
            {ASSIGN,"ASSIGN"},
            {SEMICN,"SEMICN"},
            {COMMA,"COMMA"},
            {LPARENT,"LPARENT"},
            {RPARENT,"RPARENT"},
            {LBRACK,"LBRACK"},
            {RBRACK,"RBRACK"},
            {LBRACE,"LBRACE"},
            {RBRACE,"RBRACE"}
    };
    ifstream inFile("testfile.txt",ios::in);
    ofstream outFile("output.txt",ios::out);
    if (!inFile.is_open()) {
        cout << "open error!" << endl;
        return 1;
    }
    myType type;
    string ans;
    while (inFile.peek() != EOF) {
        ans.clear();
        type = wordAnalysis(inFile,ans);
        if (type != ERROR)
            outFile << typeName[type] << " " << ans << endl;
    }
    inFile.close();
    outFile.close();
    return 0;
}