package elevator.scheduler;

import elevator.core.ElevatorState;
import elevator.core.ElevatorTask;
import org.junit.Assert;
import org.junit.Test;

public class ElevatorSingleLookScanSchedulerTest {

    private void assertTaskFloor(ElevatorTask task, int floor) {
        ElevatorLookScanSchedulerTest.assertTaskFloor(task, floor);
    }

    private ElevatorTask checkPeekAndGetTake(BaseElevatorScheduler scheduler)
        throws InterruptedException {
        return ElevatorLookScanSchedulerTest.checkPeekAndGetTake(scheduler);
    }

    private void setStateFloor(ElevatorState state, int floor) {
        state.setFloor(floor);
    }

    private void setStateDst(ElevatorState state, int dst) {
        state.setTargetFloor(dst);
    }

    @Test
    public void case1() {
        ElevatorState state = new ElevatorState(
            0, 1, 0, false, ElevatorState.RunningState.IDLE, 1);
        ElevatorSingleLookScanScheduler scheduler
            = new ElevatorSingleLookScanScheduler(16);
        scheduler.updateCurState(state);

        try {
            scheduler.put(new ElevatorTask(1, 1, 3, true));
            scheduler.put(new ElevatorTask(2, 2, 3, true));
            scheduler.put(new ElevatorTask(3, 4, 1, true));
            scheduler.put(new ElevatorTask(4, 1, 4, true));
            assertTaskFloor(checkPeekAndGetTake(scheduler), 1);
            assertTaskFloor(checkPeekAndGetTake(scheduler), 1);
            assertTaskFloor(checkPeekAndGetTake(scheduler), 2);
            assertTaskFloor(checkPeekAndGetTake(scheduler), 3);
            assertTaskFloor(checkPeekAndGetTake(scheduler), 3);
            setStateDst(state, 4);
            setStateFloor(state, 4);
            scheduler.updateCurState(state);
            assertTaskFloor(checkPeekAndGetTake(scheduler), 4);
            assertTaskFloor(checkPeekAndGetTake(scheduler), 4);
            assertTaskFloor(checkPeekAndGetTake(scheduler), 1);
            Assert.assertNull(scheduler.peek());
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}