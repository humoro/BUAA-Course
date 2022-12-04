#ifndef COMPILER5_3_ERROR_H
#define COMPILER5_3_ERROR_H
#include "Type.h"
class ErrorProcessor{
    unordered_map<int,string> errorString = {
            {a,"a"},
            {b,"b"},
            {c,"c"},
            {d,"d"},
            {e,"e"},
            {f,"f"},
            {g,"g"},
            {h,"h"},
            {i,"i"},
            {j,"j"},
            {k,"k"},
            {l,"l"},
            {m,"m"},
            {n,"n"},
            {o,"o"},
            {p,"p"}
    };
public:
    void error(ErrorType error,long long linenum) {
        string name = errorString[error];
        cerr << linenum <<  " " << name << endl;
        printf("ERROR::  ");
        printf("in line of %4lld    ::  ",linenum);
        switch (error) {
            case a:
                printf("exist illegal character or illegal lexical using ");
                break;
            case b:
                printf("redefinition of symbol identifier ");
                break;
            case c:
                printf("using the undefined variable or function ");
                break;
            case d:
                printf("call function with unmatched number of value ");
                break;
            case e:
                printf("call function with unmatched type of variable ");
                break;
            case f:
                printf("exist illegal type in the condition statement ");
                break;
            case g:
                printf("void return function exists unmatched return statement ");
                break;
            case h:
                printf("value return function lack return statement or exists unmatched return statement ");
                break;
            case i:
                printf("index of array is not type of integer ");
                break;
            case j:
                printf("trying to change value of variable which is const ");
                break;
            case k:
                printf("lack of semicolon or exits illegal character which is supposed to be semicolon ");
                break;
            case l:
                printf("lack of right parent or exists illegal character which is supposed to be right parent ");
                break;
            case m:
                printf("lack of right bracket or exists illegal character which is supposed to be right bracket ");
                break;
            case n:
                printf("lack while in the do-while recycle statements ");
                break;
            case o:
                printf("assign unmatched numerical value to const variable in const variable definition ");
                break;
            case p:
                printf("input symbol with illegal type ");
                break;
        }
        printf("\n");
    }
};
#endif
