#include "Error.h"
#include<iostream>
using namespace std;
void ErrorProcessor::error(ErrorType error,long long linenum) {
    string name = errorString[error];
    cerr << linenum <<  " " << name << endl;
    printf("ERROR::  ");
    printf("in line of %4lld    ::  ",linenum);
    switch (error) {
        case error_a:
            printf("exist illegal character or illegal lexical using ");
            break;
        case error_b:
            printf("redefinition of symbol identifier ");
            break;
        case error_c:
            printf("using the undefined variable or function ");
            break;
        case error_d:
            printf("call function with unmatched number of value ");
            break;
        case error_e:
            printf("call function with unmatched type of variable ");
            break;
        case error_f:
            printf("exist illegal type in the condition statement ");
            break;
        case error_g:
            printf("void return function exists unmatched return statement ");
            break;
        case error_h:
            printf("value return function lack return statement or exists unmatched return statement ");
            break;
        case error_i:
            printf("index of array is not type of integer ");
            break;
        case error_j:
            printf("trying to change value of variable which is const ");
            break;
        case error_k:
            printf("lack of semicolon or exits illegal character which is supposed to be semicolon ");
            break;
        case error_l:
            printf("lack of right parent or exists illegal character which is supposed to be right parent ");
            break;
        case error_m:
            printf("lack of right bracket or exists illegal character which is supposed to be right bracket ");
            break;
        case error_n:
            printf("lack while in the do-while recycle statements ");
            break;
        case error_o:
            printf("assign unmatched numerical value to const variable in const variable definition ");
            break;
        case error_p:
            printf("input symbol with illegal type ");
            break;
    }
    printf("\n");
}