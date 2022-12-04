package elevator.core;

import java.util.ArrayList;
import java.util.concurrent.ConcurrentLinkedQueue;

public class ElevatorTaskPool {
    private final ArrayList<ConcurrentLinkedQueue<ElevatorTask>> loadPool;
    private final ArrayList<ConcurrentLinkedQueue<ElevatorTask>> unloadPool;

    public ElevatorTaskPool(int numFloor) {
        loadPool = new ArrayList<>(numFloor);
        for (int i = 0; i < numFloor; i++) {
            loadPool.add(new ConcurrentLinkedQueue<>());
        }
        unloadPool = new ArrayList<>(numFloor);
        for (int i = 0; i < numFloor; i++) {
            unloadPool.add(new ConcurrentLinkedQueue<>());
        }
    }

    private ConcurrentLinkedQueue<ElevatorTask> getQueueAt(
        ArrayList<ConcurrentLinkedQueue<ElevatorTask>> pool, int floor) {
        return pool.get(floor - 1); // index = floor-1
    }

    public boolean offer(ElevatorTask task) {
        int floor = task.getFloor();
        if (task.isLoad()) {
            return getQueueAt(loadPool, floor).offer(task);
        } else {
            return getQueueAt(unloadPool, floor).offer(task);
        }
    }

    private ElevatorTask pollLoad(int floor) {
        return getQueueAt(loadPool, floor).poll();
    }

    private ElevatorTask pollUnLoad(int floor) {
        return getQueueAt(unloadPool, floor).poll();
    }

    public ElevatorTask poll(int floor) {
        ElevatorTask unload = pollUnLoad(floor);
        if (unload != null) {
            return unload;
        } else {
            return pollLoad(floor);
        }
    }
}
