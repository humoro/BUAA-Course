import com.oocourse.specs2.models.Graph;
import com.oocourse.specs2.models.NodeIdNotFoundException;
import com.oocourse.specs2.models.NodeNotConnectedException;
import com.oocourse.specs2.models.Path;
import com.oocourse.specs2.models.PathIdNotFoundException;
import com.oocourse.specs2.models.PathNotFoundException;
import java.util.HashMap;

public class MyGraph implements Graph {
    private HashMap<Integer, Path> paths;
    private HashMap<Path,Integer> ids;
    private GraphMaintain graph;
    private Integer curId = 0;

    public MyGraph() {
        this.paths = new HashMap<>();
        this.ids = new HashMap<>();
        this.graph = new GraphMaintain();
    }

    //----------------------------composition methods-------------------------//
    @Override
    public int addPath(Path path) {
        if (path == null || !path.isValid()) {
            return 0;
        } else {
            if (this.ids.containsKey(path)) {
                return this.ids.get(path);
            } else {
                this.curId++;
                this.paths.put(this.curId,path);
                this.ids.put(path,this.curId);
                this.graph.addPath(path);
                return this.curId;
            }
        }
    }

    @Override
    public int removePath(Path path) throws PathNotFoundException {
        if (path == null || !path.isValid() || !this.ids.containsKey(path)) {
            throw new PathNotFoundException(path);
        } else {
            int id = this.ids.get(path);
            this.ids.remove(path);
            this.paths.remove(id);
            this.graph.removePath(path);
            return id;
        }
    }

    @Override
    public void removePathById(int pathId) throws PathIdNotFoundException {
        if (!this.paths.containsKey(pathId)) {
            throw new PathIdNotFoundException(pathId);
        } else {
            Path curPath = this.paths.get(pathId);
            this.ids.remove(curPath);
            this.paths.remove(pathId);
            this.graph.removePath(curPath);
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
        return this.ids.containsKey(path);
    }

    @Override
    public boolean containsPathId(int pathId) {
        return this.paths.containsKey(pathId);
    }

    @Override
    public Path getPathById(int pathId)
            throws PathIdNotFoundException {
        if (this.paths.containsKey(pathId)) {
            return this.paths.get(pathId);
        } else {
            throw new PathIdNotFoundException(pathId);
        }
    }

    @Override
    public int getPathId(Path path) throws PathNotFoundException {
        if (path == null ||
            !path.isValid() ||
            !this.ids.containsKey(path)) {
            throw new PathNotFoundException(path);
        } else {
            return this.ids.get(path);
        }
    }

    @Override
    public int size() {
        return this.paths.size();
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
        if (this.graph.isNeedRefreshDis()) {
            this.graph.refreshDis();
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
}
