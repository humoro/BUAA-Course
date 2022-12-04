import com.oocourse.uml1.models.elements.UmlInterface;

import java.util.HashMap;

class InterfaceContents {
    private HashMap<UmlInterface,Integer> fatherInterfaces = new HashMap<>();
    private boolean isVisited = false;

    InterfaceContents() {}

    void addFather(UmlInterface umlInterface) {
        this.fatherInterfaces.put(umlInterface,1);
    }

    void addFather(HashMap<UmlInterface,Integer> map) {
        this.fatherInterfaces.putAll(map);
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
