import com.oocourse.specs1.models.Path;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

public class MyPath implements Path {
    private ArrayList<Integer> nodes = new ArrayList<>();
    private HashMap<Integer,Integer> nodeMap = new HashMap<>();

    public MyPath(int[] nodeList) {
        for (int i:nodeList) {
            this.nodes.add(i);
            if (!nodeMap.containsKey(i)) {
                nodeMap.put(i,1);
            }
        }
    }

    public int compareTo(Path o) {
        int bound = Math.min(o.size(),this.nodes.size());
        int i;
        int thisNode;
        int otherNode;
        for (i = 0;i < bound;i++) {
            thisNode = this.nodes.get(i);
            otherNode = o.getNode(i);
            if (thisNode != otherNode) {
                return Integer.compare(thisNode,otherNode);
            }
        }
        return Integer.compare(this.size(),o.size());
    }

    public Iterator<Integer> iterator() {
        return this.nodes.iterator();
    }

    public /*@pure@*/int size() {
        return this.nodes.size();
    }

    public /*@pure@*/ int getNode(int index) {
        return this.nodes.get(index);
    }

    public /*@pure@*/ boolean containsNode(int node) {
        return this.nodeMap.containsKey(node);
    }

    public /*pure*/ int getDistinctNodeCount() {
        return this.nodeMap.size();
    }

    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        } else if (!(obj instanceof Path)) {
            return false;
        } else {
            Path path = (Path) obj;
            if (this.nodes.size() != path.size()) {
                return false;
            }
            for (int i = 0;i < path.size();i++) {
                if (this.nodes.get(i) != path.getNode(i)) {
                    return false;
                }
            }
            return true;
        }
    }

    public boolean isValid() {
        return this.nodes.size() >= 2;
    }

    @Override
    public int hashCode() {
        return this.nodes.hashCode();
    }
}
