#pragma once
#ifndef COMPILER6_1_SYNTAXTREE_H
#define COMPILER6_1_SYNTAXTREE_H
#include "Type.h"
#include "Token.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <unordered_set>
using namespace std;
class SyntaxNode{
    TokenType type;
    vector<SyntaxNode*> kids;
    vector<string> interCodes; //该语法树节点所对应生成的中间代码
public:
    explicit SyntaxNode(TokenType type) : type(type){
        this->kids = vector<SyntaxNode*>(0, nullptr);
    }

    void addKid(SyntaxNode* node) {
        this->kids.push_back(node);
    }

    vector<SyntaxNode*> getKids() {
        return this->kids;
    }

    void addCode(string& code) {
        this->interCodes.push_back(code);
    }
};

class SyntaxTree{
    SyntaxNode* root;
public:
    explicit SyntaxTree(SyntaxNode* root) {
        this->root = root;
    }

};
#endif
