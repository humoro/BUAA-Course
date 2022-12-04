package elevator.scheduler;

import elevator.core.ElevatorState;
import elevator.core.ElevatorTask;

import java.util.PriorityQueue;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class ElevatorLookScanScheduler extends BaseElevatorScheduler {
    private final PriorityQueue<ElevatorTask> upQueue;
    private final PriorityQueue<ElevatorTask> downQueue;
    private final ReentrantLock lock;
    private PriorityQueue<ElevatorTask> currentQueue;
    private boolean stopped;

    public ElevatorLookScanScheduler(int initCapacity) {
        upQueue = new PriorityQueue<>(initCapacity / 2,
            ElevatorLookScanScheduler::lessThan);
        downQueue = new PriorityQueue<>(initCapacity / 2,
            ElevatorLookScanScheduler::greaterThan);
        currentQueue = upQueue;
        lock = new ReentrantLock();
        stopped = false;
    }

    private static int lessThan(ElevatorTask t1, ElevatorTask t2) {
        return t1.getFloor() - t2.getFloor();
    }

    private static int greaterThan(ElevatorTask t1, ElevatorTask t2) {
        return t2.getFloor() - t1.getFloor();
    }

    public PriorityQueue<ElevatorTask> getUpQueue() {
        return upQueue;
    }

    public PriorityQueue<ElevatorTask> getDownQueue() {
        return downQueue;
    }

    public PriorityQueue<ElevatorTask> getCurrentQueue() {
        return currentQueue;
    }

    public void setCurrentQueue(PriorityQueue<ElevatorTask> currentQueue) {
        this.currentQueue = currentQueue;
    }

    protected void switchQueue() {
        lock.lock();
        try {
            if (currentQueue == upQueue) {
                currentQueue = downQueue;
            } else {
                currentQueue = upQueue;
            }
        } finally {
            lock.unlock();
        }
    }

    private void updateCurrentQueue() {
        lock.lock();
        try {
            if (currentQueue.isEmpty()) {
                switchQueue();
            }
        } finally {
            lock.unlock();
        }
    }

    private void putDown(ElevatorTask task) {
        downQueue.offer(task);
    }

    private void putUp(ElevatorTask task) {
        upQueue.offer(task);
    }

    @Override
    public Lock getLock() {
        return lock;
    }

    @Override
    public void putBehavior(ElevatorTask task) throws InterruptedException {
        lock.lockInterruptibly();
        try {
            ElevatorState state = getCurState();
            int curFloor = state.getFloor();
            if (curFloor < task.getFloor()) {
                putUp(task);
            } else if (curFloor > task.getFloor()) {
                putDown(task);
            } else if (curFloor == task.getFloor()) {
                if (state.isUpgoing()) {
                    putDown(task);
                } else if (state.isDowngoing()) {
                    putUp(task);
                } else {
                    if (currentQueue == upQueue) {
                        putUp(task);
                    } else {
                        putDown(task);
                    }
                }
            }
        } finally {
            lock.unlock();
        }
    }

    @Override
    public ElevatorTask peek() {
        lock.lock();
        try {
            updateCurrentQueue();
            ElevatorTask task = currentQueue.peek();
            if (task == null && stopped) {
                currentQueue.offer(STOP_SIGNAL);
                return STOP_SIGNAL;
            } else {
                return task;
            }
        } finally {
            lock.unlock();
        }
    }

    @Override
    public ElevatorTask take() throws InterruptedException {
        ElevatorTask task;
        lock.lockInterruptibly();
        try {
            updateCurrentQueue();
            task = currentQueue.poll();
            while (task == null) {
                if (stopped) {
                    return STOP_SIGNAL;
                }
                getNeedUpdateCondition().await();
                task = currentQueue.poll();
            }
            if (task.isLoad()) {
                put(ElevatorTask.getUnloadTaskFrom(task));
            }
            return task;
        } finally {
            lock.unlock();
        }
    }

    @Override
    protected void putStopSignal() throws InterruptedException {
        stopped = true;
        lock.lockInterruptibly();
        try {
            getNeedUpdateCondition().signal();
        } finally {
            lock.unlock();
        }
    }
}
