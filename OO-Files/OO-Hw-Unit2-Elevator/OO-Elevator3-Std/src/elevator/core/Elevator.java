package elevator.core;

import elevator.io.OutputMgr;
import elevator.scheduler.BaseElevatorScheduler;
import elevator.scheduler.ElevatorLookScanScheduler;

public class Elevator implements Runnable {
    private static final long DELAY_OPEN = 200;
    private static final long DELAY_CLOSE = 200;
    private static final long DELAY_UP_PER_FLOOR = 400;
    private static final long DELAY_DOWN_PER_FLOOR = 400;
    private static final int INIT_FLOOR = 1;
    private static final int CAPACITY = -1;
    private final int id;
    private ElevatorState state;
    private BaseElevatorScheduler scheduler;

    Elevator(int id) {
        this.id = id;
        state = new ElevatorState(id, INIT_FLOOR, 0, false,
            ElevatorState.RunningState.IDLE, INIT_FLOOR);
        scheduler = new ElevatorLookScanScheduler(16);
        sendStateToScheduler();
    }

    private void moveOneFloor() throws InterruptedException {
        if (state.isUpgoing()) {
            upgoingOneFloor();
        } else if (state.isDowngoing()) {
            downgoingOneFloor();
        }
    }

    private void sendStateToScheduler() {
        scheduler.updateCurState(state);
    }

    private void updateStateAccordingTo(ElevatorTask nextTask)
        throws InterruptedException {
        setTargetFloor(nextTask.getFloor());
        if (state.isUpgoing() || state.isDowngoing()) {
            if (state.isDoorOpen()) {
                closeDoorIfNot();
                return;
            } else {
                moveOneFloor();
                return;
            }
        }
        if (state.isDoorClosed()) {
            openDoorIfNot();
            return;
        }

        updatePersonOnceAccordingTo(nextTask);
    }

    /**
     * upgoing to next floor, move to curFloor + 1
     */
    private void upgoingOneFloor() throws InterruptedException {
        Thread.sleep(DELAY_UP_PER_FLOOR);
        if (state.getFloor() == -1) {
            state.setFloor(1);
        } else {
            state.setFloor(state.getFloor() + 1);
        }
        OutputMgr.getInstance().elevatorArrive(id, state.getFloor());
    }

    private void downgoingOneFloor() throws InterruptedException {
        Thread.sleep(DELAY_DOWN_PER_FLOOR);
        if (state.getFloor() == 1) {
            state.setFloor(-1);
        } else {
            state.setFloor(state.getFloor() - 1);
        }
        OutputMgr.getInstance().elevatorArrive(id, state.getFloor());
    }

    /**
     * open the door.
     * if it is already open, do nothing
     * <p>
     * NOTICE: sleep of BOTH open & close will appear in this method.
     *
     * @throws InterruptedException
     */
    private void openDoorIfNot() throws InterruptedException {
        if (state.isDoorClosed()) {
            OutputMgr.getInstance().elevatorOpenBegin(id, state.getFloor());
            state.setDoorOpen(true);
            Thread.sleep(DELAY_OPEN);
            Thread.sleep(DELAY_CLOSE);
        }
    }

    /**
     * close the door.
     * if it is already closed, do nothing
     * <p>
     * NOTICE: sleep of close will NOT appear in this method.
     *
     * @throws InterruptedException of sleep
     */
    private void closeDoorIfNot() throws InterruptedException {
        if (state.isDoorOpen()) {
            state.setDoorOpen(false);
            OutputMgr.getInstance().elevatorCloseEnd(id, state.getFloor());
        }
    }

    public void putTask(ElevatorTask task) throws InterruptedException {
        scheduler.put(task);
    }

    /**
     * not include door operation.
     * so open the door outside the method, plz.
     * also close the door plz.
     *
     * @param task just a task
     */
    private void updatePersonOnceAccordingTo(ElevatorTask task)
        throws InterruptedException {
        if (task.getFloor() != state.getFloor()) {
            throw new Error();
        }
        if (state.isDoorClosed()) {
            throw new Error();
        }
        if (!checkAndTakeNext(task)) {
            return;
        }
        if (task.isLoad()) {
            OutputMgr.getInstance().personIn(task.getPid(), task.getFloor());
        } else if (task.isUnload()) {
            OutputMgr.getInstance().personOut(task.getPid(), task.getFloor());
        }
    }

    /**
     * check whether the given task is the next task of scheduler
     * if it is, take it.
     *
     * @param task the given task
     * @return YES I AM
     */
    private boolean checkAndTakeNext(ElevatorTask task)
        throws InterruptedException {
        scheduler.getLock().lock();
        boolean success = false;
        ElevatorTask nextTask = scheduler.peek();
        if (task == nextTask) {
            scheduler.take();
            success = true;
        }
        scheduler.getLock().unlock();
        return success;
    }

    /**
     * upgoing to next floor, move to curFloor - 1
     */
    private void setTargetFloor(int targetFloor) {
        state.setTargetFloor(targetFloor);
    }

    public ElevatorState getState() {
        return state;
    }

    public void stop() throws InterruptedException {
        scheduler.stop();
    }

    @Override
    public void run() {
        while (true) {
            try {
                ElevatorTask currentNextTask = scheduler.blockingPeek();
                if (scheduler.checkStopSignal(currentNextTask)) {
                    closeDoorIfNot();
                    break;
                }
                updateStateAccordingTo(currentNextTask);
                sendStateToScheduler();
            } catch (InterruptedException e) {
                break;
            }
        }
    }
}
