#include "Type.h"
#include "Token.h"
#include "Parser.h"
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
                if (!isLegalStringCh(ch)) errorHandle(a);//�����ַ����в��Ϸ����ַ�
                currentToken += ch;
                in.get();
                ch = in.peek();
            }
            if (isNewLine(ch)) errorHandle(a);//�����˻��з���˵��û��ƥ����˫����
            else {
                in.get();//�̵��Ҳ��˫����
            }
            currentTokenType = STRCON;
            return;
        }
        if (isSQuo(ch)) {//�����ſ�ʼ���ַ�����
            in.get();
            ch = in.peek();
            char pre = ch;
            while (!isNewLine(ch) && !isSQuo(ch)) {
                if (!isLegalCharactor(ch)) errorHandle(a);//�������ڳ��ֲ��Ϸ��ַ�
                in.get();
                pre = ch;
                ch = in.peek();
            }//��¼�ҵ����Ż�������ĩ���һ���ַ�
            currentToken += pre;
            if (isNewLine(ch)) errorHandle(a);//��һ���ַ����ǵ����ţ�����ƥ����ҵ�����
            else {
                in.get();//�ǵ����ž��̵�����
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
        //��������֤��ǰ��õ��ַ����Ƿ�Ϊ������
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
        if (isExm(ch)) { //��̾��
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

void WordParser::callBackStreamPos(ParsePosition* ps) {//�ļ���ȡָ��Ļ��˺���
    in.seekg(ps->getStreamPos());
    lineNumber = ps->getLineNumber();
}
void WordParser::flushStreamPos(ParsePosition* ps) {//��ȡ��ǰ�ļ�ָ��λ�ú�����
    ps->setStreamPos(in.tellg());
    ps->setLineNumber(lineNumber);
}
