#include <fstream>
#include <iostream>
#include <unordered_map>
#include "Type.h"
using namespace std;
static unordered_map<int, string> errorString = {
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
			{o,"o"}
};
void error(ErrorType error,long long linenum) {
	string name = errorString[error];
    cout << linenum <<  " " << name << endl;
}

