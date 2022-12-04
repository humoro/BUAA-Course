package elevator.scheduler;

import elevator.core.ElevatorState;
import elevator.core.ElevatorTask;
import org.junit.Assert;
import org.junit.Test;

public class ElevatorLookScanSchedulerTest {
    static ElevatorTask checkPeekAndGetTake(BaseElevatorScheduler scheduler)
        throws InterruptedException {
        ElevatorTask peeked = scheduler.peek();
        ElevatorTask taken = scheduler.take();
        Assert.assertEquals(peeked, taken);
        return taken;
    }

    static void assertTaskFloor(ElevatorTask task, int expFloor) {
        Assert.assertEquals(expFloor, task.getFloor());
    }

    @Test
    public void test() {
        ElevatorLookScanScheduler scheduler = new ElevatorLookScanScheduler(16);
        scheduler.updateCurState(
            new ElevatorState(
                0, 1, 0, false, ElevatorState.RunningState.IDLE, 1));
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
            assertTaskFloor(checkPeekAndGetTake(scheduler), 4);
            assertTaskFloor(checkPeekAndGetTake(scheduler), 4);
            assertTaskFloor(checkPeekAndGetTake(scheduler), 1);
            Assert.assertNull(scheduler.peek());
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}