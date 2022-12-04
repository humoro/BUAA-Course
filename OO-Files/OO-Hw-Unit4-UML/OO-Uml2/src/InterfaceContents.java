
import com.oocourse.uml2.models.elements.UmlInterface;

import java.util.HashMap;

class InterfaceContents {
    private HashMap<UmlInterface,Integer> fatherInterfaces = new HashMap<>();
    private boolean isMultiExtends = false;
    private boolean isVisited = false;

    InterfaceContents() {}

    void addFather(UmlInterface umlInterface,int count) {
        int time = count;
        if (this.fatherInterfaces.containsKey(umlInterface)) {
            time = this.fatherInterfaces.get(umlInterface) + count;
        }
        if (time > 1) {
            this.isMultiExtends = true;
        }
        this.fatherInterfaces.put(umlInterface,time);
    }

    boolean isMultiExtends() {
        return this.isMultiExtends;
    }

    boolean isTopInterface() {
        return this.fatherInterfaces.size() == 0;
    }

    HashMap<UmlInterface,Integer> getFatherInterfaces() {
        return this.fatherInterfaces;
    }

    void setVisited() {
        this.isVisited = true;
    }

    boolean isBeenVisited() {
        return this.isVisited;
    }
}
