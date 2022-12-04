import com.oocourse.uml1.interact.common.AttributeClassInformation;
import com.oocourse.uml1.interact.common.AttributeQueryType;
import com.oocourse.uml1.interact.common.OperationQueryType;
import com.oocourse.uml1.interact.exceptions.user.AttributeDuplicatedException;
import com.oocourse.uml1.interact.exceptions.user.AttributeNotFoundException;
import com.oocourse.uml1.interact.exceptions.user.ClassDuplicatedException;
import com.oocourse.uml1.interact.exceptions.user.ClassNotFoundException;
import com.oocourse.uml1.interact.format.UmlInteraction;
import com.oocourse.uml1.models.common.Direction;
import com.oocourse.uml1.models.common.ElementType;
import com.oocourse.uml1.models.common.Visibility;
import com.oocourse.uml1.models.elements.UmlAssociation;
import com.oocourse.uml1.models.elements.UmlAssociationEnd;
import com.oocourse.uml1.models.elements.UmlAttribute;
import com.oocourse.uml1.models.elements.UmlClass;
import com.oocourse.uml1.models.elements.UmlElement;
import com.oocourse.uml1.models.elements.UmlGeneralization;
import com.oocourse.uml1.models.elements.UmlInterface;
import com.oocourse.uml1.models.elements.UmlInterfaceRealization;
import com.oocourse.uml1.models.elements.UmlOperation;
import com.oocourse.uml1.models.elements.UmlParameter;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MyUmlInteraction implements UmlInteraction {
    //-------------------------container data structure-----------------------//
    private HashMap<String,ArrayList<UmlClass>> classNames = new HashMap<>();
    //------------------access element content with class id------------------//
    private HashMap<String,UmlElement> elementIds = new HashMap<>();
    //---------------------access class content with class--------------------//
    private HashMap<UmlClass,ClassContents> classContents = new HashMap<>();
    private HashMap<UmlInterface,
                    InterfaceContents> interfaceContents = new HashMap<>();
    //-------------------access operation list with class id------------------//
    private HashMap<String,
            ArrayList<UmlOperation>> operations = new HashMap<>();
    //--------------access operationType list with operation id---------------//
    private ArrayList<UmlAttribute> attributes = new ArrayList<>();

    private HashMap<String,OperationType> operationTypes = new HashMap<>();

    private ArrayList<UmlGeneralization> generalizations = new ArrayList<>();

    private ArrayList<UmlAssociation> associations = new ArrayList<>();

    private ArrayList<UmlInterfaceRealization> realizations = new ArrayList<>();

    public MyUmlInteraction(UmlElement... elements) {
        initialContainer(elements);
        fullFillContainer();
    }

    @Override
    public int getClassCount() {
        return this.classContents.size();
    }

    @Override
    public int getClassAttributeCount(
            String className,AttributeQueryType queryType)
            throws ClassNotFoundException, ClassDuplicatedException {
        UmlClass curClass = getClassByName(className);
        return this.classContents.get(curClass).getAttributeCount(queryType);
    }

    @Override
    public int getClassOperationCount(
            String className,OperationQueryType queryType)
            throws ClassNotFoundException, ClassDuplicatedException {
        UmlClass curClass = getClassByName(className);
        return this.classContents.get(curClass).getOperationCount(queryType);
    }

    @Override
    public Visibility getClassAttributeVisibility(
            String className,String attributeName)
            throws ClassNotFoundException, ClassDuplicatedException,
            AttributeNotFoundException, AttributeDuplicatedException {
        UmlClass curClass = getClassByName(className);
        return this.classContents.get(curClass).
                getAttributeVisibility(attributeName);
    }

    @Override
    public Map<Visibility, Integer> getClassOperationVisibility(
            String className,String operationName)
            throws ClassNotFoundException, ClassDuplicatedException {
        UmlClass curClass = getClassByName(className);
        return this.classContents.get(curClass).
                getOperationVisibility(operationName);
    }

    @Override
    public List<AttributeClassInformation> getInformationNotHidden(
            String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        UmlClass curClass = getClassByName(className);
        return this.classContents.get(curClass).
                getAttributesClassInformation();
    }

    @Override
    public int getClassAssociationCount(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        UmlClass curClass = getClassByName(className);
        return this.classContents.get(curClass).getAssociationCount();
    }

    @Override
    public List<String> getClassAssociatedClassList(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        UmlClass curClass = getClassByName(className);
        return this.classContents.get(curClass).getAssociationList();
    }

    @Override
    public String getTopParentClass(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        UmlClass curClass = getClassByName(className);
        String id = this.classContents.get(curClass).getTopClassId();
        return this.elementIds.get(id).getName();
    }

    @Override
    public List<String> getImplementInterfaceList(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        UmlClass curClass = getClassByName(className);
        return this.classContents.get(curClass).getRealizationList();
    }

    //-----------------------------private method-----------------------------//
    private void initialContainer(UmlElement...elements) {
        for (UmlElement element : elements) {
            String id = element.getId();
            this.elementIds.put(id,element);
            ElementType type = element.getElementType();
            if (type.equals(ElementType.UML_CLASS)) {
                initClasses((UmlClass) element);
            } else if (type.equals(ElementType.UML_INTERFACE)) {
                initInterface((UmlInterface) element);
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
            }
        }

    }

    private void fullFillContainer() {
        fullFillAttributes();
        fullFillOperation();
        fullFillAssociation();
        fullFillGeneralization();
        processExtendsInfluence();
    }

    private void fullFillAttributes() {
        for (UmlAttribute attribute : this.attributes) {
            String parentId = attribute.getParentId();
            UmlElement parent = this.elementIds.get(parentId);
            if (parent.getElementType().equals(ElementType.UML_CLASS)) {
                UmlClass parentClass = (UmlClass) parent;
                this.classContents.get(parentClass).
                        addAttribute(attribute);
            }
        }
    }

    private void fullFillOperation() {
        for (String curId : this.operations.keySet()) {
            UmlElement element = this.elementIds.get(curId);
            if (element.getElementType().equals(ElementType.UML_CLASS)) {
                ArrayList<UmlOperation> curOperations =
                        this.operations.get(curId);
                UmlClass curClass = (UmlClass) element;
                OperationType curType;
                for (UmlOperation operation : curOperations) {
                    curType = this.operationTypes.get(operation.getId());
                    this.classContents.get(curClass).addOperation(operation,
                            curType);
                }
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
            } else if (type1.equals(ElementType.UML_CLASS)) {
                UmlClass umlClass1 = (UmlClass) element1;
                this.classContents.get(umlClass1).addAssociationCount(1);
            } else if (type2.equals(ElementType.UML_CLASS)) {
                UmlClass umlClass2 = (UmlClass) element2;
                this.classContents.get(umlClass2).addAssociationCount(1);
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
            } else {
                UmlInterface sonInterface = (UmlInterface) sourceElement;
                UmlInterface fatherInterface = (UmlInterface) targetElement;
                this.interfaceContents.get(sonInterface).
                                        addFather(fatherInterface);
            }
        }
    }

    private void fullFillRealization() {
        for (UmlInterfaceRealization realization : this.realizations) {
            UmlClass sourceClass = (UmlClass) this.elementIds.
                                                get(realization.getSource());
            UmlInterface targetInterface = (UmlInterface) this.elementIds.
                                                get(realization.getTarget());
            this.classContents.get(sourceClass).
                                addRealizationInterface(targetInterface);
            this.classContents.get(sourceClass).
                                addRealizationInterface(this.interfaceContents.
                                                        get(targetInterface).
                                                        getFatherInterfaces());
        }
    }

    private void interfaceDfs(InterfaceContents contents) {
        HashMap<UmlInterface,Integer> curMap = contents.getFatherInterfaces();
        ArrayList<UmlInterface> fathers = new ArrayList<>(curMap.keySet());
        for (UmlInterface umlInterface : fathers) {
            InterfaceContents curContents = interfaceContents.
                                                get(umlInterface);
            if (curContents.isTopInterface()) {
                contents.addFather(umlInterface);
                continue;
            }
            if (!curContents.isBeenVisited()) {
                curContents.setVisited();
                interfaceDfs(curContents);
            }
            contents.addFather(curContents.getFatherInterfaces());
        }
    }

    private void classDfs(ClassContents contents,String topId) {
        ArrayList<UmlClass> sons = contents.getSonClasses();
        ClassContents curContent;
        HashMap<String,ArrayList<UmlAttribute>> curAttributes = contents.
                                                        getExtendsAttributes();
        HashMap<UmlClass,Integer> fatherAssociation = contents.
                                                        getAssociateClass();
        int fatherAssociationCount = contents.getAssociationCount();
        HashMap<UmlInterface,Integer> fatherRealization = contents.
                                                getInterfaceRealization();
        for (UmlClass sonClass : sons) {
            curContent = this.classContents.get(sonClass);
            curContent.setTopClassId(topId);
            curContent.addAssociationClass(fatherAssociation,
                                            fatherAssociationCount);
            curContent.addRealizationInterface(fatherRealization);
            for (String name : curAttributes.keySet()) {
                ArrayList<UmlAttribute> list = curAttributes.get(name);
                for (UmlAttribute attribute : list) {
                    curContent.addAttribute(name,attribute);
                }
            }
            if (curContent.isFather()) {
                classDfs(curContent,topId);
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

    private void initClasses(UmlClass umlClass) {
        String name = umlClass.getName();
        String id = umlClass.getId();
        if (!this.classNames.containsKey(name)) {
            ArrayList<UmlClass> list = new ArrayList<>();
            this.classNames.put(name,list);
        }
        this.classNames.get(name).add(umlClass);
        this.classContents.put(umlClass,new ClassContents(name,id));
    }

    private void initInterface(UmlInterface umlInterface) {
        this.interfaceContents.put(umlInterface,
                                    new InterfaceContents());
    }

    private void initAttributes(UmlAttribute attribute) {
        this.attributes.add(attribute);
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

    private UmlClass getClassByName(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        if (!this.classNames.containsKey(className)) {
            throw new ClassNotFoundException(className);
        }
        if (this.classNames.get(className).size() > 1) {
            throw new ClassDuplicatedException(className);
        }
        return this.classNames.get(className).get(0);
    }

}
