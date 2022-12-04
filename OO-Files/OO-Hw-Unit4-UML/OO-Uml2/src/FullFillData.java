import com.oocourse.uml2.models.common.ElementType;
import com.oocourse.uml2.models.elements.UmlAssociation;
import com.oocourse.uml2.models.elements.UmlAssociationEnd;
import com.oocourse.uml2.models.elements.UmlAttribute;
import com.oocourse.uml2.models.elements.UmlClass;
import com.oocourse.uml2.models.elements.UmlClassOrInterface;
import com.oocourse.uml2.models.elements.UmlElement;
import com.oocourse.uml2.models.elements.UmlGeneralization;
import com.oocourse.uml2.models.elements.UmlInteraction;
import com.oocourse.uml2.models.elements.UmlInterface;
import com.oocourse.uml2.models.elements.UmlInterfaceRealization;
import com.oocourse.uml2.models.elements.UmlLifeline;
import com.oocourse.uml2.models.elements.UmlMessage;
import com.oocourse.uml2.models.elements.UmlOperation;
import com.oocourse.uml2.models.elements.UmlStateMachine;
import com.oocourse.uml2.models.elements.UmlTransition;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Set;

class FullFillData {
    private HashMap<String,UmlElement> elementIds;// access element by id
    //followed access all classes with the same name
    private HashMap<UmlClass,ClassContents> classContents;
    private HashMap<UmlInterface,InterfaceContents> interfaceContents;
    private HashMap<UmlStateMachine,StateMachineContents> stateMachineContents;
    private HashMap<UmlInteraction,InteractionContents> interactionContents;
    //followed access operations list of class with class id
    private HashMap<String,ArrayList<UmlOperation>> operations;
    private HashMap<String,ArrayList<UmlAttribute>> attributes;
    private HashMap<String,OperationType> operationTypes;// key is operation id
    private ArrayList<UmlGeneralization> generalizations;
    private ArrayList<UmlAssociation> associations;
    private ArrayList<UmlInterfaceRealization> realizations;
    private HashMap<String,ArrayList<UmlElement>> states;
    private HashMap<String,ArrayList<UmlTransition>> transitions;
    private HashMap<String,ArrayList<UmlMessage>> messages;
    private HashMap<String,ArrayList<UmlLifeline>> lifelines;
    private HashMap<UmlClassOrInterface,TarganData> targanDatas;
    private Set<UmlClassOrInterface> circleExtends;
    private ArrayList<UmlClassOrInterface> stack = new ArrayList<>();
    private int curLoc = 0;

    FullFillData(HashMap<String,UmlElement> elementIds,
                 HashMap<UmlClass,ClassContents> classContents,
                 HashMap<UmlInterface,InterfaceContents> interfaceContents,
                 HashMap<UmlStateMachine,
                         StateMachineContents> stateMachineContents,
                 HashMap<UmlInteraction,
                         InteractionContents> interactionContents,
                 HashMap<String,ArrayList<UmlOperation>> operations,
                 HashMap<String,ArrayList<UmlAttribute>> attributes,
                 HashMap<String,OperationType> operationTypes,
                 ArrayList<UmlGeneralization> generalizations,
                 ArrayList<UmlAssociation> associations,
                 ArrayList<UmlInterfaceRealization> realizations,
                 HashMap<String,ArrayList<UmlElement>> states,
                 HashMap<String,ArrayList<UmlTransition>> transitions,
                 HashMap<String,ArrayList<UmlMessage>> messages,
                 HashMap<String,ArrayList<UmlLifeline>> lifelines,
                 HashMap<UmlClassOrInterface,TarganData> targanDatas,
                 Set<UmlClassOrInterface> circleExtends) {
        this.elementIds = elementIds;
        this.classContents = classContents;
        this.interfaceContents = interfaceContents;
        this.stateMachineContents = stateMachineContents;
        this.interactionContents = interactionContents;
        this.operations = operations;
        this.attributes = attributes;
        this.operationTypes = operationTypes;
        this.generalizations = generalizations;
        this.associations = associations;
        this.realizations = realizations;
        this.states = states;
        this.transitions = transitions;
        this.messages = messages;
        this.lifelines = lifelines;
        this.targanDatas = targanDatas;
        this.circleExtends = circleExtends;
    }

    void fullFillAll() {
        fullFillAttributes();
        fullFillOperation();
        fullFillLifeline();
        fullFillMessage();
        fullFillState();
        fullFillTransition();
        fullFillAssociation();
        fullFillGeneralization();
        if (circleExtendsCheck()) {
            processExtendsInfluence();
        }
    }

    private void fullFillAttributes() {
        for (String curId : this.attributes.keySet()) {
            UmlElement parent = this.elementIds.get(curId);
            if (parent != null &&
                parent.getElementType().equals(ElementType.UML_CLASS)) {
                UmlClass parentClass = (UmlClass) parent;
                ClassContents curContents = this.classContents.get(parentClass);
                ArrayList<UmlAttribute> curList = this.attributes.get(curId);
                for (UmlAttribute attribute : curList) {
                    curContents.addAttribute(attribute);
                }
            }
        }
    }

    private void fullFillOperation() {
        for (String curId : this.operations.keySet()) {
            UmlElement element = this.elementIds.get(curId);
            if (element != null &&
                element.getElementType().equals(ElementType.UML_CLASS)) {
                ArrayList<UmlOperation> curOperations = this.operations.
                                                        get(curId);
                UmlClass curClass = (UmlClass) element;
                OperationType curType;
                for (UmlOperation operation : curOperations) {
                    curType = this.operationTypes.get(operation.getId());
                    this.classContents.get(curClass).
                                       addOperation(operation,curType);
                }
            }
        }
    }

    private void fullFillLifeline() {
        for (String parentId : this.lifelines.keySet()) {
            UmlElement element = this.elementIds.get(parentId);
            if (element.getElementType().equals(ElementType.UML_INTERACTION)) {
                UmlInteraction interaction = (UmlInteraction) element;
                ArrayList<UmlLifeline> list = this.lifelines.get(parentId);
                for (UmlLifeline lifeline : list) {
                    this.interactionContents.get(interaction).
                                             addLifeLine(lifeline);
                }
            }
        }
    }

    private void fullFillMessage() {
        for (String parentId : this.messages.keySet()) {
            UmlElement element = this.elementIds.get(parentId);
            if (element.getElementType().equals(ElementType.UML_INTERACTION)) {
                UmlInteraction interaction = (UmlInteraction) element;
                ArrayList<UmlMessage> messages = this.messages.get(parentId);
                this.interactionContents.get(interaction).addMessages(messages);
            }
        }
    }

    private void fullFillState() {
        for (String parentId : this.states.keySet()) {
            UmlElement region = this.elementIds.get(parentId);
            String machineId = region.getParentId();
            UmlElement element = this.elementIds.get(machineId);
            if (element.getElementType().equals(ElementType.
                                                UML_STATE_MACHINE)) {
                UmlStateMachine machine = (UmlStateMachine) element;
                ArrayList<UmlElement> list = this.states.get(parentId);
                for (UmlElement state : list) {
                    this.stateMachineContents.get(machine).
                                              addState(state);
                }
            }
        }
    }

    private void fullFillTransition() {
        for (String parentId : this.transitions.keySet()) {
            UmlElement region = this.elementIds.get(parentId);
            String machineId = region.getParentId();
            UmlElement element = this.elementIds.get(machineId);
            if (element.getElementType().equals(ElementType.
                                                UML_STATE_MACHINE)) {
                UmlStateMachine machine = (UmlStateMachine) element;
                ArrayList<UmlTransition> transitions = this.transitions.
                                                            get(parentId);
                this.stateMachineContents.get(machine).
                                          addTransitions(transitions);
            }
        }
    }

    private void fullFillAssociation() {
        for (UmlAssociation association : this.associations) {
            String endId1 = association.getEnd1();
            String endId2 = association.getEnd2();
            UmlAssociationEnd end1 = (UmlAssociationEnd)
                                     this.elementIds.get(endId1);
            UmlAssociationEnd end2 = (UmlAssociationEnd)
                                     this.elementIds.get(endId2);
            String elementId1 = end1.getReference();
            String elementId2 = end2.getReference();
            UmlElement element1 = this.elementIds.get(elementId1);
            UmlElement element2 = this.elementIds.get(elementId2);
            ElementType type1 = element1.getElementType();
            ElementType type2 = element2.getElementType();
            if (type1.equals(ElementType.UML_CLASS) &&
                    type2.equals(ElementType.UML_CLASS)) {
                UmlClass umlClass1 = (UmlClass) element1;
                UmlClass umlClass2 = (UmlClass) element2;
                this.classContents.get(umlClass1).
                                   addAssociationClass(umlClass2);
                this.classContents.get(umlClass2).
                                   addAssociationClass(umlClass1);
                this.classContents.get(umlClass1).addAssociationEnd(end2);
                this.classContents.get(umlClass2).addAssociationEnd(end1);
            } else if (type1.equals(ElementType.UML_CLASS)) {
                UmlClass umlClass1 = (UmlClass) element1;
                this.classContents.get(umlClass1).addAssociationCount(1);
                this.classContents.get(umlClass1).addAssociationEnd(end2);
            } else if (type2.equals(ElementType.UML_CLASS)) {
                UmlClass umlClass2 = (UmlClass) element2;
                this.classContents.get(umlClass2).addAssociationCount(1);
                this.classContents.get(umlClass2).addAssociationEnd(end1);
            }
        }
    }

    private void fullFillGeneralization() {
        for (UmlGeneralization generalization : this.generalizations) {
            String sourceId = generalization.getSource();
            String targetId = generalization.getTarget();
            UmlElement sourceElement = this.elementIds.get(sourceId);
            UmlElement targetElement = this.elementIds.get(targetId);
            if (sourceElement.getElementType().equals(ElementType.UML_CLASS) &&
                targetElement.getElementType().equals(ElementType.UML_CLASS)) {
                UmlClass sonClass = (UmlClass) sourceElement;
                UmlClass fatherClass = (UmlClass) targetElement;
                this.classContents.get(sonClass).
                                   setDirectFatherId(fatherClass.getId());
                this.classContents.get(fatherClass).addSonClasses(sonClass);
                this.targanDatas.get(sonClass).addFather(fatherClass);
            } else {
                UmlInterface sonInterface = (UmlInterface) sourceElement;
                UmlInterface fatherInterface = (UmlInterface) targetElement;
                this.interfaceContents.get(sonInterface).
                                       addFather(fatherInterface,1);
                this.targanDatas.get(sonInterface).addFather(fatherInterface);
            }
        }
    }

    private boolean circleExtendsCheck() {
        for (UmlClassOrInterface element : this.targanDatas.keySet()) {
            TarganData curData = this.targanDatas.get(element);
            if (!curData.isLeaf() && curData.getDfnMark() == 0) {
                targanDfs(element);
                this.stack.clear();
                this.curLoc = 0;
            }
        }
        return this.circleExtends.isEmpty();
    }

    private void targanDfs(UmlClassOrInterface curElement) {
        TarganData curData = this.targanDatas.get(curElement);
        curData.setDfnMark(++this.curLoc);
        curData.setLowMark(this.curLoc);
        curData.pushInStack();
        this.stack.add(curElement);
        ArrayList<UmlClassOrInterface> fathers = curData.getFathers();
        for (UmlClassOrInterface father : fathers) {
            TarganData sonData = this.targanDatas.get(father);
            if (sonData.getDfnMark() == 0) {
                targanDfs(father);
                curData.setLowMark(Math.min(curData.getLowMark(),
                                            sonData.getLowMark()));
            } else if (sonData.isInStack()) {
                curData.setLowMark(Math.min(curData.getLowMark(),
                                            sonData.getDfnMark()));
            }
        }
        if (curData.isRoot()) {
            int curIndex = curData.getDfnMark() - 1;
            this.curLoc--;
            if (this.curLoc - curIndex == 0) {
                UmlClassOrInterface cur = this.stack.get(this.curLoc);
                this.targanDatas.get(cur).popOutStack();
                this.stack.remove(curIndex);
                return;
            }
            while (this.curLoc >= curIndex) {
                UmlClassOrInterface cur = this.stack.get(this.curLoc);
                this.circleExtends.add(cur);
                this.targanDatas.get(cur).popOutStack();
                this.stack.remove(this.curLoc--);
            }
            this.curLoc++;
        }
    }

    private void fullFillRealization() {
        for (UmlInterfaceRealization realization : this.realizations) {
            UmlClass sourceClass = (UmlClass) this.elementIds.
                                    get(realization.getSource());
            UmlInterface targetInterface = (UmlInterface) this.elementIds.
                                            get(realization.getTarget());
            this.classContents.get(sourceClass).
                               addRealizationInterface(targetInterface,1);
            HashMap<UmlInterface,Integer> realizations = this.interfaceContents.
                                                          get(targetInterface).
                                                          getFatherInterfaces();
            ClassContents sourceClassContents = this.classContents.
                                                get(sourceClass);
            for (UmlInterface umlInterface : realizations.keySet()) {
                int count = realizations.get(umlInterface);
                sourceClassContents.addRealizationInterface(umlInterface,count);
            }
        }
    }

    private void interfaceDfs(InterfaceContents contents) {
        HashMap<UmlInterface,Integer> curMap = contents.getFatherInterfaces();
        ArrayList<UmlInterface> fathers = new ArrayList<>(curMap.keySet());
        for (UmlInterface umlInterface : fathers) {
            InterfaceContents curContents = interfaceContents.
                                            get(umlInterface);
            if (curContents.isTopInterface()) {
                curContents.setVisited();
                continue;
            }
            if (!curContents.isBeenVisited()) {
                curContents.setVisited();
                interfaceDfs(curContents);
            }
            HashMap<UmlInterface,Integer> interfaces = curContents.
                                                       getFatherInterfaces();
            for (UmlInterface umlInterface1 : interfaces.keySet()) {
                int count = interfaces.get(umlInterface1);
                contents.addFather(umlInterface1,count);
            }
        }
    }

    private void classDfs(ClassContents contents,String topId) {
        ArrayList<UmlClass> sons = contents.getSonClasses();
        ClassContents sonContent;
        HashMap<String,
                ArrayList<UmlAttribute>> curAttributes = contents.
                                                         getExtendsAttributes();
        HashMap<UmlClass,
                Integer> fatherAssociation = contents.getAssociateClass();
        int fatherAssociationCount = contents.getAssociationCount();
        HashMap<UmlInterface,
                Integer> fatherRealization = contents.getInterfaceRealization();
        for (UmlClass sonClass : sons) {
            sonContent = this.classContents.get(sonClass);
            sonContent.setTopClassId(topId);
            sonContent.addAssociationClass(fatherAssociation,
                                           fatherAssociationCount);
            for (UmlInterface umlInterface : fatherRealization.keySet()) {
                int count = fatherRealization.get(umlInterface);
                sonContent.addRealizationInterface(umlInterface,count);
            }
            for (String name : curAttributes.keySet()) {
                ArrayList<UmlAttribute> list = curAttributes.get(name);
                for (UmlAttribute attribute : list) {
                    sonContent.addAttribute(name,attribute);
                }
            }
            if (sonContent.isFather()) {
                classDfs(sonContent,topId);
            }
        }
    }

    private void processExtendsInfluence() {
        for (UmlInterface umlInterface : interfaceContents.keySet()) {
            InterfaceContents contents = interfaceContents.get(umlInterface);
            if (contents.isBeenVisited()) {
                continue;
            }
            contents.setVisited();
            interfaceDfs(contents);
        }
        fullFillRealization();
        for (UmlClass curClass : classContents.keySet()) {
            ClassContents contents = this.classContents.get(curClass);
            if (contents.isTopClass()) {
                String topId = contents.getTopClassId();
                classDfs(contents,topId);
            }
        }
    }
}
