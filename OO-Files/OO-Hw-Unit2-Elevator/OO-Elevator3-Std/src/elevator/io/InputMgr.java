package elevator.io;

import com.oocourse.elevator2.ElevatorInput;
import com.oocourse.elevator2.PersonRequest;
import elevator.core.ElevatorTask;
import elevator.core.GlobalScheduler;

import java.io.IOException;

public class InputMgr implements Runnable {
    private InputMgr() {
    }

    public static InputMgr getInstance() {
        return Singleton.SINGLETON;
    }

    @Override
    public void run() {
        ElevatorInput input = new ElevatorInput(System.in);
        while (true) {
            try {
                PersonRequest request = input.nextPersonRequest();
                if (request == null) {
                    GlobalScheduler.getInstance().stop();
                    break;
                }
                GlobalScheduler.getInstance().putRequest(new ElevatorTask(
                    request.getPersonId(),
                    request.getFromFloor(),
                    request.getToFloor(),
                    true
                ));
            } catch (InterruptedException e) {
                break;
            }
        }
        try {
            input.close();
        } catch (IOException e) {
            System.err.println(e.toString());
        }
    }

    private static class Singleton {
        private static final InputMgr SINGLETON = new InputMgr();
    }
}
