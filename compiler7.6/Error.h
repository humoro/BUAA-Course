#ifndef COMPILER7_2_ERROR_H
#define COMPILER7_2_ERROR_H
#include "Type.h"
#include <unordered_map>
using namespace std;
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
    void error(ErrorType error,long long linenum);
};
#endif
