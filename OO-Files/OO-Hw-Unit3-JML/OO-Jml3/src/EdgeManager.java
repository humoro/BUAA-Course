import java.util.HashMap;

class EdgeManager {

    private HashMap<Edge,Integer> containEdges;

    EdgeManager(HashMap<Edge,Integer> containEdges) {
        this.containEdges = containEdges;
    }

    boolean containsEdge(int from,int to) {
        return this.containEdges.containsKey(new Edge(from,to));
    }
}
