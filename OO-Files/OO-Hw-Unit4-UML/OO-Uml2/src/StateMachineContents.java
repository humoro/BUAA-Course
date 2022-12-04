import com.oocourse.uml2.interact.exceptions.user.StateDuplicatedException;
import com.oocourse.uml2.interact.exceptions.user.StateNotFoundException;
import com.oocourse.uml2.models.elements.UmlElement;
import com.oocourse.uml2.models.elements.UmlState;
import com.oocourse.uml2.models.elements.UmlTransition;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

class StateMachineContents {
    private String selfName;
    private boolean isRefreshSubStates = false;
    private Set<SubSequentState> list = new HashSet<>();
    private HashMap<String,ArrayList<UmlState>> states = new HashMap<>();
    private HashMap<String,SubSequentState> subSequentStates = new HashMap<>();
    private Set<UmlTransition> transitions = new HashSet<>();

    StateMachineContents(String name) {
        this.selfName = name;
    }

    void addState(UmlElement element) {
        this.subSequentStates.put(element.getId(),
                                  new SubSequentState(element.getId()));
        String name = element.getName();
        if (name != null) {
            if (!this.states.containsKey(name)) {
                this.states.put(name,new ArrayList<>());
            }
            this.states.get(name).add((UmlState) element);
        }
    }

    void addTransitions(ArrayList<UmlTransition> list) {
        this.transitions.addAll(list);
    }

    int getStateCount() {
        return this.subSequentStates.size();
    }

    int getTransitionCount() {
        return this.transitions.size();
    }

    int getSubsequentCount(String stateName)
                    throws StateNotFoundException,
                           StateDuplicatedException {
        if (!this.states.containsKey(stateName)) {
            throw new StateNotFoundException(this.selfName,stateName);
        }
        if (this.states.get(stateName).size() > 1) {
            throw new StateDuplicatedException(this.selfName,stateName);
        }
        if (!this.isRefreshSubStates) {
            fullFillTransition();
            this.isRefreshSubStates = true;
        }
        UmlState state = this.states.get(stateName).get(0);
        if (this.subSequentStates.get(state.getId()).isCalculate() ||
            this.subSequentStates.get(state.getId()).isLeafState()) {
            return this.subSequentStates.get(state.getId()).getSubStatesCount();
        } else {
            this.subSequentStates.get(state.getId()).setCalculate();
            subStateDfs(this.subSequentStates.get(state.getId()),state.getId());
        }
        for (SubSequentState subSequentState : list) {
            subSequentState.setUnVisited();
        }
        list.clear();
        return this.subSequentStates.get(state.getId()).getSubStatesCount();
    }

    private void subStateDfs(SubSequentState sequentState,String curId) {
        this.list.add(sequentState);
        Set<String> list = sequentState.getSubStates();
        ArrayList<String> dfsList = new ArrayList<>(list);
        for (String subStateId : dfsList) {
            if (this.subSequentStates.get(subStateId).isLeafState()) {
                continue;
            }
            if (!this.subSequentStates.get(subStateId).isCalculate() &&
                !this.subSequentStates.get(subStateId).isVisited()) {
                this.subSequentStates.get(subStateId).setVisited();
                subStateDfs(this.subSequentStates.get(subStateId),subStateId);
            }
            Set<String> subStateIdOfCurElement = this.
                        subSequentStates.get(subStateId).getSubStates();
            for (String umlElementId : subStateIdOfCurElement) {
                this.subSequentStates.get(curId).addSubState(umlElementId);
            }
        }
    }

    private void fullFillTransition() {
        for (UmlTransition transition : this.transitions) {
            String sourceId = transition.getSource();
            String targetId = transition.getTarget();
            SubSequentState sourceSubSequentStates;
            sourceSubSequentStates = this.subSequentStates.get(sourceId);
            sourceSubSequentStates.addSubState(targetId);
        }
    }
}
