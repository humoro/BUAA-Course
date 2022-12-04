package derivation;

import java.math.BigInteger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

class Divisor {
    private String base; // 因子的底
    private BigInteger power; // 因子的指数
    private String body;
    private int id;
    /*
    id:0 多项式
    -1 cos
    1 sin
    */

    Divisor(String string) {
        SplitPower(string);
        SplitBase(string);
        Identify(this.base);
        UpdateBody();
    }

    private void SplitBase(String body) {
        String reg = "cos\\(x\\)|sin\\(x\\)|x";
        Pattern patternOfBase = Pattern.compile(reg);
        Matcher matcherOfBase = patternOfBase.matcher(body);
        if (matcherOfBase.find()) {
            this.base = matcherOfBase.group();
        }
    }

    private void SplitPower(String body) {
        String reg = "[+-]?\\d+";
        Pattern patternOfPower = Pattern.compile(reg);
        Matcher matcherOfPower = patternOfPower.matcher(body);
        if (matcherOfPower.find()) {
            this.power = new BigInteger(matcherOfPower.group());
        } else {
            this.power = new BigInteger("1");
        }
    }

    private void Identify(String base) {
        this.id = 0;
        if (base.equals("cos(x)")) {
            this.id = -1;
        } else if (base.equals("sin(x)")) {
            this.id = 1;
        }
    }

    private void UpdateBody() {
        this.body = this.base + "^" + this.power.toString();
    }

    BigInteger GetPower() {
        return this.power;
    }

    void ChangePower(BigInteger power) {
        this.power = power;
        UpdateBody();
    }

    int GetId() {
        return this.id;
    }

    String GetBody() {
        return this.body;
    }
}
