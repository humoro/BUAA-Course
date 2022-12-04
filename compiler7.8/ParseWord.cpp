#include "Type.h"
#include "Token.h"
#include "Parse.h"
using namespace std;
void WordParser::wordParser() {//�ʷ���������
    currentToken.clear();
    char ch = in.peek();
    while (isSpace(ch)) {
        in.get();
		if (isNewLine(ch)) {
			lineNumber++;
		}
        ch = in.peek();
    }//�̵������Ʊ���Ϳո�
    if (isAlpha(ch) || isDQuo(ch) || isSQuo(ch)) {
        if (isDQuo(ch)) {//�����˫���ſ�ʼ,˵�����ַ�������
            in.get();
            ch = in.peek();
            while (!isDQuo(ch) && !isNewLine(ch)) {
                if (!isLegalStringCh(ch)) errorHandle(error_a);//�����ַ����в��Ϸ����ַ�
                currentToken += ch;
                in.get();
                ch = in.peek();
            }
            if (isNewLine(ch)) errorHandle(error_a);//�����˻��з���˵��û��ƥ����˫����
            else {
                in.get();//�̵��Ҳ��˫����
            }
            currentTokenType = token_strConstant;
            return;
        }
        if (isSQuo(ch)) {//�����ſ�ʼ���ַ�����
            in.get();
            ch = in.peek();
            char pre = ch;
            while (!isNewLine(ch) && !isSQuo(ch)) {
                if (!isLegalCharactor(ch)) errorHandle(error_a);//�������ڳ��ֲ��Ϸ��ַ�
                in.get();
                pre = ch;
                ch = in.peek();
            }//��¼�ҵ����Ż�������ĩ���һ���ַ�
            currentToken += pre;
            if (isNewLine(ch)) errorHandle(error_a);//��һ���ַ����ǵ����ţ�����ƥ����ҵ�����
            else {
                in.get();//�ǵ����ž��̵�����
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
        //��������֤��ǰ��õ��ַ����Ƿ�Ϊ������
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
        if (isExm(ch)) { //��̾��
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

void WordParser::callBackStreamPos(ParsePosition* ps) {//�ļ���ȡָ��Ļ��˺���
    in.seekg(ps->getStreamPos());
    lineNumber = ps->getLineNumber();
}
void WordParser::flushStreamPos(ParsePosition* ps) {//��ȡ��ǰ�ļ�ָ��λ�ú�����
    ps->setStreamPos(in.tellg());
    ps->setLineNumber(lineNumber);
}
