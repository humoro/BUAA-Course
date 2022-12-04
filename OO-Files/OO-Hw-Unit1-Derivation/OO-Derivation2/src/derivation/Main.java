package derivation;

import java.util.ArrayList;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scan = new Scanner(System.in);
        if (scan.hasNextLine()) {
            String string = scan.nextLine();
            Poly poly = new Poly(string);
            boolean isRightFormat = poly.CheckFormat();
            if (!isRightFormat) {
                System.out.print("WRONG FORMAT!");
            } else {
                ArrayList<ArrayList<Term>> derivation = poly.Derivation();
                ArrayList<Term> terms = new ArrayList<>();
                int i;
                for (i = 0;i < derivation.size();i++) {
                    int j;
                    for (j = 0;j < derivation.get(i).size();j++) {
                        terms.add(derivation.get(i).get(j));
                    }
                } // 提取出List中的所有项
                ArrayList<Term> terms1 = poly.NoRep(terms);
                terms = poly.Simplify1(terms1);
                terms = poly.NoRep(terms);
                terms = poly.Simplify2(terms,"cos\\(x\\)\\^2(\\*)?",-1);
                terms = poly.NoRep(terms);
                terms = poly.Simplify2(terms,"sin\\(x\\)\\^2(\\*)?",1);
                terms = poly.NoRep(terms);
                terms = poly.BubbleSortOfCoe(terms);
                poly.print(terms);
            }
        } else {
            System.out.println("WRONG FORMAT!");
        }
    }
}
