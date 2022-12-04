#include<fstream>
using namespace std;
extern void SyntaxParser(ifstream& in,ofstream& out);
int main() {
    ifstream inFile("testfile.txt",ios::in|ios::binary);
    ofstream outFile("output.txt",ios::out);
    SyntaxParser(inFile,outFile);
    return 0;
}