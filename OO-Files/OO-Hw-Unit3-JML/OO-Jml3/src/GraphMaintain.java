import com.oocourse.specs3.models.NodeIdNotFoundException;
import com.oocourse.specs3.models.NodeNotConnectedException;
import com.oocourse.specs3.models.Path;

import java.util.HashMap;

class GraphMaintain {
    private GraphAlgorithm algorithm;
    private GraphManager graphManager;
    private NodeManager nodes;
    private EdgeManager edges;
    private PathManager paths;

    GraphMaintain() {
        HashMap<Integer,Integer> nodes = new HashMap<>();
        HashMap<Edge,Integer> containEdge = new HashMap<>();
        HashMap<Integer,Path> mypaths = new HashMap<>();
        HashMap<Path,Integer> myIds = new HashMap<>();
        HashMap<Integer,HashMap<Integer,Integer>> disTable = new HashMap<>();
        HashMap<Integer,HashMap<Integer,Integer>> priceTable = new HashMap<>();
        HashMap<Integer,HashMap<Integer,Integer>> transTable = new HashMap<>();
        HashMap<Integer,HashMap<Integer,Integer>> unpleasant = new HashMap<>();
        HashMap<Integer,Integer> blocks = new HashMap<>();
        this.graphManager = new GraphManager(nodes,containEdge,mypaths,myIds,
                                             disTable,priceTable,transTable,
                                             unpleasant,blocks);
        this.algorithm = new GraphAlgorithm(containEdge,disTable,priceTable,
                                            transTable,unpleasant,blocks);
        this.nodes = new NodeManager(nodes);
        this.edges = new EdgeManager(containEdge);
        this.paths = new PathManager(mypaths,myIds);
    }
    //----------------------------composition methods-------------------------//

    void refreshGraph(Path path,int id,int addOrRemove) {
        this.graphManager.refreshGraph(path,id,addOrRemove);
        this.algorithm.setRefreshed();
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
    //----------------------------graph algorithm-----------------------------//

    boolean nodesConnected(int fromNodeId,int toNodeId) {
        return this.algorithm.isNodesConnected(fromNodeId,toNodeId);
    }

    int getShortestNodesDis(int fromNodeId,int toNodeId) {
        return this.algorithm.distance(fromNodeId,toNodeId);
    }

    int leastTicketPrice(int fromNodeId,int toNodeId) {
        return this.algorithm.ticketPrice(fromNodeId,toNodeId);
    }

    int leastTransferCount(int fromNodeId,int toNodeId) {
        return this.algorithm.transferCount(fromNodeId,toNodeId);
    }

    int unpleasantValue(Path path,int from,int to) {
        return this.algorithm.unpValue(path,from,to);
    }

    int leastUnpleasantValue(int fromNodeId, int toNodeId) {
        return this.algorithm.leastunpValue(fromNodeId,toNodeId);
    }

    int connectedBlockCount() {
        return this.algorithm.blockCount();
    }
    //--------------------------inquire methods of node-----------------------//

    int getNodeCount() {
        return this.nodes.getCount();
    }

    boolean hasNode(int nodeId) {
        return this.nodes.ownNode(nodeId);
    }
    //--------------------------inquire methods of path-----------------------//

    boolean hasPath(Path path) {
        return this.paths.ownPath(path);
    }

    boolean hasPath(int id) {
        return this.paths.ownId(id);
    }

    Path getPath(int id) {
        return this.paths.gainPath(id);
    }

    int getId(Path path) {
        return this.paths.gainId(path);
    }

    int getPathSize() {
        return this.paths.getCount();
    }
    //--------------------------inquire methods of edge-----------------------//

    boolean hasEdge(int fromNodeId,int toNodeId) {
        return this.edges.containsEdge(fromNodeId,toNodeId);
    }
}
