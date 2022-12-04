#ifndef COMPILER7_2_ERROR_H
#define COMPILER7_2_ERROR_H
#include "Type.h"
#include <unordered_map>
using namespace std;
class ErrorProcessor{
    unordered_map<int,string> errorString = {
            {error_a, "a"},
            {error_b, "b"},
            {error_c, "c"},
            {error_d, "d"},
            {error_e, "e"},
            {error_f, "f"},
            {error_g, "g"},
            {error_h, "h"},
            {error_i, "i"},
            {error_j, "j"},
            {error_k, "k"},
            {error_l, "l"},
            {error_m, "m"},
            {error_n, "n"},
            {error_o, "o"},
            {error_p, "p"}
    };
public:
    void error(ErrorType error,long long linenum);
};
#endif
