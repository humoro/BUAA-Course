#include "Type.h"
bool isIdentifierToken(TokenType type) {
    return type == IDENFR;
}

bool isIntConToken(TokenType type) {
    return type == INTCON;
}

bool isCharConToken(TokenType type) {
    return type == CHARCON;
}

bool isStrConToken(TokenType type) {
    return type == STRCON;
}

bool isIntToken(TokenType type) {
    return type == INTTK;
}

bool isCharToken(TokenType type) {
    return type == CHARTK;
}

bool isConstToken(TokenType type) {
    return type == CONSTTK;
}

bool isTypeIdenfrToken(TokenType type) {
    return type == INTTK || type == CHARTK;
}

bool isVoidToken(TokenType type) {
    return type == VOIDTK;
}

bool isMainToken(TokenType type) {
    return type == MAINTK;
}

bool isIfToken(TokenType type) {
    return type == IFTK;
}

bool isElseToken(TokenType type) {
    return type == ELSETK;
}

bool isDoToken(TokenType type) {
    return DOTK;
}

bool isWhile(TokenType type) {
    return type == WHILETK;
}

bool isForToken(TokenType type) {
    return type == FORTK;
}

bool isScanfToken(TokenType type) {
    return SCANFTK;
}

bool isPrintfToken(TokenType type) {
    return PRINTFTK;
}

bool isReturnToken(TokenType type) {
    return type == RETURNTK;
}

bool isPlusToken(TokenType type) { //包含加减
    return type == PLUS || type == MINU;
}

bool isMultipleToken(TokenType type) {//包含乘除
    return type == MULT || type == DIV;
}

bool isCompareToken(TokenType type) { //包含，大于，小于，不小于，不大于，等于，不等于
    return type == EQL || type == NEQ || type == LSS || type == LEQ ||type == GRE || type == GEQ;
}

bool isAssignToken(TokenType type) {
    return type == ASSIGN;
}

bool isSemiToken(TokenType type) {
    return type == SEMICN;
}

bool isCommaToken(TokenType type) {
    return type == COMMA;
}

bool isLparentToken(TokenType type) {
    return type == LPARENT;
}

bool isLbrackToken(TokenType type) {
    return type == LBRACK;
}

bool isLbrace(TokenType type) {
    return type == LBRACE;
}

bool isRparentToken(TokenType type) {
    return type == RPARENT;
}

bool isRbrackToken(TokenType type) {
    return type == RBRACK;
}

bool isRbraceToken(TokenType type) {
    return type == RBRACE;
}