import java.util.HashSet;
import java.util.Set;

class SubSequentState {
    private boolean isCalculate = false;
    private boolean isHasSelf = false;
    private boolean isLeafState = true;
    private boolean isVisited = false;
    private String selfId;
    private Set<String> subStates = new HashSet<>();

    SubSequentState(String selfId) {
        this.selfId = selfId;
    }

    int getSubStatesCount() {
        int count = 0;
        if (this.isHasSelf) {
            count++;
        }
        return this.subStates.size() + count;
    }

    boolean isCalculate() {
        return this.isCalculate;
    }

    boolean isVisited() {
        return this.isVisited;
    }

    boolean isLeafState() {
        return this.isLeafState;
    }

    Set<String> getSubStates() {
        return this.subStates;
    }

    void addSubState(String subStateId) {
        if (!this.selfId.equals(subStateId)) {
            this.subStates.add(subStateId);
            this.isLeafState = false;
        } else {
            this.isHasSelf = true;
        }
    }

    void setVisited() {
        this.isVisited = true;
    }

    void setUnVisited() {
        this.isVisited = false;
    }

    void setCalculate() {
        this.isCalculate = true;
    }
}
