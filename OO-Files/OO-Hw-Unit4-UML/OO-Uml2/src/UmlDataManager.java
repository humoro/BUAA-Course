import com.oocourse.uml2.interact.common.AttributeClassInformation;
import com.oocourse.uml2.interact.exceptions.user.ClassDuplicatedException;
import com.oocourse.uml2.interact.exceptions.user.ClassNotFoundException;
import com.oocourse.uml2.interact.exceptions.user.InteractionDuplicatedException;
import com.oocourse.uml2.interact.exceptions.user.InteractionNotFoundException;
import com.oocourse.uml2.interact.exceptions.user.StateMachineDuplicatedException;
import com.oocourse.uml2.interact.exceptions.user.StateMachineNotFoundException;
import com.oocourse.uml2.interact.exceptions.user.UmlRule002Exception;
import com.oocourse.uml2.interact.exceptions.user.UmlRule008Exception;
import com.oocourse.uml2.interact.exceptions.user.UmlRule009Exception;
import com.oocourse.uml2.models.elements.UmlClass;
import com.oocourse.uml2.models.elements.UmlClassOrInterface;
import com.oocourse.uml2.models.elements.UmlElement;
import com.oocourse.uml2.models.elements.UmlInteraction;
import com.oocourse.uml2.models.elements.UmlInterface;
import com.oocourse.uml2.models.elements.UmlStateMachine;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

class UmlDataManager {
    private HashMap<String,UmlElement> elementIds;
    private HashMap<String,ArrayList<UmlClass>> classNames;
    private HashMap<UmlClass,ClassContents> classContents;
    private HashMap<UmlInterface,InterfaceContents> interfaceContents;
    private HashMap<String,ArrayList<UmlStateMachine>> stateMachineNames;
    private HashMap<UmlStateMachine,StateMachineContents> stateMachineContents;
    private HashMap<String,ArrayList<UmlInteraction>> interactionNames;
    private HashMap<UmlInteraction,InteractionContents> interactionContents;
    private Set<UmlClassOrInterface> circleExtends;

    UmlDataManager(HashMap<String,UmlElement> elementIds,
                   HashMap<String,ArrayList<UmlClass>> classNames,
                   HashMap<UmlClass,ClassContents> classContents,
                   HashMap<UmlInterface,InterfaceContents> interfaceContents,
                   HashMap<String,ArrayList<UmlStateMachine>> stateMachineNames,
                   HashMap<UmlStateMachine,
                           StateMachineContents> stateMachineContents,
                   HashMap<String,ArrayList<UmlInteraction>> interactionNames,
                   HashMap<UmlInteraction,
                           InteractionContents> interactionContents,
                   Set<UmlClassOrInterface> circleExtends) {
        this.classNames = classNames;
        this.elementIds = elementIds;
        this.classContents = classContents;
        this.interfaceContents = interfaceContents;
        this.stateMachineNames = stateMachineNames;
        this.stateMachineContents = stateMachineContents;
        this.interactionNames = interactionNames;
        this.interactionContents = interactionContents;
        this.circleExtends = circleExtends;
    }

    UmlClass getClassByName(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        if (!this.classNames.containsKey(className)) {
            throw new ClassNotFoundException(className);
        }
        if (this.classNames.get(className).size() > 1) {
            throw new ClassDuplicatedException(className);
        }
        return this.classNames.get(className).get(0);
    }

    ClassContents getClassContents(UmlClass targetClass) {
        return this.classContents.get(targetClass);
    }

    int getClassCount() {
        return this.classContents.size();
    }

    String getClassNameById(String id) {
        return this.elementIds.get(id).getName();
    }

    UmlStateMachine getStateMachineByName(String machineName)
                                   throws StateMachineNotFoundException,
                                          StateMachineDuplicatedException {
        if (!this.stateMachineNames.containsKey(machineName)) {
            throw new StateMachineNotFoundException(machineName);
        }
        if (this.stateMachineNames.get(machineName).size() > 1) {
            throw new StateMachineDuplicatedException(machineName);
        }
        return this.stateMachineNames.get(machineName).get(0);
    }

    StateMachineContents getStateMachineContents(UmlStateMachine machine) {
        return this.stateMachineContents.get(machine);
    }

    UmlInteraction getInteractionByName(String interactionName)
                                 throws InteractionNotFoundException,
                                        InteractionDuplicatedException {
        if (!this.interactionNames.containsKey(interactionName)) {
            throw new InteractionNotFoundException(interactionName);
        }
        if (this.interactionNames.get(interactionName).size() > 1) {
            throw new InteractionDuplicatedException(interactionName);
        }
        return this.interactionNames.get(interactionName).get(0);
    }

    InteractionContents getInteractionContents(UmlInteraction interaction) {
        return this.interactionContents.get(interaction);
    }

    void checkUml002() throws UmlRule002Exception {
        Set<AttributeClassInformation> list = new HashSet<>();
        for (UmlClass umlClass : this.classContents.keySet()) {
            String className = umlClass.getName();
            ClassContents contents = this.classContents.get(umlClass);
            HashMap<String,Integer> r002Name = contents.getSelfR002Name();
            for (String name : r002Name.keySet()) {
                if (r002Name.get(name) > 1) {
                    list.add(new AttributeClassInformation(name,className));
                }
            }
        }
        if (!list.isEmpty()) {
            throw new UmlRule002Exception(list);
        }
    }

    void checkUml008() throws UmlRule008Exception {
        if (this.circleExtends.size() != 0) {
            throw new UmlRule008Exception(this.circleExtends);
        }
    }

    void checkUml009() throws UmlRule009Exception {
        Set<UmlClassOrInterface> list = new HashSet<>();
        for (UmlClass umlClass : this.classContents.keySet()) {
            if (this.classContents.get(umlClass).isMultiInherent()) {
                list.add(umlClass);
            }
        }
        for (UmlInterface umlInterface : this.interfaceContents.keySet()) {
            if (this.interfaceContents.get(umlInterface).isMultiExtends()) {
                list.add(umlInterface);
            }
        }
        if (!list.isEmpty()) {
            throw new UmlRule009Exception(list);
        }
    }
}
