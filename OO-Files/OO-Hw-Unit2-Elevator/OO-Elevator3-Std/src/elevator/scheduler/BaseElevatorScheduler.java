package elevator.scheduler;

import elevator.core.ElevatorState;
import elevator.core.ElevatorTask;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;

public abstract class BaseElevatorScheduler {
    protected static final ElevatorTask STOP_SIGNAL =
        new ElevatorTask(0, 0, 0, true);

    private Condition needUpdateCond;
    private ElevatorState curState;

    public abstract Lock getLock();

    /**
     * peek condition is a condition of getLock()
     *
     * @return needUpdateCond
     */
    protected Condition getNeedUpdateCondition() {
        if (needUpdateCond == null) {
            needUpdateCond = getLock().newCondition();
        }
        return needUpdateCond;
    }

    public void put(ElevatorTask task) throws InterruptedException {
        getLock().lockInterruptibly();
        try {
            putBehavior(task);
        } finally {
            getNeedUpdateCondition().signal();
            getLock().unlock();
        }
    }

    public abstract void putBehavior(ElevatorTask task)
        throws InterruptedException;

    public abstract ElevatorTask peek();

    public abstract ElevatorTask take() throws InterruptedException;

    public ElevatorTask blockingPeek() throws InterruptedException {
        getLock().lockInterruptibly();
        ElevatorTask task;
        try {
            task = peek();
            while (task == null) {
                getNeedUpdateCondition().await();
                task = peek();
            }
        } finally {
            getLock().unlock();
        }
        return task;
    }

    protected abstract void putStopSignal() throws InterruptedException;

    public void stop() throws InterruptedException {
        putStopSignal();
    }

    public boolean checkStopSignal(ElevatorTask task) {
        return task == STOP_SIGNAL;
    }

    public void updateCurState(ElevatorState state) {
        getLock().lock();
        try {
            curState = new ElevatorState(state);
        } finally {
            getLock().unlock();
        }
    }

    public ElevatorState getCurState() {
        return curState;
    }
}
