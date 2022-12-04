package elevator.core;

/**
 * An elevator task is sth. like "somebody in somewhere"
 */
public class ElevatorTask {
    private final int pid;
    private final int fromFloor;
    private final int toFloor;
    private final boolean isLoad;

    public ElevatorTask(int pid, int fromFloor, int toFloor, boolean isLoad) {
        this.pid = pid;
        this.fromFloor = fromFloor;
        this.toFloor = toFloor;
        this.isLoad = isLoad;
    }

    public static ElevatorTask getUnloadTaskFrom(ElevatorTask loadTask) {
        if (loadTask.isUnload()) {
            throw new Error();
        }
        return new ElevatorTask(
            loadTask.pid,
            loadTask.fromFloor,
            loadTask.toFloor,
            false
        );
    }

    /**
     * person id
     *
     * @return pid
     */
    public int getPid() {
        return pid;
    }

    /**
     * target floor.
     * <p>
     * the task must be done on given floor
     *
     * @return floor
     */
    public int getFloor() {
        if (isLoad) {
            return fromFloor;
        } else {
            return toFloor;
        }
    }

    public boolean isUpgoing() {
        return fromFloor < toFloor;
    }

    public boolean isDowngoing() {
        return !isUpgoing();
    }

    public boolean isLoad() {
        return isLoad;
    }

    public boolean isUnload() {
        return !isLoad;
    }
}
