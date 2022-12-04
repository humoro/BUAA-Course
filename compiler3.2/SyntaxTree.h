#pragma once
#ifndef COMPILER3_2_SYNTAXTREE_H
#define COMPILER3_2_SYNTAXTREE_H
#include "Types.h"
#include<fstream>
#include <vector>
class SyntaxNode{
    myType type;
    string content;
    string typeName;
    bool hasContent;
    vector<SyntaxNode*> kids;
public:
    explicit SyntaxNode(myType type) : type(type){
        this->typeName = getTypeName(type);
        this->hasContent = false;
        this->kids = vector<SyntaxNode*>(0, nullptr);
    }

    SyntaxNode(myType type,string& content) : type(type),content(content){
        this->hasContent = true;
        this->typeName = getTypeName(type);
        this->kids = vector<SyntaxNode*>(0, nullptr);
    }

    void printAns() {
        if (this->type == ERROR || this->type == SyntaxModel) return;
        if (!hasContent) {
            cout << typeName << endl;
        } else {
            cout << typeName << " " << content << endl;
        }
    }

    void addKid(SyntaxNode* node) {
        this->kids.push_back(node);
    }

    vector<SyntaxNode*> Kids() {
        return this->kids;
    }
};

class SyntaxTree{
    SyntaxNode* root;
public:
    explicit SyntaxTree(SyntaxNode* root) {
        this->root = root;
    }

    void printTree() {
        tranversal(root);
    }

private:
    static void tranversal(SyntaxNode* visNode) {
        for (auto node : visNode->Kids()) {
            tranversal(node);
        }
        visNode->printAns();
    }
};
#endif //COMPILER3_2_SYNTAXTREE_H
