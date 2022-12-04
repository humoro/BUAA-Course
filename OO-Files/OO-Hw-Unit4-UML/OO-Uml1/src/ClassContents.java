import com.oocourse.uml1.interact.common.AttributeClassInformation;
import com.oocourse.uml1.interact.common.AttributeQueryType;
import com.oocourse.uml1.interact.common.OperationQueryType;
import com.oocourse.uml1.interact.exceptions.user.AttributeDuplicatedException;
import com.oocourse.uml1.interact.exceptions.user.AttributeNotFoundException;
import com.oocourse.uml1.models.common.Visibility;
import com.oocourse.uml1.models.elements.UmlAttribute;
import com.oocourse.uml1.models.elements.UmlClass;
import com.oocourse.uml1.models.elements.UmlInterface;
import com.oocourse.uml1.models.elements.UmlOperation;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Set;

class ClassContents {
    private String selfName;
    private String selfId;
    private String topClassId;
    private String directFatherId;
    private ArrayList<UmlClass> sonClasses = new ArrayList<>();
    //access attribute with name(all)
    private HashMap<String,
            ArrayList<UmlAttribute>> attributes = new HashMap<>();
    //all attributes divided with className
    private HashMap<String,
            ArrayList<UmlAttribute>> extendsAttributes = new HashMap<>();
    private List<AttributeClassInformation> information = new ArrayList<>();
    private int attributeNumber = 0;
    //access operation with returnType
    private HashMap<OperationQueryType,
                    ArrayList<UmlOperation>>
                    operationsQueryTypes = new HashMap<>();
    //access operation with name
    private HashMap<String,
            HashMap<Visibility,Integer>> operationVisibility = new HashMap<>();
    private HashMap<UmlClass,Integer> associateClass = new HashMap<>();
    private List<String> associationList = new ArrayList<>();
    private boolean isRefreshAssociation = false;
    private Integer associationCount = 0;
    private HashMap<UmlInterface,Integer> interfaceRealization =
                                                                new HashMap<>();
    private List<String> realizationList = new ArrayList<>();
    private boolean isRefreshRealization = false;

    ClassContents(String className,String id) {
        this.selfName = className;
        this.selfId = id;
        this.directFatherId = id;
        this.topClassId = id;
        this.initContainer();
    }

    //-------------------------------add method-------------------------------//
    void addAttribute(UmlAttribute attribute) {
        this.addAttribute(this.selfName,attribute);
    } // this is for self class

    void addAttribute(String className,UmlAttribute attribute) {
        this.attributeNumber++;
        if (!this.extendsAttributes.containsKey(className)) {
            ArrayList<UmlAttribute> list = new ArrayList<>();
            this.extendsAttributes.put(className,list);
        }
        this.extendsAttributes.get(className).add(attribute);
        Visibility curVis = attribute.getVisibility();
        if (!curVis.equals(Visibility.PRIVATE)) {
            this.information.add(new AttributeClassInformation(
                                  attribute.getName(),className));
        }
        String curName = attribute.getName();
        if (!this.attributes.containsKey(curName)) {
            this.attributes.put(curName,new ArrayList<>());
        }
        this.attributes.get(curName).add(attribute);
    } // this is for non_self class

    void addOperation(UmlOperation operation,OperationType type) {
        String curName = operation.getName();
        if (!this.operationVisibility.containsKey(curName)) {
            HashMap<Visibility,Integer> map = new HashMap<>();
            map.put(Visibility.PUBLIC,0);
            map.put(Visibility.PROTECTED,0);
            map.put(Visibility.PACKAGE,0);
            map.put(Visibility.PRIVATE,0);
            this.operationVisibility.put(curName,map);
        }
        int curTime = this.operationVisibility.get(curName).
                get(operation.getVisibility()) + 1;
        this.operationVisibility.get(curName).
                put(operation.getVisibility(),curTime);
        this.operationsQueryTypes.get(OperationQueryType.ALL).add(operation);
        if (!type.getHasParameter()) {
            this.operationsQueryTypes.get(OperationQueryType.NON_PARAM).
                    add(operation);
        } else {
            this.operationsQueryTypes.get(OperationQueryType.PARAM).
                    add(operation);
        }
        if (type.getReturnType()) {
            this.operationsQueryTypes.get(OperationQueryType.RETURN).
                    add(operation);
        } else {
            this.operationsQueryTypes.get(OperationQueryType.NON_RETURN).
                    add(operation);
        }
    }

    void addSonClasses(UmlClass umlClass) {
        this.sonClasses.add(umlClass);
    }

    void addAssociationClass(UmlClass umlClass) {
        this.addAssociationCount(1);
        this.associateClass.put(umlClass,1);
    }

    void addAssociationClass(HashMap<UmlClass,Integer> classes,int count) {
        this.addAssociationCount(count);
        this.associateClass.putAll(classes);
    }

    void addAssociationCount(int addNum) {
        this.associationCount += addNum;
    }

    void addRealizationInterface(UmlInterface umlInterface) {
        this.interfaceRealization.put(umlInterface,1);
    }

    void addRealizationInterface(HashMap<UmlInterface,Integer> interfaces) {
        this.interfaceRealization.putAll(interfaces);
    }

    void setDirectFatherId(String id) {
        this.directFatherId = id;
    }

    void setTopClassId(String id) {
        this.topClassId = id;
    }
    //-------------------------------get method-------------------------------//

    String getTopClassId() {
        return this.topClassId;
    }

    boolean isTopClass() {
        return this.directFatherId.equals(this.selfId) &&
               this.sonClasses.size() != 0;
    }

    boolean isFather() {
        return this.sonClasses.size() != 0;
    }

    ArrayList<UmlClass> getSonClasses() {
        return this.sonClasses;
    }

    int getAttributeCount(AttributeQueryType type) {
        if (type.equals(AttributeQueryType.SELF_ONLY)) {
            if (!this.extendsAttributes.containsKey(this.selfName)) {
                return 0;
            }
            return this.extendsAttributes.get(this.selfName).size();
        } else {
            return this.attributeNumber;
        }
    }

    HashMap<String,ArrayList<UmlAttribute>> getExtendsAttributes() {
        return this.extendsAttributes;
    }

    Visibility getAttributeVisibility(String attributeName)
            throws AttributeNotFoundException, AttributeDuplicatedException {
        if (!this.attributes.containsKey(attributeName)) {
            throw new AttributeNotFoundException(this.selfName,
                                                 attributeName);
        }
        if (this.attributes.get(attributeName).size() > 1) {
            throw new AttributeDuplicatedException(this.selfName,
                                                   attributeName);
        }
        return this.attributes.get(attributeName).get(0).getVisibility();
    }

    List<AttributeClassInformation> getAttributesClassInformation() {
        return this.information;
    }

    int getOperationCount(OperationQueryType type) {
        return this.operationsQueryTypes.get(type).size();
    }

    HashMap<Visibility,Integer> getOperationVisibility(String operationName) {
        if (!this.operationVisibility.containsKey(operationName)) {
            return new HashMap<>();
        }
        return this.operationVisibility.get(operationName);
    }

    int getAssociationCount() {
        return this.associationCount;
    }

    List<String> getAssociationList() {
        if (!this.isRefreshAssociation) {
            Set<UmlClass> keys = this.associateClass.keySet();
            for (UmlClass umlClass : keys) {
                this.associationList.add(umlClass.getName());
            }
            this.isRefreshAssociation = true;
        }
        return this.associationList;
    }

    HashMap<UmlClass,Integer> getAssociateClass() {
        return this.associateClass;
    }

    List<String> getRealizationList() {
        if (!this.isRefreshRealization) {
            Set<UmlInterface> keys = this.interfaceRealization.keySet();
            for (UmlInterface umlInterface : keys) {
                this.realizationList.add(umlInterface.getName());
            }
            this.isRefreshRealization = true;
        }
        return this.realizationList;

    }

    HashMap<UmlInterface,Integer> getInterfaceRealization() {
        return this.interfaceRealization;
    }

    //--------------------------------private method--------------------------//
    private void initContainer() {
        for (OperationQueryType type : OperationQueryType.values()) {
            this.operationsQueryTypes.put(type,new ArrayList<>());
        }
    }
}
