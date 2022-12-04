#pragma once
#ifndef COMPILER3_2_SYNTAXTREE_H
#define COMPILER3_2_SYNTAXTREE_H
#include "Type.h"
#include "Token.h"
#include <fstream>
#include <vector>
#include <iostream>
using namespace std;
class SyntaxNode{
    TokenType type;
    string content;
    bool hasContent;
    vector<SyntaxNode*> kids;
public:
    explicit SyntaxNode(TokenType type) : type(type){
        this->hasContent = false;
        this->kids = vector<SyntaxNode*>(0, nullptr);
    }

    SyntaxNode(TokenType type, string& content) : type(type), content(content){
        this->hasContent = true;
        this->kids = vector<SyntaxNode*>(0, nullptr);
    }

    void addKid(SyntaxNode* node) {
        this->kids.push_back(node);
    }

    vector<SyntaxNode*> getKids() {
        return this->kids;
    }
};

class SyntaxTree{
    SyntaxNode* root;
public:
    explicit SyntaxTree(SyntaxNode* root) {
        this->root = root;
    }

};
#endif //COMPILER3_2_SYNTAXTREE_H
