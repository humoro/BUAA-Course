import com.oocourse.specs3.models.Path;
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

    @Override
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

    @Override
    public Iterator<Integer> iterator() {
        return this.nodes.iterator();
    }

    @Override
    public int size() {
        return this.nodes.size();
    }

    @Override
    public int getNode(int index) {
        return this.nodes.get(index);
    }

    @Override
    public boolean containsNode(int node) {
        return this.nodeMap.containsKey(node);
    }

    @Override
    public int getDistinctNodeCount() {
        return this.nodeMap.size();
    }

    @Override
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

    @Override
    public boolean isValid() {
        return this.nodes.size() >= 2;
    }

    @Override
    public int hashCode() {
        return this.nodes.hashCode();
    }

    public int getUnpleasantValue(int var1) {
        if (!this.nodeMap.containsKey(var1)) {
            return 0;
        } else {
            return (int)Math.pow(4,(var1 % 5 + 5) % 5);
        }
    }
}
