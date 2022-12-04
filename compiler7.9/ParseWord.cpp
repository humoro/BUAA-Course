#include "Type.h"
#include "Token.h"
#include "Parse.h"
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
                if (!isLegalStringCh(ch)) errorHandle(error_a);//出现字符串中不合法的字符
                currentToken += ch;
                in.get();
                ch = in.peek();
            }
            if (isNewLine(ch)) errorHandle(error_a);//遇到了换行符，说明没有匹配右双引号
            else {
                in.get();//吞掉右侧的双引号
            }
            currentTokenType = token_strConstant;
            return;
        }
        if (isSQuo(ch)) {//单引号开始，字符常量
            in.get();
            ch = in.peek();
            char pre = ch;
            while (!isNewLine(ch) && !isSQuo(ch)) {
                if (!isLegalCharactor(ch)) errorHandle(error_a);//单引号内出现不合法字符
                in.get();
                pre = ch;
                ch = in.peek();
            }//记录右单引号或者是行末最后一个字符
            currentToken += pre;
            if (isNewLine(ch)) errorHandle(error_a);//下一个字符不是单引号，即无匹配的右单引号
            else {
                in.get();//是单引号就吞掉符号
            }
            currentTokenType =  token_charConstant;
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
            currentTokenType =  token_constToken;
            return;
        } else if (isInt(currentToken)) {
            currentTokenType =  token_intToken;
            return;
        } else if (isChar(currentToken)) {
            currentTokenType =  token_charToken;
            return;
        } else if (isVoid(currentToken)) {
            currentTokenType =  token_voidToken;
            return;
        } else if (isMain(currentToken)) {
            currentTokenType =  token_mainToken;
            return;
        } else if (isIf(currentToken)) {
            currentTokenType =  token_ifToken;
            return;
        } else if (isElse(currentToken)) {
            currentTokenType =  token_elseToken;
            return;
        } else if (isDo(currentToken)) {
            currentTokenType =  token_doToken;
            return;
        } else if (isWhile(currentToken)) {
            currentTokenType =  token_whileToken;
            return;
        } else if (isFor(currentToken)) {
            currentTokenType =  token_forToken;
            return;
        } else if (isScanf(currentToken)) {
            currentTokenType =  token_scanfToken;
            return;
        } else if (isPrintf(currentToken)) {
            currentTokenType =  token_printfToken;
            return;
        } else if (isReturn(currentToken)) {
            currentTokenType =  token_returnToken;
            return;
        } else {
            currentTokenType =  token_identifier;
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
        currentTokenType =  token_intConstant;
        return;
    } else {
        currentToken += ch;
        in.get();
        if (isPlus(ch)) {
            currentTokenType =  token_plus;//'+'
            return;
        }
        if (isMinus(ch)) {
            currentTokenType =  token_minus;//'-'
            return;
        }
        if (isMult(ch)) {
            currentTokenType =  token_multiply;//'*'
            return;
        }
        if (isDivid(ch)) {
            currentTokenType =  token_divide;//'/'
            return;
        }
        if (isLss(ch)) {//'<'
            ch = in.peek();
            if (isAssign(ch)) {//'<='
                currentToken += ch;
                in.get();
                currentTokenType =  token_lequal;
                return;
            }
            currentTokenType =  token_less;
            return;
        }
        if (isGre(ch)) {//'>'
            ch = in.peek();
            if (isAssign(ch)) {//'>='
                currentToken += ch;
                in.get();
                currentTokenType =  token_gequal;
                return;
            }
            currentTokenType =  token_greater;
            return;
        }
        if (isExm(ch)) { //感叹号
            ch = in.peek();
            if (isAssign(ch)) {//'!='
                in.get();
                currentToken += ch;
                currentTokenType =  token_nequal;
                return;
            }
        }
        if (isAssign(ch)) {//'='
            ch = in.peek();
            if (isAssign(ch)) {//'=='
                currentToken += ch;
                in.get();
                currentTokenType =  token_equal;
                return;
            }
            currentTokenType =  token_assign;
            return;
        }
        if (isSemi(ch)) {
            currentTokenType = token_semicolon;
            return;
        }//';'
        if (isComma(ch)){
            currentTokenType = token_comma;
            return;
        }
        if (isLparent(ch)) {
            currentTokenType = token_lparent;
            return;

        }
        if (isLbrace(ch)) {
            currentTokenType = token_lbrace;
            return;
        }
        if (isLbrack(ch)) {
            currentTokenType = token_lbrack;
            return;
        }
        if (isRparent(ch)) {
            currentTokenType = token_rparent;
            return;
        }
        if (isRbrace(ch)) {
            currentTokenType = token_rbrace;
            return;
        }
        if (isRbrack(ch)) {
            currentTokenType = token_rbrack;
            return;
        }
     }
    currentTokenType = token_error;
    errorHandle(error_a);
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
