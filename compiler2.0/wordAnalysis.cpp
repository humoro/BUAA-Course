#include <bits/stdc++.h>
using namespace std;
/* 
	this is a simple word analysis program 
	this anslysis program can only specify the identfy symbol which almost equals to the string

*/

enum Mytype{
    BEGINSY, //开始标识符
    ENDSY, //结束标识符
    IFSY, // 条件if标识符
    THENSY, // 条件then 标识符
    ELSESY, // 条件else标识符
    IDSY, // 普通标识符
    INTSY,// 整常数标识符
    PLUSSY,// 加号
    MINUSSY,// 减号
    STARSY,//星号（乘号）
    DIVIDSY,//除号（斜杠）
    LPARSY,//左括号
    RPARSY,//右括号
    COMMASY,//逗号
    SEMISY,//分号
    COLONSY,//冒号
    ASSIGNSY,//赋值语句 ：=
    EQUSY,	//等号
    ERROR = -1
};

bool isSpace(char c){ // include the space tab and newline smybols
	return isspace(c);
}

bool isDigit(char c) {
	return isdigit(c);
}

bool isLetter(char c) {
	return isalpha(c);
}

bool isStar(char c) {
	return c == '*';
}

bool isMinus(char c) {
	return c == '-';
}

bool isPlus(char c) {
	return c == '+';
}

bool isDivid(char c) {
	return c == '/';
}

bool isColon(char c) {
	return c == ':';
}

bool isEqu(char c) {
	return c == '=';
}

bool isComma(char c) {
	return c == ',';
}

bool isSemmi(char c) {
	return c == ';';
}

bool isLpar(char c) {
	return c == '(';
}

bool isRpar(char c) {
	return c == ')';
}

Mytype analysisFunc(ifstream& in,string& ans) {
    unordered_map<int,string> typenameMap {//保留字表
            {BEGINSY,"begin"},
            {ENDSY,"end"},
            {IFSY, "if"},
            {THENSY, "then"},
            {ELSESY, "else"}
    };
    ans.clear();
    char symbol = in.get();
    while (isSpace(symbol)) {
        in >> symbol;
    }
	ans += symbol;
	if (isLetter(symbol)) {
        symbol = in.peek();
        while (isLetter(symbol) || isDigit(symbol)) {
            ans += symbol;
            in.get();
            symbol = in.peek();
        }
        if (ans == typenameMap[BEGINSY]) return BEGINSY;
        if (ans == typenameMap[ENDSY]) return ENDSY;
        if (ans == typenameMap[IFSY]) return IFSY;
        if (ans == typenameMap[THENSY]) return THENSY;
        if (ans == typenameMap[ELSESY]) return ELSESY;
        else return IDSY;
	} else if (isDigit(symbol)) {
        symbol = in.peek();
        while (isDigit(symbol)) {
            in.get();
            ans += symbol;
            symbol = in.peek();
        }
        return INTSY;
	} else if (isColon(symbol)) {
        symbol = in.peek();
        if (isEqu(symbol)) {
            ans += symbol;
            in.get();
            return ASSIGNSY;
        } else {
            return COLONSY;
        }
	} else {
	    in.get();
        if (isStar(symbol)) return STARSY;
        if (isDivid(symbol)) return DIVIDSY;
        if (isPlus(symbol)) return PLUSSY;
        if (isMinus(symbol)) return MINUSSY;
        if (isComma(symbol)) return COMMASY;
        if (isSemmi(symbol)) return SEMISY;
        if (isLpar(symbol)) return LPARSY;
        if (isRpar(symbol)) return RPARSY;
        return ERROR;
	}
}

void printType(Mytype type, string& ans) {
    switch (type) {
        case BEGINSY: cout << "BEGINSY : ";
            break;
        case ENDSY: cout  << "ENDSY : ";
            break;
        case IFSY: cout << "IFSY : ";
            break;
        case THENSY: cout << "ENDSY : ";
            break;
        case ELSESY: cout << "ELSESY : ";
            break;
        case IDSY: cout << "IDSY : ";
            break;
        case INTSY: cout << "INTSY : ";
            break;
        case PLUSSY: cout << "PLUSSY : ";
            break;
        case MINUSSY: cout << "MINUSSY : ";
            break;
        case STARSY: cout << "STARSY : ";
            break;
        case DIVIDSY: cout << "DIVIDSY : ";
            break;
        case LPARSY: cout << "LPARSY : ";
            break;
        case RPARSY: cout << "RPARSY : ";
            break;
        case COMMASY: cout << "COMMASY : ";
            break;
        case SEMISY: cout << "SEMISY : ";
            break;
        case COLONSY: cout << "COLONSY : ";
            break;
        case ASSIGNSY:cout << "ASSIGNSY : ";
            break;
        case EQUSY: cout << "EQUSY : ";
            break;
        default: cout << "ERROR!" <<endl;
                return;
    }
    cout << ans << endl;
}

int main(int argc, char** argv) {
	ifstream inFile("test.txt", ios::in);
	if (!inFile.is_open()) {
	    cout << "open test file error!" <<endl;
	    return 1;
	}
	Mytype curtype;
	string ans;
    while (inFile.peek() != EOF) {
        curtype = analysisFunc(inFile,ans);
        printType(curtype,ans);
    }
	return 0;
}