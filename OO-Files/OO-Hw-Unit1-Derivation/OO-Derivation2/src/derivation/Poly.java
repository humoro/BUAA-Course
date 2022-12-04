package derivation;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collections;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

class Poly {
    private ArrayList<Term> item = new ArrayList<>(); // 表达式项字符串形式List
    private  String input;

    private static final BigInteger ONE = new BigInteger("1");

    Poly(String string) {
        this.input = string;
    }

    boolean CheckFormat() { // 检查格式并且提取出符合格式的项
        int index = input.indexOf(',');
        if (index != -1 || input.length() == 0) { // 确保字符串无',' 且不是空串
            return false;
        }
        String input1;
        String regOfHead = "(\\d[ \t]+\\d)|(\\d[ \t]*[xsc])" +
                "|([)x][ \t]*[sxc])|([)x][ \t]*\\d)";
        Pattern patternOfHead = Pattern.compile(regOfHead);
        Matcher matcherOfHead = patternOfHead.matcher(input);
        if (matcherOfHead.find()) {
            return false;
        } // 由于开头项可以无加减号，检查是否除了开头项还有不带加减运算符的项
        String reg1 = "(([ \t]*[+-][ \t]*[+-][ \t]*)|([ \t]*[+-][ \t]*)" +
                "|[ \t]*)(([+-]?\\d+)" +
                "|((x|(cos[ \t]*\\([ \t]*x[ \t]*\\))" +
                "|(sin[ \t]*\\([ \t]*x[ \t]*\\)))" +
                "([ \t]*\\^[ \t]*[+-]?\\d+)?))" +
                "([ \t]*\\*[ \t]*(([+-]?\\d+)" +
                "|((x|(cos[ \t]*\\([ \t]*x[ \t]*\\))" +
                "|(sin[ \t]*\\([ \t]*x[ \t]*\\)))" +
                "([ \t]*\\^[ \t]*[+-]?\\d+)?)))*[ \t]*";
        // 匹配带有系数和正负号的项
        input1 = MatchRightTerm(input, reg1);
        input1 = input1.replace(",", ""); // 去除','
        return (input1.length() == 0);
    }

    private String MatchRightTerm(String string, String reg) { // 提取项
        StringBuilder stringBuilder = new StringBuilder(string);
        Pattern pattern = Pattern.compile(reg);
        Matcher matcher = pattern.matcher(stringBuilder);
        int j;
        while (matcher.find()) {
            this.item.add(new Term(matcher.group()));
            for (j = matcher.start(); j < matcher.end(); j++) {
                stringBuilder.setCharAt(j, ',');
            } // 等字符替换匹配项为','
        }
        return stringBuilder.toString();
    }

    ArrayList<ArrayList<Term>> Derivation() { // 求导
        int size = this.item.size();
        int i;
        ArrayList<ArrayList<Term>> list = new ArrayList<>();
        for (i = 0;i < size;i++) {
            ArrayList<Term> terms = new ArrayList<>(); // 存储求导得出来的项
            Term term = item.get(i); // 当前要求导数的项
            String buffer = term.GetTerm();
            for (int j = 0;j < term.GetList().size();j++) {
                String newDivisor = "";
                Divisor divisor = term.GetList().get(j); // 当前因子
                BigInteger coe;
                coe = term.GetCoe().multiply(term.GetList().get(j).GetPower());
                // 求导系数
                int id = divisor.GetId();
                if (id == 0) {
                    newDivisor += "x^" + divisor.GetPower().
                            subtract(ONE).toString();
                } else if (id == 1) {
                    newDivisor += "sin(x)^" + divisor.GetPower().
                            subtract(ONE).toString() + "*cos(x)";
                } else if (id == -1) {
                    newDivisor += "cos(x)^" + divisor.GetPower().
                            subtract(ONE).toString() + "*-1*sin(x)";
                }
                String newBuffer = buffer.
                        replace(divisor.GetBody(),newDivisor);
                terms.add(new Term(coe.toString() + newBuffer));
                // 把当前因子替换成求导后的因子创建新的项
            }
            list.add(terms);
        }
        return list;
    }

    ArrayList<Term> NoRep(ArrayList<Term> list) {
        Collections.sort(list);
        int end;
        int start = 0;
        while (start < list.size()) {
            end = start + 1;
            while (end < list.size()) {
                if (list.get(start).GetTerm().
                        equals(list.get(end).GetTerm())) {
                    list.get(start).ChangeCoe(list.get(start).GetCoe().
                            add(list.get(end).GetCoe()));
                    list.remove(end);
                }
                else {
                    break;
                }
            }
            start++;
        } // 去除同类项
        return list; // 按系数从高到低排列
    }

    ArrayList<Term> BubbleSortOfCoe(ArrayList<Term> list) {
        int i;
        int j;
        for (i = 0;i < list.size();i++) {
            for (j = 0;j < list.size() - i - 1;j++) {
                if (list.get(j).GetCoe().
                        compareTo(list.get(j + 1).GetCoe()) < 0) {
                    Collections.swap(list,j,j + 1);
                }
            }
        }
        return list; // 按系数从大到小排列
    }

    ArrayList<Term> Simplify1(ArrayList<Term> list) {
        String regCos = "cos\\(x\\)\\^2(\\*)?";
        Pattern pattern = Pattern.compile(regCos);
        for (int i = 0;i < list.size();) {
            Term termI = list.get(i);
            Matcher matcher = pattern.matcher(termI.GetTerm());
            int flag = 0;
            if (matcher.find()) {
                String whole = termI.GetCoe().toString() +
                        termI.GetTerm().replace(matcher.group(),"");
                for (int j = 0;j < list.size();j++) {
                    if (j != i) {
                        Term termJ = list.get(j);
                        String regSin = "sin\\(x\\)\\^2(\\*)?";
                        Pattern pattern1 = Pattern.compile(regSin);
                        Matcher matcher1 = pattern1.matcher(termJ.GetTerm());
                        if (matcher1.find()) {
                            String whole1 = termJ.GetCoe().toString() + termJ.
                                    GetTerm().replace(matcher1.group(),"");
                            if (whole.equals(whole1)) {
                                flag++;
                                Term term = new Term(whole);
                                list.remove(i);
                                if (j > i) {
                                    j--;
                                    list.remove(j);
                                } else {
                                    list.remove(j);
                                    i--;
                                }
                                list.add(term);
                                break;
                            }
                        }
                    }
                }
            }
            if (flag == 0) {
                i++;
            }
        }
        return list;
    }

    ArrayList<Term> Simplify2(ArrayList<Term> list,String reg,int id) {
        Pattern pattern = Pattern.compile(reg);
        for (int i = 0;i < list.size();) {
            Term termI = list.get(i);
            Matcher matcher = pattern.matcher(termI.GetTerm());
            int flag = 0;
            if (matcher.find()) {
                String whole = termI.GetCoe().multiply(
                        new BigInteger("-1")).toString() +
                        termI.GetTerm().replace(matcher.group(),"");
                for (int j = 0;j < list.size();j++) {
                    if (j != i) {
                        Term term = list.get(j);
                        if ((term.GetCoe().toString() + term.GetTerm()).
                                equals(whole)) {
                            flag++;
                            Term newTerm = new Term("");
                            if (id == 1) { // 合并sin
                                newTerm = new Term(whole + "*cos(x)^2");
                            } else if (id == -1) {
                                newTerm = new Term(whole + "*sin(x)^2");
                            }
                            list.add(newTerm);
                            list.remove(i);
                            if (j > i) {
                                list.remove(j - 1);
                            } else {
                                list.remove(j);
                                i--;
                            }
                            break;
                        }
                    }
                }
            }
            if (flag == 0) {
                i++;
            }
        }
        return list;
    }

    void print(ArrayList<Term> list) {
        int i;
        int flag = 0;
        for (i = 0;i < list.size();i++) {
            if (!list.get(i).GetCoe().equals(BigInteger.ZERO)) { //系数为零不会输出

                if (!list.get(i).GetTerm().equals("")) { // 系数不为零且表达式含有因子
                    if (flag != 0 && list.get(i).GetCoe().
                            compareTo(BigInteger.ONE) == 0) {
                        System.out.print("+" + list.get(i).GetTerm());
                    } else if (flag == 0 && list.get(i).GetCoe().
                            compareTo(BigInteger.ONE) == 0) {
                        System.out.print(list.get(i).GetTerm());
                    } else if (list.get(i).GetCoe().
                            compareTo(new BigInteger("-1")) == 0) {
                        System.out.print("-" + list.get(i).GetTerm());
                    } else if (list.get(i).GetCoe().
                            compareTo(BigInteger.ZERO) < 0) {
                        System.out.print(
                                list.get(i).GetCoe().toString()
                                        + "*" + list.get(i).GetTerm());
                    } else if (flag != 0 && list.get(i).GetCoe().
                            compareTo(BigInteger.ZERO) > 0) {
                        System.out.print(
                                "+" + list.get(i).GetCoe().toString()
                                        + "*" + list.get(i).GetTerm());
                    } else if (flag == 0 && list.get(i).GetCoe().
                            compareTo(BigInteger.ZERO) > 0) {
                        System.out.print(list.get(i).GetCoe().toString()
                                + "*" + list.get(i).GetTerm());
                    }
                } else { // 表达式不含因子只有非零系数
                    if (flag == 0 && list.get(i).GetCoe().
                            compareTo(BigInteger.ZERO) > 0) {
                        System.out.print(list.get(i).GetCoe().toString());
                    } else if (flag != 0 && list.get(i).GetCoe().
                            compareTo(BigInteger.ZERO) > 0) {
                        System.out.print("+" + list.get(i).GetCoe().toString());
                    } else {
                        System.out.print(list.get(i).GetCoe().toString());
                    }
                }
                flag++;
            }
        }
        if (i == list.size() && flag == 0) {
            System.out.print("0");
        }
    }
}
