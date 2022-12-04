//本程序是用于切割词语并且判断其词语类型,当前词法分析器只适用于没有文法错误的程序
#include "Types.h"
using namespace std;
myType wordParser(string& ans) {
    ans.clear();
    char ch = cin.peek();
    while (isSpace(ch)) {
        cin.get();
        ch = cin.peek();
    }//吞掉换行制表符和空格
    if (isAlpha(ch) || isULine(ch) || isDQuo(ch) || isSQuo(ch)) {
        if (isDQuo(ch)) {//如果是双引号开始,说明是字符串常量
            cin.get();
            ch = cin.peek();
            while (!isDQuo(ch)) {
                ans += ch;
                cin.get();
                ch = cin.peek();
            }
            cin.get();//把右边的双引号吞掉
            return STRCON;
        }
        if (isSQuo(ch)) {//单引号开始，字符常量
            cin.get();
            ch = cin.get();
            ans += ch;
            cin.get();//把右边的单引号吞掉
            return CHARCON;
        }
        ans += ch;
        cin.get();
        ch = cin.peek();
        while(isULine(ch) || isAlpha(ch) || isDigit(ch)) {
            ans += ch;
            cin.get();
            ch = cin.peek();
        }
        //下面是验证当前获得的字符串是否为保留字
        if (isConst(ans)) {
            return CONSTTK;
        } else if (isInt(ans)) {
            return INTTK;
        } else if (isChar(ans)) {
            return CHARTK;
        } else if (isVoid(ans)) {
            return VOIDTK;
        } else if (isMain(ans)) {
            return MAINTK;
        } else if (isIf(ans)) {
            return IFTK;
        } else if (isElse(ans)) {
            return ELSETK;
        } else if (isDo(ans)) {
            return DOTK;
        } else if (isWhile(ans)) {
            return WHILETK;
        } else if (isFor(ans)) {
            return FORTK;
        } else if (isScanf(ans)) {
            return SCANFTK;
        } else if (isPrintf(ans)) {
            return PRINTFTK;
        } else if (isReturn(ans)) {
            return RETURNTK;
        } else {
            return IDENFR;
        }
    } else if (isDigit(ch)) {
        ans += ch;
        cin.get();
        ch = cin.peek();
        while (isDigit(ch)) {
            cin.get();
            ans += ch;
            ch = cin.peek();
        }
        return INTCON;
    } else {
        ans += ch;
        cin.get();
        if (isPlus(ch)) return PLUS;
        if (isMinus(ch)) return MINU;
        if (isMult(ch)) return MULT;
        if (isDivid(ch)) return DIV;
        if (isLss(ch)) {
            ch = cin.peek();
            if (isAssign(ch)) {
                ans += ch;
                cin.get();
                return LEQ;
            }
            return LSS;
        }
        if (isGre(ch)) {
            ch = cin.peek();
            if (isAssign(ch)) {
                ans += ch;
                cin.get();
                return GEQ;
            }
            return GRE;
        }
        if (isExm(ch)) { //感叹号
            ch = cin.peek();
            if (isAssign(ch)) {
                cin.get();
                ans += ch;
                return NEQ;
            }
        }
        if (isAssign(ch)) {
            ch = cin.peek();
            if (isAssign(ch)) {
                ans += ch;
                cin.get();
                return EQL;
            }
            return ASSIGN;
        }
        if (isSemi(ch)) return SEMICN;
        if (isComma(ch)) return COMMA;
        if (isLparent(ch)) return LPARENT;
        if (isLbrace(ch)) return LBRACE;
        if (isLbrack(ch)) return LBRACK;
        if (isRparent(ch)) return RPARENT;
        if (isRbrace(ch)) return RBRACE;
        if (isRbrack(ch)) return RBRACK;
     }
    return ERROR;
}
