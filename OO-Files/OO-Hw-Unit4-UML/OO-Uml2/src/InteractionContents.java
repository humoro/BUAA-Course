import com.oocourse.uml2.interact.exceptions.user.LifelineDuplicatedException;
import com.oocourse.uml2.interact.exceptions.user.LifelineNotFoundException;
import com.oocourse.uml2.models.elements.UmlLifeline;
import com.oocourse.uml2.models.elements.UmlMessage;

import java.util.ArrayList;
import java.util.HashMap;

class InteractionContents {
    private String selfName;
    private Integer lifelineCount = 0;
    private boolean isRefreshMessage = false;
    private HashMap<String,ArrayList<UmlLifeline>> lifelines = new HashMap<>();
    private HashMap<String,
                    ArrayList<UmlMessage>> incomingCounts = new HashMap<>();
    private ArrayList<UmlMessage> messages = new ArrayList<>();

    InteractionContents(String name) {
        this.selfName = name;
    }

    void addLifeLine(UmlLifeline lifeline) {
        this.lifelineCount++;
        String name = lifeline.getName();
        if (!this.lifelines.containsKey(name)) {
            this.lifelines.put(name,new ArrayList<>());
        }
        this.lifelines.get(name).add(lifeline);
    }

    void addMessages(ArrayList<UmlMessage> messages) {
        this.messages.addAll(messages);
    }

    int getMessageCount() {
        return this.messages.size();
    }

    int getIncomingCount(String lifelineName)
                  throws LifelineNotFoundException,
                         LifelineDuplicatedException {
        if (!this.lifelines.containsKey(lifelineName)) {
            throw new LifelineNotFoundException(this.selfName,lifelineName);
        }
        if (this.lifelines.get(lifelineName).size() > 1) {
            throw new LifelineDuplicatedException(this.selfName,lifelineName);
        }
        UmlLifeline lifeline = this.lifelines.get(lifelineName).get(0);
        String id = lifeline.getId();
        if (!this.isRefreshMessage) {
            this.isRefreshMessage = true;
            classifyMessages();
        }
        if (!this.incomingCounts.containsKey(id)) {
            return 0;
        }
        return this.incomingCounts.get(id).size();
    }

    int getLifelineCount() {
        return this.lifelineCount;
    }

    private void classifyMessages() {
        for (UmlMessage message : this.messages) {
            String targetId = message.getTarget();
            if (!this.incomingCounts.containsKey(targetId)) {
                this.incomingCounts.put(targetId,new ArrayList<>());
            }
            this.incomingCounts.get(targetId).add(message);
        }
    }
}
