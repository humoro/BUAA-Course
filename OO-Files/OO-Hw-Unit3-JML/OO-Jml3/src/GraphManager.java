import com.oocourse.specs3.models.Path;

import java.util.HashMap;
import java.util.Set;

class GraphManager {
    private static final int ADD = 1;
    private static final int REMOVE = -1;
    private static final int UNCONNECTEDIFO = 65536;

    private static int F(int x) {
        return (x % 5 + 5) % 5;
    }

    private static int H(int x) {
        return (int)Math.pow(4,x);
    }

    private static int U_E(int u,int v) {
        return H(Math.max(F(u),F(v)));
    }

    private HashMap<Integer,Integer> nodes;
    private HashMap<Edge,Integer> containEdge;
    private HashMap<Integer, Path> paths;
    private HashMap<Path,Integer> ids;
    private HashMap<Integer,HashMap<Integer,Integer>> disTable;
    private HashMap<Integer,HashMap<Integer,Integer>> priceTable;
    private HashMap<Integer,HashMap<Integer,Integer>> transTable;
    private HashMap<Integer,HashMap<Integer,Integer>> unpleasantTable;
    private HashMap<Integer,Integer> blocks;
    private HashMap<Integer,HashMap<Integer,
            HashMap<Integer,Integer>>> pathEdges;
    private HashMap<Integer,HashMap<Integer,
            HashMap<Integer,Integer>>> unplEdges;

    GraphManager(HashMap<Integer,Integer> nodes,
                 HashMap<Edge,Integer> containEdge,
                 HashMap<Integer, Path> paths,
                 HashMap<Path,Integer> ids,
                 HashMap<Integer,HashMap<Integer,Integer>> disTable,
                 HashMap<Integer,HashMap<Integer,Integer>> priceTable,
                 HashMap<Integer,HashMap<Integer,Integer>> transTable,
                 HashMap<Integer,HashMap<Integer,Integer>> unpleasantTable,
                 HashMap<Integer,Integer> blocks) {
        this.nodes = nodes;
        this.containEdge = containEdge;
        this.paths = paths;
        this.ids = ids;
        this.disTable = disTable;
        this.priceTable = priceTable;
        this.transTable = transTable;
        this.unpleasantTable = unpleasantTable;
        this.blocks = blocks;
        this.pathEdges = new HashMap<>();
        this.unplEdges = new HashMap<>();
    }

    void refreshGraph(Path path,int id,int addOrRemove) {
        if (addOrRemove == ADD) {
            HashMap<Integer,HashMap<Integer,Integer>> edges = new HashMap<>();
            HashMap<Integer,HashMap<Integer,Integer>> edges1 = new HashMap<>();
            int size = path.size();
            for (int i = 0; i < size - 1; i++) {
                int cur = path.getNode(i);
                int next = path.getNode(i + 1);
                if (!edges.containsKey(cur)) {
                    create(cur,edges);
                }
                if (!edges.containsKey(next)) {
                    create(next,edges);
                }
                if (!edges1.containsKey(cur)) {
                    create(cur,edges1);
                }
                if (!edges1.containsKey(next)) {
                    create(next,edges1);
                }
                edges.get(cur).put(next,1);
                edges.get(next).put(cur,1);
                int value = U_E(cur,next);
                edges1.get(cur).put(next,value);
                edges1.get(next).put(cur,value);
            }
            this.pathEdges.put(id,edges);
            this.unplEdges.put(id,edges1);
            pathFloyd(id);
        } else {
            this.pathEdges.remove(id);
            this.unplEdges.remove(id);
        }
        int size = path.size();
        for (int i = 0; i < size; i++) {
            int cur = path.getNode(i);
            refreshNodes(cur,addOrRemove);
            if (i < size - 1) {
                int nextNode = path.getNode(i + 1);
                refreshEdge(cur,nextNode,addOrRemove);
            }
        }
        clearTable();
        buildTable();
        if (addOrRemove == ADD) {
            addPath(path,id);
        } else {
            removePath(path,id);
        }
    }

    private void create(int node,
                        HashMap<Integer,HashMap<Integer,Integer>> maps) {
        Set<Integer> keys = maps.keySet();
        HashMap<Integer,Integer> map = new HashMap<>();
        for (Integer key: keys) {
            maps.get(key).put(node,UNCONNECTEDIFO);
            map.put(key,UNCONNECTEDIFO);
        }
        map.put(node,0);
        maps.put(node,map);
    }

    private void refreshNodes(int curNode,int addOrRemove) {
        if (addOrRemove == ADD) {
            if (this.nodes.containsKey(curNode)) {
                int time = this.nodes.get(curNode) + 1;
                this.nodes.put(curNode,time);
            } else {
                refreshTables(curNode,ADD);
                this.nodes.put(curNode,1);
            }
        } else {
            int time = this.nodes.get(curNode) - 1;
            if (time == 0) {
                this.nodes.remove(curNode);
                refreshTables(curNode,REMOVE);
            } else {
                this.nodes.put(curNode,time);
            }
        }

    }

    private void refreshEdge(int node0,int node1,int addOrRem) {
        Edge edge0 = new Edge(node0,node1);
        Edge edge1 = new Edge(node1,node0);
        if (addOrRem == REMOVE) {
            int time = this.containEdge.get(edge0) - 1;
            if (time == 0) {
                this.containEdge.remove(edge0);
                this.containEdge.remove(edge1);
            } else {
                this.containEdge.put(edge0,time);
                this.containEdge.put(edge1,time);
            }
        } else {
            int time = 1;
            if (this.containEdge.containsKey(edge0)) {
                time = this.containEdge.get(edge0) + 1;
            }
            this.containEdge.put(edge0,time);
            this.containEdge.put(edge1,time);
        }
    }

    private void addPath(Path path,int id) {
        this.paths.put(id,path);
        this.ids.put(path,id);
    }

    private void removePath(Path path,int id) {
        this.paths.remove(id);
        this.ids.remove(path);
    }

    private void refreshTables(int node,int addOrRemove) {
        if (addOrRemove == ADD) {
            HashMap<Integer,Integer> map0 = new HashMap<>();
            HashMap<Integer,Integer> map1 = new HashMap<>();
            HashMap<Integer,Integer> map2 = new HashMap<>();
            HashMap<Integer,Integer> map3 = new HashMap<>();
            for (Integer key : this.nodes.keySet()) {
                this.disTable.get(key).put(node,UNCONNECTEDIFO);
                this.priceTable.get(key).put(node,UNCONNECTEDIFO);
                this.transTable.get(key).put(node,UNCONNECTEDIFO);
                this.unpleasantTable.get(key).put(node,UNCONNECTEDIFO);
                map0.put(key,UNCONNECTEDIFO);
                map1.put(key,UNCONNECTEDIFO);
                map2.put(key,UNCONNECTEDIFO);
                map3.put(key,UNCONNECTEDIFO);
            }
            map0.put(node,UNCONNECTEDIFO);
            map1.put(node,UNCONNECTEDIFO);
            map2.put(node,UNCONNECTEDIFO);
            map3.put(node,UNCONNECTEDIFO);
            this.disTable.put(node,map0);
            this.priceTable.put(node,map1);
            this.transTable.put(node,map2);
            this.unpleasantTable.put(node,map3);
            this.blocks.put(node,0);
        } else {
            this.disTable.remove(node);
            this.priceTable.remove(node);
            this.transTable.remove(node);
            this.unpleasantTable.remove(node);
            this.blocks.remove(node);
            for (Integer key : this.nodes.keySet()) {
                this.disTable.get(key).remove(node);
                this.priceTable.get(key).remove(node);
                this.transTable.get(key).remove(node);
                this.unpleasantTable.get(key).remove(node);
            }
        }
    }

    private void clearTable() {
        Set<Integer> keys = this.nodes.keySet();
        for (Integer ikey : keys) {
            this.blocks.put(ikey,UNCONNECTEDIFO);
            for (Integer jkey : keys) {
                this.disTable.get(ikey).put(jkey,UNCONNECTEDIFO);
                this.priceTable.get(ikey).put(jkey,UNCONNECTEDIFO);
                this.transTable.get(ikey).put(jkey,UNCONNECTEDIFO);
                this.unpleasantTable.get(ikey).put(jkey,UNCONNECTEDIFO);
            }
        }
    }

    private void pathFloyd(int id) {
        Set<Integer> keys = this.pathEdges.get(id).keySet();
        HashMap<Integer,HashMap<Integer,Integer>> curmap =
                this.pathEdges.get(id);
        HashMap<Integer,HashMap<Integer,Integer>> curmap1 =
                this.unplEdges.get(id);
        for (Integer kkey : keys) {
            for (Integer ikey : keys) {
                for (Integer jkey : keys) {
                    int min = Math.min(curmap.get(ikey).get(jkey),
                                curmap.get(ikey).get(kkey) +
                                curmap.get(kkey).get(jkey));
                    curmap.get(ikey).put(jkey,min);
                    min = Math.min(curmap1.get(ikey).get(jkey),curmap1.
                            get(ikey).get(kkey) + curmap1.get(kkey).get(jkey));
                    curmap1.get(ikey).put(jkey,min);
                }
            }
        }
    }

    private void buildTable() {
        Set<Integer> ids = this.pathEdges.keySet();
        for (Integer id : ids) {
            HashMap<Integer,HashMap<Integer,Integer>> curmap =
                    this.pathEdges.get(id);
            HashMap<Integer,HashMap<Integer,Integer>> curmap1 =
                    this.unplEdges.get(id);
            Set<Integer> keys = this.pathEdges.get(id).keySet();
            for (Integer kkey : keys) {
                for (Integer ikey : keys) {
                    int time = Math.min(this.priceTable.get(kkey).get(ikey),
                            curmap.get(kkey).get(ikey));
                    this.disTable.get(kkey).put(ikey,time);
                    this.priceTable.get(kkey).put(ikey,time);
                    this.transTable.get(kkey).put(ikey,time);
                    time = Math.min(this.unpleasantTable.get(kkey).get(ikey),
                            curmap1.get(kkey).get(ikey));
                    this.unpleasantTable.get(kkey).put(ikey,time);
                }
            }
        }
    }
}
