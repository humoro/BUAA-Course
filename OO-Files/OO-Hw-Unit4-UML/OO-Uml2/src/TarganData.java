import com.oocourse.uml2.models.elements.UmlClassOrInterface;

import java.util.ArrayList;

class TarganData {
    private ArrayList<UmlClassOrInterface> fathers = new ArrayList<>();
    private boolean isInStack = false;
    private int dfnMark = 0;
    private int lowMark = 301;

    void addFather(UmlClassOrInterface element) {
        this.fathers.add(element);
    }

    void setDfnMark(int mark) {
        this.dfnMark = mark;
    }

    void setLowMark(int mark) {
        this.lowMark = mark;
    }

    void pushInStack() {
        this.isInStack = true;
    }

    void popOutStack() {
        this.isInStack = false;
    }

    int getDfnMark() {
        return this.dfnMark;
    }

    int getLowMark() {
        return this.lowMark;
    }

    boolean isRoot() {
        return this.dfnMark == this.lowMark;
    }

    boolean isInStack() {
        return this.isInStack;
    }

    boolean isLeaf() {
        return this.fathers.isEmpty();
    }

    ArrayList<UmlClassOrInterface> getFathers() {
        return this.fathers;
    }
}
