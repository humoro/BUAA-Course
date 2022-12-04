package elevator.scheduler;

import elevator.core.ElevatorTask;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class ElevatorFcfsScheduler extends BaseElevatorScheduler {
    private LinkedBlockingQueue<ElevatorTask> loadQueue;
    private LinkedBlockingQueue<ElevatorTask> unloadQueue;
    private Lock putLock;

    public ElevatorFcfsScheduler() {
        loadQueue = new LinkedBlockingQueue<>();
        unloadQueue = new LinkedBlockingQueue<>();
        putLock = new ReentrantLock();
    }

    @Override
    public Lock getLock() {
        return putLock;
    }

    @Override
    public void putBehavior(ElevatorTask task) throws InterruptedException {
        putLoad(task);
    }

    private void putLoad(ElevatorTask task) throws InterruptedException {
        loadQueue.put(task);
    }

    private void putUnload(ElevatorTask task) throws InterruptedException {
        unloadQueue.put(task);
    }

    @Override
    public ElevatorTask take() throws InterruptedException {
        LinkedBlockingQueue<ElevatorTask> curQueue = getCurQueue();
        ElevatorTask task = curQueue.take();

        if (curQueue == loadQueue) {
            putUnload(ElevatorTask.getUnloadTaskFrom(task));
        }

        return task;
    }

    @Override
    public ElevatorTask peek() {
        return getCurQueue().peek();
    }

    private LinkedBlockingQueue<ElevatorTask> getCurQueue() {
        if (unloadQueue.isEmpty()) {
            return loadQueue;
        } else {
            return unloadQueue;
        }
    }

    @Override
    protected void putStopSignal() throws InterruptedException {
        put(STOP_SIGNAL);
    }
}
