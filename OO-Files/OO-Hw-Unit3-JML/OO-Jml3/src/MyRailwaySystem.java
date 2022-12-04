import com.oocourse.specs3.models.NodeIdNotFoundException;
import com.oocourse.specs3.models.NodeNotConnectedException;
import com.oocourse.specs3.models.Path;
import com.oocourse.specs3.models.PathIdNotFoundException;
import com.oocourse.specs3.models.PathNotFoundException;
import com.oocourse.specs3.models.RailwaySystem;

public class MyRailwaySystem implements RailwaySystem {
    private static final Integer ADD = 1;
    private static final Integer REMOVE = -1;

    private GraphMaintain graph;
    private Integer curId = 0;

    public MyRailwaySystem() {
        this.graph = new GraphMaintain();
    }

    //----------------------------composition methods-------------------------//
    @Override
    public int addPath(Path path) {
        if (path == null || !path.isValid()) {
            return 0;
        } else {
            if (this.graph.hasPath(path)) {
                return this.graph.getId(path);
            } else {
                this.curId++;
                this.graph.refreshGraph(path,this.curId,ADD);
                return this.curId;
            }
        }
    }

    @Override
    public int removePath(Path path) throws PathNotFoundException {
        if (path == null || !path.isValid() || !this.graph.hasPath(path)) {
            throw new PathNotFoundException(path);
        } else {
            int id = this.graph.getId(path);
            this.graph.refreshGraph(path,id,REMOVE);
            return id;
        }
    }

    @Override
    public void removePathById(int pathId) throws PathIdNotFoundException {
        if (!this.graph.hasPath(pathId)) {
            throw new PathIdNotFoundException(pathId);
        } else {
            Path path = this.graph.getPath(pathId);
            this.graph.refreshGraph(path,pathId,REMOVE);
        }
    }

    //----------------------------inquire methods of node---------------------//
    @Override
    public int getDistinctNodeCount() {
        return this.graph.getNodeCount();
    }

    @Override
    public boolean containsNode(int nodeId) {
        return this.graph.hasNode(nodeId);
    }

    //----------------------------inquire methods of path---------------------//
    @Override
    public boolean containsPath(Path path) {
        return this.graph.hasPath(path);
    }

    @Override
    public boolean containsPathId(int pathId) {
        return this.graph.hasPath(pathId);
    }

    @Override
    public Path getPathById(int pathId)
            throws PathIdNotFoundException {
        if (this.graph.hasPath(pathId)) {
            return this.graph.getPath(pathId);
        } else {
            throw new PathIdNotFoundException(pathId);
        }
    }

    @Override
    public int getPathId(Path path) throws PathNotFoundException {
        if (path == null ||
            !path.isValid() ||
            !this.graph.hasPath(path)) {
            throw new PathNotFoundException(path);
        } else {
            return this.graph.getId(path);
        }
    }

    @Override
    public int size() {
        return this.graph.getPathSize();
    }

    //---------------------------inquire methods of graph---------------------//
    @Override
    public boolean containsEdge(int fromNodeId, int toNodeId) {
        if (!this.graph.hasNode(fromNodeId) ||
            !this.graph.hasNode(toNodeId)) {
            return false;
        }
        return this.graph.hasEdge(fromNodeId,toNodeId);
    }

    @Override
    public boolean isConnected(int fromNodeId, int toNodeId)
                                throws NodeIdNotFoundException {
        boolean fromExist = this.graph.hasNode(fromNodeId);
        if (!fromExist) {
            throw new NodeIdNotFoundException(fromNodeId);
        }
        boolean toExist = this.graph.hasNode(toNodeId);
        if (!toExist) {
            throw new NodeIdNotFoundException(toNodeId);
        }
        if (fromNodeId == toNodeId) {
            return true;
        }
        return this.graph.nodesConnected(fromNodeId,toNodeId);
    }

    @Override
    public int getShortestPathLength(int fromNodeId, int toNodeId)
            throws NodeIdNotFoundException, NodeNotConnectedException {
        this.graph.checkNodeLine(fromNodeId,toNodeId);
        if (fromNodeId == toNodeId) {
            return 0;
        }
        return this.graph.getShortestNodesDis(fromNodeId,toNodeId);
    }
    //--------------------------inquire methods of railway--------------------//

    @Override
    public int getLeastTicketPrice(int var1, int var2)
            throws NodeIdNotFoundException, NodeNotConnectedException {
        this.graph.checkNodeLine(var1,var2);
        return this.graph.leastTicketPrice(var1,var2);
    }

    @Override
    public int getLeastTransferCount(int var1, int var2)
            throws NodeIdNotFoundException, NodeNotConnectedException {
        this.graph.checkNodeLine(var1,var2);
        return this.graph.leastTransferCount(var1,var2);
    }

    @Override
    public int getUnpleasantValue(Path var1, int var2, int var3) {
        return this.graph.unpleasantValue(var1,var2,var3);
    }

    @Override
    public int getLeastUnpleasantValue(int var1, int var2)
            throws NodeIdNotFoundException, NodeNotConnectedException {
        this.graph.checkNodeLine(var1,var2);
        return this.graph.leastUnpleasantValue(var1,var2);
    }

    @Override
    public int getConnectedBlockCount() {
        return this.graph.connectedBlockCount();
    }
}
