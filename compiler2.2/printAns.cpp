#include<bits/stdc++.h>
#include "WordType.h"
using namespace std;
unordered_map<int ,string> typeName {
        {IDENFR, "IDENFR"},
        {INTCON, "INTCON" },
        {CHARCON,"CHARCON" },
        {STRCON,"STRCON" },
        {CONSTTK, "CONSTTK" },
        {INTTK,"INTTK" },
        {CHARTK,"CHARTK" },
        {VOIDTK, "VOIDTK"},
        {MAINTK,"MAINTK"},
        {IFTK, "IFTK"},
        {ELSETK," ELSETK" },
        {DOTK,"DOTK" },
        {WHILETK, "WHILETK"},
        {FORTK,"FORTK"},
        {SCANFTK, "SCANFTK"},
        {PRINTFTK, "PRINTFTK"},
        {RETURNTK, "RETURNTK"},
        {PLUS,"PLUS" },
        {MINU, "MINU"},
        {MULT,"MULT" },
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
        {RBRACE,"RBRACE"},
        {ERROR,"ERROR"}
};
void printAns(myType type,string& ans,ofstream& out) {
    if (type != ERROR) {
        out << typeName[type] << " " << ans << endl;
    }
}