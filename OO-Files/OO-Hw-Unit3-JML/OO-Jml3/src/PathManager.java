import com.oocourse.specs3.models.Path;

import java.util.HashMap;

class PathManager {
    private HashMap<Integer, Path> paths;
    private HashMap<Path,Integer> ids;

    PathManager(HashMap<Integer, Path> paths,
                HashMap<Path,Integer> ids) {
        this.paths = paths;
        this.ids = ids;
    }

    Path gainPath(int id) {
        return this.paths.get(id);
    }

    int gainId(Path path) {
        return this.ids.get(path);
    }

    boolean ownPath(Path path) {
        return this.ids.containsKey(path);
    }

    boolean ownId(Integer id) {
        return this.paths.containsKey(id);
    }

    int getCount() {
        return this.paths.size();
    }
}
