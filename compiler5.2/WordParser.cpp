#include "Type.h"
#include "Token.h"
#include "Parser.h"
using namespace std;
void WordParser::wordParser() {//词法分析函数
    currentToken.clear();
    char ch = in.peek();
    while (isSpace(ch)) {
        in.get();
		if (isNewLine(ch)) {
			lineNumber++;
		}
        ch = in.peek();
    }//吞掉换行制表符和空格
    if (isAlpha(ch) || isDQuo(ch) || isSQuo(ch)) {
        if (isDQuo(ch)) {//如果是双引号开始,说明是字符串常量
            in.get();
            ch = in.peek();
            while (!isDQuo(ch) && !isNewLine(ch)) {
                if (!isLegalStringCh(ch)) errorHandle(a);//出现字符串中不合法的字符
                currentToken += ch;
                in.get();
                ch = in.peek();
            }
            if (isNewLine(ch)) errorHandle(a);//遇到了换行符，说明没有匹配右双引号
            else {
                in.get();//吞掉右侧的双引号
            }
            currentTokenType = STRCON;
            return;
        }
        if (isSQuo(ch)) {//单引号开始，字符常量
            in.get();
            ch = in.peek();
            char pre = ch;
            while (!isNewLine(ch) && !isSQuo(ch)) {
                if (!isLegalCharactor(ch)) errorHandle(a);//单引号内出现不合法字符
                in.get();
                pre = ch;
                ch = in.peek();
            }//记录右单引号或者是行末最后一个字符
            currentToken += pre;
            if (isNewLine(ch)) errorHandle(a);//下一个字符不是单引号，即无匹配的右单引号
            else {
                in.get();//是单引号就吞掉符号
            }
            currentTokenType =  CHARCON;
            return;
        }
        currentToken += ch;
        in.get();
        ch = in.peek();
        while(isAlpha(ch) || isDigit(ch)) {
            currentToken += ch;
            in.get();
            ch = in.peek();
        }
        //下面是验证当前获得的字符串是否为保留字
        if (isConst(currentToken)) {
            currentTokenType =  CONSTTK;
            return;
        } else if (isInt(currentToken)) {
            currentTokenType =  INTTK;
            return;
        } else if (isChar(currentToken)) {
            currentTokenType =  CHARTK;
            return;
        } else if (isVoid(currentToken)) {
            currentTokenType =  VOIDTK;
            return;
        } else if (isMain(currentToken)) {
            currentTokenType =  MAINTK;
            return;
        } else if (isIf(currentToken)) {
            currentTokenType =  IFTK;
            return;
        } else if (isElse(currentToken)) {
            currentTokenType =  ELSETK;
            return;
        } else if (isDo(currentToken)) {
            currentTokenType =  DOTK;
            return;
        } else if (isWhile(currentToken)) {
            currentTokenType =  WHILETK;
            return;
        } else if (isFor(currentToken)) {
            currentTokenType =  FORTK;
            return;
        } else if (isScanf(currentToken)) {
            currentTokenType =  SCANFTK;
            return;
        } else if (isPrintf(currentToken)) {
            currentTokenType =  PRINTFTK;
            return;
        } else if (isReturn(currentToken)) {
            currentTokenType =  RETURNTK;
            return;
        } else {
            currentTokenType =  IDENFR;
        }
        return;
    } else if (isDigit(ch)) {
        currentToken += ch;
        in.get();
        ch = in.peek();
        while (isDigit(ch)) {
            in.get();
            currentToken += ch;
            ch = in.peek();
        }
        currentTokenType =  INTCON;
        return;
    } else {
        currentToken += ch;
        in.get();
        if (isPlus(ch)) {
            currentTokenType =  PLUS;//'+'
            return;
        }
        if (isMinus(ch)) {
            currentTokenType =  MINU;//'-'
            return;
        }
        if (isMult(ch)) {
            currentTokenType =  MULT;//'*'
            return;
        }
        if (isDivid(ch)) {
            currentTokenType =  DIV;//'/'
            return;
        }
        if (isLss(ch)) {//'<'
            ch = in.peek();
            if (isAssign(ch)) {//'<='
                currentToken += ch;
                in.get();
                currentTokenType =  LEQ;
                return;
            }
            currentTokenType =  LSS;
            return;
        }
        if (isGre(ch)) {//'>'
            ch = in.peek();
            if (isAssign(ch)) {//'>='
                currentToken += ch;
                in.get();
                currentTokenType =  GEQ;
                return;
            }
            currentTokenType =  GRE;
            return;
        }
        if (isExm(ch)) { //感叹号
            ch = in.peek();
            if (isAssign(ch)) {//'!='
                in.get();
                currentToken += ch;
                currentTokenType =  NEQ;
                return;
            }
        }
        if (isAssign(ch)) {//'='
            ch = in.peek();
            if (isAssign(ch)) {//'=='
                currentToken += ch;
                in.get();
                currentTokenType =  EQL;
                return;
            }
            currentTokenType =  ASSIGN;
            return;
        }
        if (isSemi(ch)) {
            currentTokenType = SEMICN;
            return;
        }//';'
        if (isComma(ch)){
            currentTokenType = COMMA;
            return;
        }
        if (isLparent(ch)) {
            currentTokenType = LPARENT;
            return;

        }
        if (isLbrace(ch)) {
            currentTokenType = LBRACE;
            return;
        }
        if (isLbrack(ch)) {
            currentTokenType = LBRACK;
            return;
        }
        if (isRparent(ch)) {
            currentTokenType = RPARENT;
            return;
        }
        if (isRbrace(ch)) {
            currentTokenType = RBRACE;
            return;
        }
        if (isRbrack(ch)) {
            currentTokenType = RBRACK;
            return;
        }
     }
    currentTokenType = ERROR;
    errorHandle(a);
    wordParser();//
}

void WordParser::callBackStreamPos(ParsePosition* ps) {//文件读取指针的回退函数
    in.seekg(ps->getStreamPos());
    lineNumber = ps->getLineNumber();
}
void WordParser::flushStreamPos(ParsePosition* ps) {//获取当前文件指针位置和行数
    ps->setStreamPos(in.tellg());
    ps->setLineNumber(lineNumber);
}
