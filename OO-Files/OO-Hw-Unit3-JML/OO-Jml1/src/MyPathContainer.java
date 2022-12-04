import com.oocourse.specs1.models.Path;
import com.oocourse.specs1.models.PathContainer;
import com.oocourse.specs1.models.PathIdNotFoundException;
import com.oocourse.specs1.models.PathNotFoundException;

import java.util.HashMap;

public class MyPathContainer implements PathContainer {
    private HashMap<Integer,Path> paths;
    private HashMap<Path,Integer> ids;
    private HashMap<Integer,Integer> nodes;
    private Integer curId = 0;

    public MyPathContainer() {
        this.paths = new HashMap<>();
        this.ids = new HashMap<>();
        this.nodes = new HashMap<>();
    }

    public /*@pure@*/int size() {
        return this.paths.size();
    }

    public /*@pure@*/ boolean containsPath(Path path) {
        return this.ids.containsKey(path);
    }

    public /*@pure@*/ boolean containsPathId(int pathId) {
        return this.paths.containsKey(pathId);
    }

    public /*@pure@*/ Path getPathById(int pathId)
                                            throws PathIdNotFoundException {
        if (this.paths.containsKey(pathId)) {
            return this.paths.get(pathId);
        } else {
            throw new PathIdNotFoundException(pathId);
        }
    }

    public /*@pure@*/ int getPathId(Path path) throws PathNotFoundException {
        if (path == null || !path.isValid() || !this.ids.containsKey(path)) {
            throw new PathNotFoundException(path);
        } else {
            return this.ids.get(path);
        }
    }

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
                refreshNode(path,1);
                return this.curId;
            }
        }
    }

    public int removePath(Path path) throws PathNotFoundException {
        if (path == null || !path.isValid() || !this.ids.containsKey(path)) {
            throw new PathNotFoundException(path);
        } else {
            int id = this.ids.get(path);
            removeNode(path);
            this.ids.remove(path);
            this.paths.remove(id);
            return id;
        }
    }

    public void removePathById(int pathId) throws PathIdNotFoundException {
        if (!this.paths.containsKey(pathId)) {
            throw new PathIdNotFoundException(pathId);
        } else {
            Path curPath = this.paths.get(pathId);
            removeNode(pathId);
            this.ids.remove(curPath);
            this.paths.remove(pathId);
        }
    }

    public /*@pure@*/int getDistinctNodeCount() {
        return this.nodes.size();
    }

    private void refreshNode(Path path,int addOrRemove) {
        for (int i = 0; i < path.size(); i++) {
            Integer curNode = path.getNode(i);
            boolean exist = this.nodes.containsKey(curNode);
            if (exist) {
                Integer time = this.nodes.get(curNode) + addOrRemove;
                this.nodes.remove(curNode);
                if (time > 0) {
                    this.nodes.put(curNode,time);
                }
            } else if (addOrRemove == 1) {
                Integer time = 1;
                this.nodes.put(curNode,time);
            }
        }
    }

    private void removeNode(Integer id) {
        Path path = this.paths.get(id);
        refreshNode(path,-1);
    }

    private void removeNode(Path path) {
        refreshNode(path,-1);
    }
}
