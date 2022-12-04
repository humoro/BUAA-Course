package derivation;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

class Term implements Comparable<Term> {
    private BigInteger coe;
    private String term; // 项的主体
    private int[] size = new int[3]; // 三种因子的个数
    private ArrayList<Divisor> divisor = new ArrayList<>();//因子

    Term(String term) {
        String string;
        this.coe = BigInteger.ONE;
        string = SplitDivisor(term);
        SplitTermCoe(string);
        MergeSameBase();
        UpdateTerm();
    }

    public int compareTo(Term term) {
        return (term.term).compareTo(this.term);
    }

    private String  SplitDivisor(String term) { // 分离因子
        String termWithoutSpace = term.replaceAll("[ \t]", "");
        String regOfCos = "cos\\(x\\)(\\^[+-]?\\d+)?"; // 余弦函数因子
        termWithoutSpace = BuildList(termWithoutSpace, regOfCos, 0);
        String regOfSin = "sin\\(x\\)(\\^[+-]?\\d+)?"; // 正弦函数因子
        termWithoutSpace = BuildList(termWithoutSpace, regOfSin, 1);
        String regOfX = "x(\\^[+-]?\\d+)?"; // 多项式因子
        termWithoutSpace = BuildList(termWithoutSpace, regOfX, 2);
        return termWithoutSpace;
    }

    private String BuildList(String term, String reg, int id) {
        int end = 0;
        StringBuilder stringBuilder = new StringBuilder(term);
        Pattern pattern = Pattern.compile(reg);
        Matcher matcher = pattern.matcher(stringBuilder);
        while (matcher.find()) {
            end++;
            Divisor divisor = new Divisor(matcher.group());
            this.divisor.add(divisor);
            int j;
            for (j = matcher.start(); j < matcher.end(); j++) {
                stringBuilder.setCharAt(j, ',');
            }
        }
        this.size[id] = end;
        return stringBuilder.toString();
    }

    private void  SplitTermCoe(String term) { // 分离出项的符号
        StringBuilder stringBuilder = new StringBuilder(term);
        String regOfCon = "[+-]?\\d+";
        Pattern pattern = Pattern.compile(regOfCon);
        Matcher matcher = pattern.matcher(stringBuilder);
        while (matcher.find()) {
            this.coe = this.coe.multiply(new BigInteger(matcher.group()));
            int start;
            for (start = matcher.start();start < matcher.end();start++) {
                stringBuilder.setCharAt(start,',');
            }
        }
        String reg = "[+-]*";
        Pattern pattern1 = Pattern.compile(reg);
        Matcher matcher1 = pattern1.matcher(stringBuilder);
        while (matcher1.find()) {
            StringBuilder ops = new StringBuilder(matcher1.group());
            int i;
            for (i = 0;i < ops.length();i++) {
                if (ops.charAt(i) == '-') {
                    this.coe = this.coe.multiply(new BigInteger("-1"));
                }
            }
        }
    }

    private void MergeSameBase() { // 合并同底的因子
        int i;
        int j;
        int total = 3; // 因子类型的个数默认是三种
        int current = 0;
        for (i = 0; i < total && current < total; current++) {
            if (this.size[current] > 0) {
                BigInteger power = this.divisor.get(i).GetPower();
                for (j = i + 1; j < i + this.size[current]; ) {
                    power = power.add(this.divisor.get(j).GetPower());
                    this.divisor.remove(j);
                    this.size[current]--;
                }
                boolean exist = !power.equals(new BigInteger("0"));
                if (exist) {
                    this.divisor.get(i).ChangePower(power);
                    i++;
                } else {
                    this.divisor.remove(i);
                    total--;
                }
            }
        }
    }

    private void UpdateTerm() {
        int i;
        this.term = "";
        for (i = 0; i < this.divisor.size(); i++) {
            if (i == 0) {
                this.term = this.term.
                        concat(this.divisor.get(i).GetBody());
            } else {
                this.term = this.term.
                        concat("*" + this.divisor.get(i).GetBody());
            }
        }
    }

    ArrayList<Divisor> GetList() {
        return this.divisor;
    }

    BigInteger GetCoe() {
        return this.coe;
    }

    String GetTerm() {
        return this.term;
    }

    void ChangeCoe(BigInteger coe) {
        this.coe = coe;
    }
}
