public class Edge {
    private int from;
    private int to;

    Edge(int from,int to) {
        this.from = from;
        this.to = to;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        } else if (!(obj instanceof Edge)) {
            return false;
        } else {
            return this.from == ((Edge) obj).from &&
                    this.to == ((Edge) obj).to;
        }
    }

    @Override
    public int hashCode() {
        int result = 17;
        result = result * 31 + Integer.hashCode(this.from);
        result = result * 31 + Integer.hashCode(this.to);
        return result;
    }
}
