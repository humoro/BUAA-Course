package derivation;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Main {
    private ArrayList<StringBuilder> item = new ArrayList<>(); // 表达式项字符串形式List

    private Main() {
    }

    private boolean CheckFormat(String input) { // 检查格式
        int index = input.indexOf(',');
        if (index != -1 || input.length() == 0) { // 确保字符串无',' 且不是空串
            return false;
        }
        String reg4 = "(\\d+[ \t]+(\\d+|x))|(x[ \t]*x)|(\\d+x)"; // 补充格式检查漏洞
        Pattern pattern4 = Pattern.compile(reg4);
        Matcher matcher4 = pattern4.matcher(input);
        if (matcher4.find()) {
            return false;
        }
        String reg1 = "(([ \t]*[+-][ \t]*[+-])|([ \t]*[+-][ \t]*)|[ \t]*)?" +
                "\\d+[ \t]*\\*[ \t]*x" +
                "([ \t]*\\^[ \t]*[+-]?\\d+)?[ \t]*"; /*优先匹配带有系数和指数的幂函数
                ，再次匹配带有系数无指数幂函数 */
        String reg2 = "(([ \t]*[+-][ \t]*){1,2}|([ \t]*))" +
                "((x([ \t]*\\^[ \t]*)[+-]?\\d+)|x)([ \t]*)"; /*优先匹配有指数无系数幂函数
                ，再次匹配x*/
        String reg3 = "(([ \t]*[+-][ \t]*[+-])|([ \t]*[+-][ \t]*)|[ \t]*)?" +
                "(\\d+)([ \t]*)"; // 匹配常数项
        String input1 = MatchRightPoly(input, reg1);
        String input2 = MatchRightPoly(input1, reg2);
        String input3 = MatchRightPoly(input2, reg3);
        String input4 = input3.replace(",", ""); // 去除','
        return (input4.length() == 0);
    }

    private String MatchRightPoly(String string, String reg) { // 提取项
        StringBuilder stringBuilder = new StringBuilder(string);
        Pattern pattern = Pattern.compile(reg);
        Matcher matcher = pattern.matcher(stringBuilder);
        int j;
        while (matcher.find()) {
            this.item.add(new StringBuilder(matcher.group()));
            for (j = matcher.start(); j < matcher.end(); j++) {
                stringBuilder.setCharAt(j, ',');
            } // 等字符替换匹配项为','
        }
        return stringBuilder.toString();
    }

    private ArrayList<Poly> Rep(ArrayList<Poly> list) { // 合并同类项
        int start;
        int end;
        int size = list.size();
        for (start = 0; start < size - 1;start++) {
            end = start + 1;
            while (start < size - 1 && (
                    new BigInteger(list.get(end).GetPower()).equals(
                            new BigInteger(list.get(start).GetPower())))) {
                list.get(start).ChangeCoe(
                        new BigInteger(list.get(start).GetCoe()).
                                add(new BigInteger(list.get(end).GetCoe())).
                                toString());
                list.remove(end); // 删除同类项
                size--;
            }
        }
        return list;
    }

    private ArrayList<Poly> Derivation(ArrayList<Poly> list) { // 求导
        ArrayList<Poly> derivationList = new ArrayList<>();
        int i;
        for (i = 0; i < list.size(); i++) {
            Poly poly = new Poly();
            poly.ChangeCoe(new BigInteger(list.get(i).GetCoe()).
                    multiply(new BigInteger(list.get(i).GetPower())).
                    toString());
            poly.ChangePower(new BigInteger(list.get(i).GetPower()).
                    subtract(new BigInteger("1")).toString());
            derivationList.add(poly);
        }
        return derivationList;
    }

    private void Print(ArrayList<Poly> list) {
        int i;
        int flag = 0; // 记录当前打印的非零项
        for (i = 0; i < list.size(); i++) {
            if (new BigInteger(list.get(i).GetCoe()).
                    compareTo(new BigInteger("0")) == 0
                    && i == list.size() - 1 && flag == 0) {
                System.out.println("0"); // 计算结果无非零项，打印0
            } else if (new BigInteger(list.get(i).GetCoe()).
                    compareTo(new BigInteger("0")) != 0) {
                if (flag != 0 && (new BigInteger(list.get(i).GetCoe()).
                        compareTo(new BigInteger("1")) > 0)) {
                    System.out.print("+" + list.get(i).GetCoe());
                } else if (flag != 0 && (new BigInteger(list.get(i).GetCoe()).
                        compareTo(new BigInteger("1")) == 0)) {
                    System.out.print("+");
                } else {
                    System.out.print(list.get(i).GetCoe());
                }
                if (new BigInteger(list.get(i).GetPower()).
                        compareTo(new BigInteger("0")) != 0) {
                    if (new BigInteger(list.get(i).GetPower()).
                            compareTo(new BigInteger("1")) != 0) {
                        System.out.print("*x^" + list.get(i).GetPower());
                    } else {
                        System.out.print("*x");
                    }
                }
                flag++;
            }
        }
    }

    public static void main(String[] args) {
        Scanner scan = new Scanner(System.in);
        if (scan.hasNextLine()) {
            String string = scan.nextLine();
            Main main = new Main();
            boolean isRightFormat = main.CheckFormat(string);
            if (!isRightFormat) {
                System.out.print("WRONG FORMAT!");
            } else {
                ArrayList<Poly> derList = new ArrayList<>();
                int j;
                for (j = 0; j < main.item.size(); j++) {
                    Poly poly = new Poly();
                    poly.SplitNum(new String(main.item.get(j)));
                    derList.add(poly); // 构建项的ArrayList
                }
                Collections.sort(derList); // 按指数大小升序排列
                ArrayList<Poly> noRepList = main.Rep(derList);
                ArrayList<Poly> derivationList = main.Derivation(noRepList);
                main.Print(derivationList);
            }
        } else {
            System.out.println("WRONG FORMAT!");
        }
    }
}
