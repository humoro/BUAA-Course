import nl.flotsam.xeger.Xeger;


public class Auto{
    private Auto () {}
    private String producePoly() {
        String poly = "(";
        String TermReg = "(a|((cos[(]((x(\\^[+]?[0-9]+)?)|a)[)])(\\^[+]?[0-9]+)?)|((sin[(]((x(\\^[+]?[0-9]+)?)|a)[)])(\\^[+]?[0-9]+)?)|(x(\\^[+][0-9]+)?)|([+-]?[0-9]+))(\\*(a|((cos[(]((x(\\^[+]?[0-9]+)?)|a)[)])(\\^[+]?[0-9]+)?)|((sin[(]((x(\\^[+]?[0-9]+)?)|a)[)])(\\^[-+]?[0-9]+)?)|(x(\\^[+][0-9]+)?)|([+-]?[0-9]+)))*";
        Xeger Term = new Xeger(TermReg);
        int times = (int)(Math.random()*3+1);
        for(int i = 0;i < times;i++) {
            poly = poly.concat("+" + Term.generate());
        }
        return poly + ")";
    }
    public static void main(String[] args) {
        Auto auto = new Auto();
        for (int i = 0;i < 50;i++) {
            String poly = auto.producePoly();
            while(poly.length() <= 60 && poly.indexOf('a') != -1) {
                poly = poly.replaceAll("a",auto.producePoly());
            }
            poly = poly.replaceAll("a","1");
            System.out.println(poly);
        }
    }
}
