import java.util.HashMap;

class NodeManager {
    private static final Integer ADD = 1;

    private HashMap<Integer,Integer> nodes;

    NodeManager() {
        this.nodes = new HashMap<>();
    }

    void refreshNode(int curNode,int addOrRemove) {
        HashMap<Integer,Integer> curOpMap;
        curOpMap = this.nodes;
        boolean exist = curOpMap.containsKey(curNode);
        if (exist) {
            Integer time = curOpMap.get(curNode) + addOrRemove;
            curOpMap.remove(curNode);
            if (time > 0) {
                curOpMap.put(curNode,time);
            }
        } else if (addOrRemove == ADD) {
            Integer time = 1;
            curOpMap.put(curNode,time);
        }
    }

    boolean containNode(int node) {
        return this.nodes.containsKey(node);
    }

    int getDistinctSize() {
        return this.nodes.size();
    }
}
