//�������������и���ﲢ���ж����������,��ǰ�ʷ�������ֻ������û���ķ�����ĳ���
#include<fstream>
#include "Types.h"
using namespace std;
myType wordAnalysis(ifstream& in,string& ans) {
    ans.clear();
    char ch = in.peek();
    while (isSpace(ch)) {
        in.get();
        ch = in.peek();
    }//�̵������Ʊ���Ϳո�
    if (isAlpha(ch) || isULine(ch) || isDQuo(ch) || isSQuo(ch)) {
        if (isDQuo(ch)) {//�����˫���ſ�ʼ,˵�����ַ�������
            in.get();
            ch = in.peek();
            while (!isDQuo(ch)) {
                ans += ch;
                in.get();
                ch = in.peek();
            }
            in.get();//���ұߵ�˫�����̵�
            return STRCON;
        }
        if (isSQuo(ch)) {//�����ſ�ʼ���ַ�����
            in.get();
            ch = in.get();
            ans += ch;
            in.get();//���ұߵĵ������̵�
            return CHARCON;
        }
        ans += ch;
        in.get();
        ch = in.peek();
        while(isULine(ch) || isAlpha(ch) || isDigit(ch)) {
            ans += ch;
            in.get();
            ch = in.peek();
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
        in.get();
        ch = in.peek();
        while (isDigit(ch)) {
            in.get();
            ans += ch;
            ch = in.peek();
        }
        return INTCON;
    } else {
        ans += ch;
        in.get();
        if (isPlus(ch)) return PLUS;
        if (isMinus(ch)) return MINU;
        if (isMult(ch)) return MULT;
        if (isDivid(ch)) return DIV;
        if (isLss(ch)) {
            ch = in.peek();
            if (isAssign(ch)) {
                ans += ch;
                in.get();
                return LEQ;
            }
            return LSS;
        }
        if (isGre(ch)) {
            ch = in.peek();
            if (isAssign(ch)) {
                ans += ch;
                in.get();
                return GEQ;
            }
            return GRE;
        }
        if (isExm(ch)) { //��̾��
            ch = in.peek();
            if (isAssign(ch)) {
                in.get();
                ans += ch;
                return NEQ;
            }
        }
        if (isAssign(ch)) {
            ch = in.peek();
            if (isAssign(ch)) {
                ans += ch;
                in.get();
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
