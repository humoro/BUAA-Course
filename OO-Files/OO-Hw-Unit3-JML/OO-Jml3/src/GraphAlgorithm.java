
import com.oocourse.specs3.models.Path;

import java.util.HashMap;
import java.util.Set;

class GraphAlgorithm {
    private static final int UNCONNECTEDINFO = 65536;
    private static final int U_S = 32;
    private static final int TRANSFERCOUNT = 121;
    private static final int TRANSFERPRICE = 2;

    private HashMap<Edge,Integer> containsEdge;
    private HashMap<Integer,HashMap<Integer,Integer>> disTable;
    private HashMap<Integer,HashMap<Integer,Integer>> priceTable;
    private HashMap<Integer,HashMap<Integer,Integer>> transTable;
    private HashMap<Integer,HashMap<Integer,Integer>> unpleasantTable;
    private HashMap<Integer,Integer> blocks;
    private int blockNo = 0;
    private boolean refresheddis = true;
    private boolean refreshedpric = true;
    private boolean refreshedtrans = true;
    private boolean refreshedunp = true;
    private boolean refreshedblock = true;

    GraphAlgorithm(HashMap<Edge,Integer> containsEdge,
                   HashMap<Integer,HashMap<Integer,Integer>> disTable,
                   HashMap<Integer,HashMap<Integer,Integer>> priceTable,
                   HashMap<Integer,HashMap<Integer,Integer>> transTable,
                   HashMap<Integer,HashMap<Integer,Integer>> unpleasantTable,
                   HashMap<Integer,Integer> blocks
                   ) {
        this.containsEdge = containsEdge;
        this.disTable = disTable;
        this.priceTable = priceTable;
        this.transTable = transTable;
        this.unpleasantTable = unpleasantTable;
        this.blocks = blocks;
    }

    boolean isNodesConnected(int from,int to) {
        if (from == to) {
            return true;
        }
        if (this.refreshedblock) {
            this.blockCount();
        }
        return this.blocks.get(from).equals(this.blocks.get(to));
    }

    int distance(int from,int to) {
        if (from == to) {
            return 0;
        }
        if (this.refresheddis) {
            floyd(disTable,0);
            this.refresheddis = false;
        }
        return this.disTable.get(from).get(to);
    }

    int ticketPrice(int from,int to) {
        if (from == to) {
            return 0;
        }
        if (this.refreshedpric) {
            floyd(priceTable,TRANSFERPRICE);
            this.refreshedpric = false;
        }
        return this.priceTable.get(from).get(to);
    }

    int transferCount(int from,int to) {
        if (from == to) {
            return 0;
        }
        if (this.refreshedtrans) {
            floyd(transTable,TRANSFERCOUNT);
            this.refreshedtrans = false;
        }
        return this.transTable.get(from).get(to) / TRANSFERCOUNT;
    }

    int unpValue(Path path,int from,int to) {
        return from - to + path.size();
    }

    int leastunpValue(int from,int to) {
        if (from == to) {
            return 0;
        }
        if (this.refreshedunp) {
            floyd(unpleasantTable,U_S);
            this.refreshedunp = false;
        }
        return this.unpleasantTable.get(from).get(to);
    }

    int blockCount() {
        if (this.refreshedblock) {
            int cnt = 0;
            for (Integer ikey : this.blocks.keySet()) {
                if (this.blocks.get(ikey) >= UNCONNECTEDINFO) {
                    cnt++;
                    this.blocks.put(ikey,cnt);
                    dfs(ikey,cnt);
                }
            }
            this.blockNo = cnt;
            this.refreshedblock = false;
        }
        return this.blockNo;
    }

    private void dfs(int i,int cnt) {
        for (Integer jkey : this.blocks.keySet()) {
            if (this.blocks.get(jkey) >= UNCONNECTEDINFO &&
                this.containsEdge.containsKey(new Edge(i,jkey))) {
                this.blocks.put(jkey,cnt);
                dfs(jkey,cnt);
            }
        }
    }

    private void floyd(HashMap<Integer,HashMap<Integer,Integer>> optable,
                          int offset) {
        Set<Integer> keys = this.blocks.keySet();
        for (Integer kkey: keys) {
            for (Integer ikey : keys) {
                for (Integer jkey : keys) {
                    int min = Math.min(optable.get(ikey).get(jkey),
                            optable.get(ikey).get(kkey) +
                                    optable.get(kkey).get(jkey) + offset);
                    optable.get(ikey).put(jkey,min);
                }
            }
        }
    }

    void setRefreshed() {
        refresheddis = true;
        refreshedpric = true;
        refreshedtrans = true;
        refreshedunp = true;
        refreshedblock = true;
    }
}
