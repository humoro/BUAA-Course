import java.util.HashMap;

class NodeManager {

    private HashMap<Integer,Integer> nodes;

    NodeManager(HashMap<Integer,Integer> nodes) {
        this.nodes = nodes;
    }

    boolean ownNode(int node) {
        return this.nodes.containsKey(node);
    }

    int getCount() {
        return this.nodes.size();
    }
}
