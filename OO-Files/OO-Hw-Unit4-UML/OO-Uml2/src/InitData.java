import com.oocourse.uml2.models.common.Direction;
import com.oocourse.uml2.models.common.ElementType;
import com.oocourse.uml2.models.elements.UmlAssociation;
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
import com.oocourse.uml2.models.elements.UmlParameter;
import com.oocourse.uml2.models.elements.UmlStateMachine;
import com.oocourse.uml2.models.elements.UmlTransition;
import java.util.ArrayList;
import java.util.HashMap;

class InitData {
    private HashMap<String,UmlElement> elementIds;// access element by id
    //followed access all classes with the same name
    private HashMap<String,ArrayList<UmlClass>> classNames;
    private HashMap<UmlClass,ClassContents> classContents;
    private HashMap<UmlInterface,InterfaceContents> interfaceContents;
    private HashMap<String,ArrayList<UmlStateMachine>> stateMachineNames;
    private HashMap<UmlStateMachine,StateMachineContents> stateMachineContents;
    private HashMap<String,ArrayList<UmlInteraction>> interactionNames;
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

    InitData(HashMap<String,UmlElement> elementIds,
            HashMap<String,ArrayList<UmlClass>> classNames,
            HashMap<UmlClass,ClassContents> classContents,
            HashMap<UmlInterface,InterfaceContents> interfaceContents,
            HashMap<String,ArrayList<UmlStateMachine>> stateMachineNames,
            HashMap<UmlStateMachine,StateMachineContents> stateMachineContents,
            HashMap<String,ArrayList<UmlInteraction>> interactionNames,
            HashMap<UmlInteraction,InteractionContents> interactionContents,
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
            HashMap<UmlClassOrInterface,TarganData> targanDatas) {
        this.elementIds = elementIds;
        this.classNames = classNames;
        this.classContents = classContents;
        this.interfaceContents = interfaceContents;
        this.stateMachineNames = stateMachineNames;
        this.stateMachineContents = stateMachineContents;
        this.interactionNames = interactionNames;
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
    }

    void initAll(UmlElement...elements) {
        for (UmlElement element : elements) {
            String id = element.getId();
            this.elementIds.put(id,element);
            ElementType type = element.getElementType();
            if (type.equals(ElementType.UML_CLASS)) {
                initClasses((UmlClass) element);
            } else if (type.equals(ElementType.UML_INTERFACE)) {
                initInterface((UmlInterface) element);
            } else if (type.equals(ElementType.UML_STATE_MACHINE)) {
                initStateMachine((UmlStateMachine) element);
            } else if (type.equals(ElementType.UML_INTERACTION)) {
                initInteraction((UmlInteraction) element);
            } else if (type.equals(ElementType.UML_ATTRIBUTE)) {
                initAttributes((UmlAttribute) element);
            } else if (type.equals(ElementType.UML_OPERATION)) {
                initOperations((UmlOperation) element);
            } else if (type.equals(ElementType.UML_PARAMETER)) {
                initParameters((UmlParameter) element);
            } else if (type.equals(ElementType.UML_INTERFACE_REALIZATION)) {
                initRealizations((UmlInterfaceRealization) element);
            } else if (type.equals(ElementType.UML_GENERALIZATION)) {
                initGeneralization((UmlGeneralization) element);
            } else if (type.equals(ElementType.UML_ASSOCIATION)) {
                initAssociation((UmlAssociation) element);
            } else if (type.equals(ElementType.UML_STATE) ||
                       type.equals(ElementType.UML_PSEUDOSTATE) ||
                       type.equals(ElementType.UML_FINAL_STATE)) {
                initState(element);
            } else if (type.equals(ElementType.UML_TRANSITION)) {
                initTransition((UmlTransition) element);
            } else if (type.equals(ElementType.UML_LIFELINE)) {
                initLifeline((UmlLifeline) element);
            } else if (type.equals(ElementType.UML_MESSAGE)) {
                initMessage((UmlMessage) element);
            }
        }
    }

    private void initClasses(UmlClass umlClass) {
        String name = umlClass.getName();
        String id = umlClass.getId();
        if (!this.classNames.containsKey(name)) {
            ArrayList<UmlClass> list = new ArrayList<>();
            this.classNames.put(name,list);
        }
        this.classNames.get(name).add(umlClass);
        this.classContents.put(umlClass,new ClassContents(name,id));
        this.targanDatas.put(umlClass,new TarganData());
    }

    private void initInterface(UmlInterface umlInterface) {
        this.interfaceContents.put(umlInterface,
                                   new InterfaceContents());
        this.targanDatas.put(umlInterface,new TarganData());
    }

    private void initStateMachine(UmlStateMachine machine) {
        String name = machine.getName();
        if (!this.stateMachineNames.containsKey(name)) {
            ArrayList<UmlStateMachine> list = new ArrayList<>();
            this.stateMachineNames.put(name,list);
        }
        this.stateMachineNames.get(name).add(machine);
        this.stateMachineContents.put(machine,new StateMachineContents(name));
    }

    private void initInteraction(UmlInteraction interaction) {
        String name = interaction.getName();
        if (!this.interactionNames.containsKey(name)) {
            ArrayList<UmlInteraction> list = new ArrayList<>();
            this.interactionNames.put(name,list);
        }
        this.interactionNames.get(name).add(interaction);
        this.interactionContents.put(interaction,new InteractionContents(name));
    }

    private void initAttributes(UmlAttribute attribute) {
        String parentId = attribute.getParentId();
        if (!this.attributes.containsKey(parentId)) {
            this.attributes.put(parentId,new ArrayList<>());
        }
        this.attributes.get(parentId).add(attribute);
    }

    private void initOperations(UmlOperation operation) {
        String parentId = operation.getParentId();
        if (!this.operations.containsKey(parentId)) {
            ArrayList<UmlOperation> list = new ArrayList<>();
            this.operations.put(parentId,list);
        }
        this.operations.get(parentId).add(operation);
        String id = operation.getId();
        if (!this.operationTypes.containsKey(id)) {
            this.operationTypes.put(id,new OperationType());
        }
    }

    private void initParameters(UmlParameter parameter) {
        String parentId = parameter.getParentId();
        if (!this.operationTypes.containsKey(parentId)) {
            OperationType type = new OperationType();
            this.operationTypes.put(parentId,type);
        }
        if (!parameter.getDirection().equals(Direction.RETURN)) {
            this.operationTypes.get(parentId).setHasParameter();
        } else {
            this.operationTypes.get(parentId).setReturnType();
        }
    }

    private void initRealizations(UmlInterfaceRealization realization) {
        this.realizations.add(realization);
    }

    private void initGeneralization(UmlGeneralization generalization) {
        this.generalizations.add(generalization);
    }

    private void initAssociation(UmlAssociation association) {
        this.associations.add(association);
    }

    private void initState(UmlElement state) {
        String parentId = state.getParentId();
        if (!this.states.containsKey(parentId)) {
            this.states.put(parentId,new ArrayList<>());
        }
        this.states.get(parentId).add(state);
    }

    private void initTransition(UmlTransition transition) {
        String parentId = transition.getParentId();
        if (!this.transitions.containsKey(parentId)) {
            this.transitions.put(parentId,new ArrayList<>());
        }
        this.transitions.get(parentId).add(transition);
    }

    private void initLifeline(UmlLifeline lifeline) {
        String parentId = lifeline.getParentId();
        if (!this.lifelines.containsKey(parentId)) {
            this.lifelines.put(parentId,new ArrayList<>());
        }
        this.lifelines.get(parentId).add(lifeline);
    }

    private void initMessage(UmlMessage message) {
        String parentId = message.getParentId();
        if (!this.messages.containsKey(parentId)) {
            this.messages.put(parentId,new ArrayList<>());
        }
        this.messages.get(parentId).add(message);
    }

}
