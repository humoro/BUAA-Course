#include<fstream>
#include "SyntaxTree.h"
using namespace std;
extern SyntaxTree* SyntaxParser();
int main() {
    ifstream in("testfile.txt",ios::in|ios::binary);
    ofstream out("output.txt",ios::out);
    cin.rdbuf(in.rdbuf());
    cout.rdbuf(out.rdbuf());
    auto* root = SyntaxParser();
    root->printTree();
    return 0;
}