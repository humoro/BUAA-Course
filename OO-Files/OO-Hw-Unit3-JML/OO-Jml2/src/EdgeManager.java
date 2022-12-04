import java.util.HashMap;

class EdgeManager {
    private static final Integer ADD = 1;

    private HashMap<Integer, HashMap<Integer,Integer>> linkTable;

    EdgeManager() {
        this.linkTable = new HashMap<>();
    }

    void buildEdge(Integer node0,Integer node1,int addOrRem) {
        boolean exist = this.linkTable.containsKey(node0);
        if (exist) {
            exist = this.linkTable.get(node0).containsKey(node1);
        }
        if (exist) {
            int time = this.linkTable.get(node0).get(node1) + addOrRem;
            this.linkTable.get(node0).remove(node1);
            this.linkTable.get(node1).remove(node0);
            if (time > 0) {
                this.linkTable.get(node0).put(node1,time);
                this.linkTable.get(node1).put(node0,time);
            } else {
                if (this.linkTable.get(node0).isEmpty()) {
                    this.linkTable.remove(node0);
                }
                if (this.linkTable.get(node1).isEmpty()) {
                    this.linkTable.remove(node1);
                }
            }
        } else {
            if (addOrRem == ADD) {
                boolean exist0 = this.linkTable.containsKey(node0);
                boolean exist1 = this.linkTable.containsKey(node1);
                if (exist0) {
                    this.linkTable.get(node0).put(node1,1);
                } else {
                    HashMap<Integer,Integer> newmap = new HashMap<>();
                    newmap.put(node1,1);
                    this.linkTable.put(node0,newmap);
                }
                if (exist1) {
                    this.linkTable.get(node1).put(node0,1);
                } else {
                    HashMap<Integer,Integer> newmap = new HashMap<>();
                    newmap.put(node0,1);
                    this.linkTable.put(node1,newmap);
                }
            }
        }
    }

    boolean containsEdge(int fromNodeId, int toNodeId) {
        HashMap<Integer,Integer> fromLinks = this.linkTable.get(fromNodeId);
        HashMap<Integer,Integer> toLinks = this.linkTable.get(toNodeId);
        return fromLinks.containsKey(toNodeId) &&
                toLinks.containsKey(fromNodeId);
    }
}
