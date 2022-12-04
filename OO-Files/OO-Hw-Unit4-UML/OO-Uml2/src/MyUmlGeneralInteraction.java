import com.oocourse.uml2.interact.common.AttributeClassInformation;
import com.oocourse.uml2.interact.common.AttributeQueryType;
import com.oocourse.uml2.interact.common.OperationQueryType;
import com.oocourse.uml2.interact.exceptions.user.AttributeDuplicatedException;
import com.oocourse.uml2.interact.exceptions.user.AttributeNotFoundException;
import com.oocourse.uml2.interact.exceptions.user.ClassDuplicatedException;
import com.oocourse.uml2.interact.exceptions.user.ClassNotFoundException;
import com.oocourse.uml2.interact.exceptions.user.InteractionDuplicatedException;
import com.oocourse.uml2.interact.exceptions.user.InteractionNotFoundException;
import com.oocourse.uml2.interact.exceptions.user.LifelineDuplicatedException;
import com.oocourse.uml2.interact.exceptions.user.LifelineNotFoundException;
import com.oocourse.uml2.interact.exceptions.user.StateDuplicatedException;
import com.oocourse.uml2.interact.exceptions.user.StateMachineDuplicatedException;
import com.oocourse.uml2.interact.exceptions.user.StateMachineNotFoundException;
import com.oocourse.uml2.interact.exceptions.user.StateNotFoundException;
import com.oocourse.uml2.interact.exceptions.user.UmlRule002Exception;
import com.oocourse.uml2.interact.exceptions.user.UmlRule008Exception;
import com.oocourse.uml2.interact.exceptions.user.UmlRule009Exception;
import com.oocourse.uml2.interact.format.UmlGeneralInteraction;
import com.oocourse.uml2.models.common.Visibility;
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
import com.oocourse.uml2.models.elements.UmlStateMachine;
import com.oocourse.uml2.models.elements.UmlTransition;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class MyUmlGeneralInteraction implements UmlGeneralInteraction {
    private InitData initData;
    private FullFillData fullFillData;
    private UmlDataManager dataManager;

    public MyUmlGeneralInteraction(UmlElement... elements) {
        CreatManager();
        this.initData.initAll(elements);
        this.fullFillData.fullFillAll();
    }
    //-----------------------------private method-----------------------------//

    private void CreatManager() {
        HashMap<String,UmlElement> elementIds = new HashMap<>();
        //followed access all classes with the same name
        HashMap<String,ArrayList<UmlClass>> classNames = new HashMap<>();
        HashMap<UmlClass,ClassContents> classContents = new HashMap<>();
        HashMap<UmlInterface,
                InterfaceContents> interfaceContents = new HashMap<>();
        HashMap<String,
                ArrayList<UmlStateMachine>> stateMachineNames = new HashMap<>();
        HashMap<UmlStateMachine,
                StateMachineContents> stateMachineContents = new HashMap<>();
        HashMap<String,
                ArrayList<UmlInteraction>> interactionNames = new HashMap<>();
        HashMap<UmlInteraction,
                InteractionContents> interactionContents = new HashMap<>();
        HashMap<String,ArrayList<UmlOperation>> operations = new HashMap<>();
        HashMap<String,ArrayList<UmlAttribute>> attributes = new HashMap<>();
        HashMap<String,OperationType> operationTypes = new HashMap<>();
        ArrayList<UmlGeneralization> generalizations = new ArrayList<>();
        ArrayList<UmlAssociation> associations = new ArrayList<>();
        ArrayList<UmlInterfaceRealization> realizations = new ArrayList<>();
        HashMap<String,ArrayList<UmlElement>> states = new HashMap<>();
        HashMap<String,ArrayList<UmlTransition>> transitions = new HashMap<>();
        HashMap<String,ArrayList<UmlMessage>> messages = new HashMap<>();
        HashMap<String,ArrayList<UmlLifeline>> lifelines = new HashMap<>();
        HashMap<UmlClassOrInterface,TarganData> targanDatas = new HashMap<>();
        Set<UmlClassOrInterface> circleExtends = new HashSet<>();
        this.initData = new InitData(elementIds,classNames,classContents,
                                    interfaceContents,stateMachineNames,
                                    stateMachineContents,interactionNames,
                                    interactionContents,operations,attributes,
                                    operationTypes,generalizations,associations,
                                    realizations,states,
                                    transitions,messages,lifelines,targanDatas);
        this.fullFillData = new FullFillData(elementIds,classContents,
                                            interfaceContents,
                                            stateMachineContents,
                                            interactionContents,
                                            operations,attributes,
                                            operationTypes,generalizations,
                                            associations,realizations,states,
                                            transitions,messages,lifelines,
                                            targanDatas,circleExtends);
        this.dataManager = new UmlDataManager(elementIds,classNames,
                                              classContents,interfaceContents,
                                              stateMachineNames,
                                              stateMachineContents,
                                              interactionNames,
                                              interactionContents,
                                              circleExtends);
    }
    //-----------------------UmlStandardPreCheck----------------------------//

    public void checkForUml002() throws UmlRule002Exception {
        this.dataManager.checkUml002();
    }

    public void checkForUml008() throws UmlRule008Exception {
        this.dataManager.checkUml008();
    }

    public void checkForUml009() throws UmlRule009Exception {
        this.dataManager.checkUml009();
    }
    //--------------------------ClassModelInteraction-------------------------//

    @Override
    public int getClassCount() {
        return this.dataManager.getClassCount();
    }

    @Override
    public int getClassAttributeCount(String className,
                                      AttributeQueryType queryType)
                               throws ClassNotFoundException,
                                      ClassDuplicatedException {
        UmlClass curClass = this.dataManager.getClassByName(className);
        return this.dataManager.getClassContents(curClass).
                                getAttributeCount(queryType);
    }

    @Override
    public int getClassOperationCount(String className,
                                      OperationQueryType queryType)
                               throws ClassNotFoundException,
                                      ClassDuplicatedException {
        UmlClass curClass = this.dataManager.getClassByName(className);
        return this.dataManager.getClassContents(curClass).
                                getOperationCount(queryType);
    }

    @Override
    public Visibility getClassAttributeVisibility(String className,
                                                  String attributeName)
                                           throws ClassNotFoundException,
                                                  ClassDuplicatedException,
                                                  AttributeNotFoundException,
                                                  AttributeDuplicatedException {
        UmlClass curClass = this.dataManager.getClassByName(className);
        return this.dataManager.getClassContents(curClass).
                                getAttributeVisibility(attributeName);
    }

    @Override
    public Map<Visibility,
            Integer> getClassOperationVisibility(String className,
                                                 String operationName)
                                          throws ClassNotFoundException,
                                                 ClassDuplicatedException {
        UmlClass curClass = this.dataManager.getClassByName(className);
        return this.dataManager.getClassContents(curClass).
                                getOperationVisibility(operationName);
    }

    @Override
    public List<AttributeClassInformation> getInformationNotHidden(
                                                   String className)
                                            throws ClassNotFoundException,
                                                   ClassDuplicatedException {
        UmlClass curClass = this.dataManager.getClassByName(className);
        return this.dataManager.getClassContents(curClass).
                                getAttributesClassInformation();
    }

    @Override
    public int getClassAssociationCount(String className)
                                 throws ClassNotFoundException,
                                        ClassDuplicatedException {
        UmlClass curClass = this.dataManager.getClassByName(className);
        return this.dataManager.getClassContents(curClass).
                                getAssociationCount();
    }

    @Override
    public List<String> getClassAssociatedClassList(String className)
                                             throws ClassNotFoundException,
                                                    ClassDuplicatedException {
        UmlClass curClass = this.dataManager.getClassByName(className);
        return this.dataManager.getClassContents(curClass).
                                getAssociationList();
    }

    @Override
    public String getTopParentClass(String className)
                             throws ClassNotFoundException,
                                    ClassDuplicatedException {
        UmlClass curClass = this.dataManager.getClassByName(className);
        String id = this.dataManager.getClassContents(curClass).
                                     getTopClassId();
        return this.dataManager.getClassNameById(id);
    }

    @Override
    public List<String> getImplementInterfaceList(String className)
                                           throws ClassNotFoundException,
                                                  ClassDuplicatedException {
        UmlClass curClass = this.dataManager.getClassByName(className);
        return this.dataManager.getClassContents(curClass).
                                getRealizationList();
    }
    //------------------------UmlCollaborationInteraction---------------------//

    public int getParticipantCount(String interactionName)
                            throws InteractionNotFoundException,
                                   InteractionDuplicatedException {
        UmlInteraction curInteraction = this.dataManager.
                                        getInteractionByName(interactionName);
        InteractionContents contents = this.dataManager.
                                        getInteractionContents(curInteraction);
        return contents.getLifelineCount();
    }

    public int getMessageCount(String interactionName)
                        throws InteractionNotFoundException,
                               InteractionDuplicatedException {
        UmlInteraction curInteraction = this.dataManager.
                                        getInteractionByName(interactionName);
        InteractionContents contents = this.dataManager.
                                        getInteractionContents(curInteraction);
        return contents.getMessageCount();
    }

    public int getIncomingMessageCount(String interactionName,
                                       String lifelineName)
                                throws InteractionNotFoundException,
                                       InteractionDuplicatedException,
                                       LifelineNotFoundException,
                                       LifelineDuplicatedException {
        UmlInteraction curInteraction = this.dataManager.
                                        getInteractionByName(interactionName);
        InteractionContents contents = this.dataManager.
                                        getInteractionContents(curInteraction);
        return contents.getIncomingCount(lifelineName);
    }
    //---------------------------UmlStateChartInteraction---------------------//

    public int getStateCount(String stateMachineName)
                      throws StateMachineNotFoundException,
                             StateMachineDuplicatedException {
        UmlStateMachine machine = this.dataManager.
                                  getStateMachineByName(stateMachineName);
        StateMachineContents contents = this.dataManager.
                                        getStateMachineContents(machine);
        return contents.getStateCount();
    }

    public int getTransitionCount(String stateMachineName)
                           throws StateMachineNotFoundException,
                                  StateMachineDuplicatedException {

        UmlStateMachine machine = this.dataManager.
                                  getStateMachineByName(stateMachineName);
        StateMachineContents contents = this.dataManager.
                                        getStateMachineContents(machine);
        return contents.getTransitionCount();
    }

    public int getSubsequentStateCount(String stateMachineName,String stateName)
                                throws StateMachineNotFoundException,
                                       StateMachineDuplicatedException,
                                       StateNotFoundException,
                                       StateDuplicatedException {
        UmlStateMachine machine = this.dataManager.
                                  getStateMachineByName(stateMachineName);
        StateMachineContents contents = this.dataManager.
                                        getStateMachineContents(machine);
        return contents.getSubsequentCount(stateName);
    }
}
