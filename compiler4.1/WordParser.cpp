//�������������и���ﲢ���ж����������,��ǰ�ʷ�������ֻ������û���ķ�����ĳ���
#include "Type.h"
#include "Token.h"
#include "Error.h"
#include<iostream>
using namespace std;
extern long long linenum;
tokenType wordParser(string& ans) {
    ans.clear();
    char ch = cin.peek();
    while (isSpace(ch)) {
        cin.get();
		if (isNewLine(ch)) {
			linenum++;
		}
        ch = cin.peek();
    }//�̵������Ʊ���Ϳո�
    if (isAlpha(ch) || isDQuo(ch) || isSQuo(ch)) {
        if (isDQuo(ch)) {//�����˫���ſ�ʼ,˵�����ַ�������
            cin.get();
            ch = cin.peek();
            while (!isDQuo(ch) && !isNewLine(ch)) {
                if (!isLegalStringCh(ch)) error(a,linenum);//�����ַ����в��Ϸ����ַ�
                ans += ch;
                cin.get();
                ch = cin.peek();
            }
            if (isNewLine(ch)) error(a,linenum);//�����˻��з���˵��û��ƥ����˫����
            else {
                cin.get();//�̵��Ҳ��˫����
            }
            return STRCON;
        }
        if (isSQuo(ch)) {//�����ſ�ʼ���ַ�����
            cin.get();
            ch = cin.peek();
            char pre = ch;
            while (!isNewLine(ch) && !isSQuo(ch)) {
                if (!isLegalCharactor(ch)) error(a,linenum);//�������ڳ��ֲ��Ϸ��ַ�
                cin.get();
                pre = ch;
                ch = cin.peek();
            }//��¼�ҵ����Ż�������ĩ���һ���ַ�
            ans += pre;
            if (isNewLine(ch)) error(a,linenum);//��һ���ַ����ǵ����ţ�����ƥ����ҵ�����
            else {
                cin.get();//�ǵ����ž��̵�����
            }
            return CHARCON;
        }
        ans += ch;
        cin.get();
        ch = cin.peek();
        while(isAlpha(ch) || isDigit(ch)) {
            ans += ch;
            cin.get();
            ch = cin.peek();
        }
        //��������֤��ǰ��õ��ַ����Ƿ�Ϊ������
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
        if (isPlus(ch)) return PLUS;//'+'
        if (isMinus(ch)) return MINU;//'-'
        if (isMult(ch)) return MULT;//'*'
        if (isDivid(ch)) return DIV;//'/'
        if (isLss(ch)) {//'<'
            ch = cin.peek();
            if (isAssign(ch)) {//'<='
                ans += ch;
                cin.get();
                return LEQ;
            }
            return LSS;
        }
        if (isGre(ch)) {//'>'
            ch = cin.peek();
            if (isAssign(ch)) {//'>='
                ans += ch;
                cin.get();
                return GEQ;
            }
            return GRE;
        }
        if (isExm(ch)) { //��̾��
            ch = cin.peek();
            if (isAssign(ch)) {//'!='
                cin.get();
                ans += ch;
                return NEQ;
            }
        }
        if (isAssign(ch)) {//'='
            ch = cin.peek();
            if (isAssign(ch)) {//'=='
                ans += ch;
                cin.get();
                return EQL;
            }
            return ASSIGN;
        }
        if (isSemi(ch)) return SEMICN;//';'
        if (isComma(ch)) return COMMA;
        if (isLparent(ch)) return LPARENT;
        if (isLbrace(ch)) return LBRACE;
        if (isLbrack(ch)) return LBRACK;
        if (isRparent(ch)) return RPARENT;
        if (isRbrace(ch)) return RBRACE;
        if (isRbrack(ch)) return RBRACK;
     }
    error(a,linenum);
    return wordParser(ans);//
}
