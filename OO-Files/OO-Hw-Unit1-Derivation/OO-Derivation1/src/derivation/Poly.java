package derivation;

import java.math.BigInteger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Poly implements Comparable<Poly> { // 增加排序接口
    private String coe; // 系数
    private String power; // 指数

    public int compareTo(Poly poly) {
        return (new BigInteger(String.valueOf(this.power))
                .compareTo(new BigInteger(String.valueOf(poly.power))));
    } // Collections.sort()排序的排序函数

    private char SplitDerivationOp(String poly) { // 分离出项的符号
        char op;
        StringBuilder oriSymbol = new
                StringBuilder(poly.replaceAll("[ \t]", ""));
        String opReg = "[+-]*";
        Pattern opPattern = Pattern.compile(opReg);
        Matcher opMatcher = opPattern.matcher(oriSymbol);
        StringBuilder symbol = null;
        if (opMatcher.find()) {
            symbol = new StringBuilder(opMatcher.group());
        }
        int i;
        int addCnt = 0;
        int subCnt = 0;
        for (i = 0; symbol != null && i < symbol.length(); i++) {
            if (symbol.charAt(i) == '+') {
                addCnt = addCnt + 1;
            } else if (symbol.charAt(i) == '-') {
                subCnt = subCnt + 1;
            }
        }
        if (subCnt % 2 == 1) {
            op = '-';
        } else {
            op = '+';
        }
        return op;
    }

    private String SplitCoe(String poly, char op) { // 分离系数
        String poly1 = poly.replaceAll("[ \t]", "");
        String conReg = "\\d+";
        String coeReg = "(\\d+\\*)?x";
        Pattern conPattern = Pattern.compile(conReg);
        Pattern coePattern = Pattern.compile(coeReg);
        Matcher conMatch = conPattern.matcher(poly1);
        Matcher coeMatch = coePattern.matcher(poly1);
        if (coeMatch.find()) { // 若是带系数的项
            Pattern conPatternOfCoe = Pattern.compile(conReg); // 从项的前半部分截取系数
            Matcher conMatchOfCoe = conPatternOfCoe.matcher(coeMatch.group());
            if (conMatchOfCoe.find()) {
                return op + conMatchOfCoe.group();
            } else { // 系数为1
                return op + "1";
            }
        } else if (conMatch.find()) { // 常数项
            return op + conMatch.group();
        } else {
            return "0";
        }
    }

    private String SplitPower(String poly) { // 分离指数
        String poly1 = poly.replaceAll("[ \t]", ""); // 去除空白符
        String powerConReg = "[+-]?\\d+";
        String powerReg = "x(\\^[+-]?\\d+)?";
        Pattern powerPattern = Pattern.compile(powerReg);
        Pattern conPatternOfPower = Pattern.compile(powerConReg);
        Matcher powerMatch = powerPattern.matcher(poly1);
        if (powerMatch.find()) { // 找到指数
            Matcher conMatchOfPower = conPatternOfPower
                    .matcher(powerMatch.group());
            if (conMatchOfPower.find()) {
                return conMatchOfPower.group();
            } else {
                return "1";
            }
        } else { // 常数，指数为零
            return "0";
        }
    }

    void SplitNum(String poly) {
        char op = SplitDerivationOp(poly);
        this.coe = SplitCoe(poly, op);
        this.power = SplitPower(poly);
    }

    String GetCoe() {
        return this.coe;
    }

    String GetPower() {
        return this.power;
    }

    void ChangeCoe(String coe) {
        this.coe = coe;
    }

    void ChangePower(String power) {
        this.power = power;
    }
}
