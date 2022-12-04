import com.oocourse.specs2.models.NodeIdNotFoundException;
import com.oocourse.specs2.models.NodeNotConnectedException;
import com.oocourse.specs2.models.Path;

import java.util.HashMap;
import java.util.Set;

class GraphMaintain {
    private static final Integer ADD = 1;
    private static final Integer REMOVE = -1;
    private static final int INFO = 251;

    private HashMap<Integer, HashMap<Integer,Integer>> distanceTable;
    private boolean needRefreshDis = true;
    private NodeManager nodes;
    private EdgeManager edges;

    GraphMaintain() {
        this.nodes = new NodeManager();
        this.edges = new EdgeManager();
        this.distanceTable = new HashMap<>();
    }

    //----------------------------composition methods-------------------------//
    void addPath(Path path) {
        refreshGraph(path,ADD);
        clearDis();
    }

    void removePath(Path path) {
        refreshGraph(path,REMOVE);
        clearDis();
    }

    private void refreshGraph(Path path,int addOrRemove) {
        int size = path.size();
        for (int i = 0; i < size; i++) {
            int cur = path.getNode(i);
            Set<Integer> keys = this.distanceTable.keySet();
            boolean isContain = this.nodes.containNode(cur);
            if (addOrRemove == ADD && !isContain) {
                HashMap<Integer,Integer> newmap = new HashMap<>();
                for (Integer key : keys) {
                    newmap.put(key,0);
                    this.distanceTable.get(key).put(cur,0);
                }
                newmap.put(cur,0);
                this.distanceTable.put(cur,newmap);
            }
            this.nodes.refreshNode(cur,addOrRemove);
            isContain = this.nodes.containNode(cur);
            if (addOrRemove == REMOVE && !isContain) {
                for (Integer key : keys) {
                    if (!key.equals(cur)) {
                        this.distanceTable.get(key).remove(cur);
                    }
                }
                this.distanceTable.remove(cur);
            }
            if (i < size - 1) {
                Integer nextNode = path.getNode(i + 1);
                this.edges.buildEdge(cur,nextNode,addOrRemove);
            }
        }
    }
    //------------------------------distance method---------------------------//

    private void clearDis() {
        for (Integer key : this.distanceTable.keySet()) {
            HashMap<Integer,Integer> curMap = this.distanceTable.get(key);
            for (Integer okey : curMap.keySet()) {
                if (this.edges.containsEdge(key,okey)) {
                    curMap.put(okey,1);
                } else if (key.equals(okey)) {
                    curMap.put(okey,0);
                } else {
                    curMap.put(okey,INFO);
                }
            }
        }
        this.needRefreshDis = true;
    }

    void refreshDis() {
        Set<Integer> keys = this.distanceTable.keySet();
        for (Integer kkey : keys) {
            for (Integer ikey : keys) {
                for (Integer jkey : keys) {
                    int min = Math.min(this.distanceTable.get(ikey).get(jkey),
                            this.distanceTable.get(ikey).get(kkey) +
                                    this.distanceTable.get(kkey).get(jkey));
                    this.distanceTable.get(ikey).put(jkey,min);
                }
            }
        }
        this.needRefreshDis = false;
    }
    //--------------------------inquire method of graph-----------------------//

    void checkNodeLine(int var1,int var2)
            throws NodeIdNotFoundException, NodeNotConnectedException {
        if (!hasNode(var1)) {
            throw new NodeIdNotFoundException(var1);
        }
        if (!hasNode(var2)) {
            throw new NodeIdNotFoundException(var2);
        }
        if (!nodesConnected(var1,var2)) {
            throw new NodeNotConnectedException(var1, var2);
        }
    }

    boolean nodesConnected(int fromNodeId,int toNodeId) {
        return this.getShortestNodesDis(fromNodeId,toNodeId) < INFO - 1;
    }

    int getShortestNodesDis(int fromNodeId,int toNodeId) {
        return this.distanceTable.get(fromNodeId).get(toNodeId);
    }

    boolean isNeedRefreshDis() {
        return this.needRefreshDis;
    }

    //--------------------------inquire method of node-----------------------//

    int getNodeCount() {
        return this.nodes.getDistinctSize();
    }

    boolean hasNode(int nodeId) {
        return this.nodes.containNode(nodeId);
    }
    //-------------------------------edge methods-----------------------------//

    boolean hasEdge(int fromNodeId,int toNodeId) {
        return this.edges.containsEdge(fromNodeId,toNodeId);
    }
}
