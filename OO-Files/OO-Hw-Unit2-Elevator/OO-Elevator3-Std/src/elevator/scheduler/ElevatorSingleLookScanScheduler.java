package elevator.scheduler;

import elevator.core.ElevatorState;
import elevator.core.ElevatorTask;

import java.util.PriorityQueue;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class ElevatorSingleLookScanScheduler extends BaseElevatorScheduler {
    private final PriorityQueue<ElevatorTask> queue1;
    private final PriorityQueue<ElevatorTask> queue2;
    private final Lock lock;
    private boolean stopped = false;
    private PriorityQueue<ElevatorTask> current;

    public ElevatorSingleLookScanScheduler(int initCapacity) {
        lock = new ReentrantLock();
        queue1 = new PriorityQueue<>(initCapacity / 2,
            ElevatorSingleLookScanScheduler::compare);
        queue2 = new PriorityQueue<>(initCapacity / 2,
            ElevatorSingleLookScanScheduler::compare);
        current = queue1;
    }

    private static int compare(ElevatorTask t1, ElevatorTask t2) {
        if (t1 == STOP_SIGNAL) {
            return 1;
        }
        if (t2 == STOP_SIGNAL) {
            return -1;
        }
        return t1.getFloor() - t2.getFloor();
    }

    private void updateCurrentQueue() {
        getLock().lock();
        try {
            if (current.isEmpty()) {
                switchQueue();
            }
        } finally {
            lock.unlock();
        }
    }

    private void switchQueue() {
        getLock().lock();
        try {
            if (current == queue1) {
                current = queue2;
            } else {
                current = queue1;
            }
        } finally {
            getLock().unlock();
        }
    }

    @Override
    public Lock getLock() {
        return lock;
    }

    private PriorityQueue<ElevatorTask> getAnotherQueue() {
        if (current == queue1) {
            return queue2;
        } else {
            return queue1;
        }
    }

    @Override
    public void putBehavior(ElevatorTask task) throws InterruptedException {
        lock.lockInterruptibly();
        try {
            if (task == STOP_SIGNAL) {
                getAnotherQueue().add(task);
                return;
            }
            ElevatorState state = getCurState();
            if (state.isDowngoing()) {
                current.add(task);
                return;
            }

            // is upgoing
            if (task.getFloor() < state.getFloor()) {
                getAnotherQueue().add(task);
            } else if (task.getFloor() > state.getFloor()) {
                current.add(task);
            } else {
                // state at cur floor
                if (state.isUpgoing()) {
                    getAnotherQueue().add(task);
                } else {
                    current.add(task);
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
            ElevatorTask task = current.peek();
            if (task == null && stopped) {
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
        lock.lockInterruptibly();
        try {
            updateCurrentQueue();
            while (peek() == null) {
                if (stopped = true) {
                    return STOP_SIGNAL;
                } else {
                    getNeedUpdateCondition().await();
                }
            }
            ElevatorTask task = current.poll();
            if (task != null && task.isLoad()) {
                put(ElevatorTask.getUnloadTaskFrom(task));
            }
            return task;
        } finally {
            lock.unlock();
        }
    }

    @Override
    protected void putStopSignal() throws InterruptedException {
        lock.lockInterruptibly();
        try {
            stopped = true;
            getNeedUpdateCondition().signal();
        } finally {
            lock.unlock();
        }
    }
}
