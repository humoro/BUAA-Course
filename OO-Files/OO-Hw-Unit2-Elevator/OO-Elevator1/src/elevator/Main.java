package elevator;

import com.oocourse.TimableOutput;

class Main {
    public static void main(String[] args) {
        TimableOutput.initStartTimestamp();
        RequestQueue requests = new RequestQueue();
        RequestQueue scheduled = new RequestQueue();
        InputHandler inputHandle = new InputHandler(requests);
        inputHandle.start();
        ElevatorScheduler scheduler = new ElevatorScheduler(requests,scheduled);
        scheduler.start();
        ElevatorRun elevatorRun = new ElevatorRun(scheduled);
        elevatorRun.start();

        while (true) {
            if (!inputHandle.isAlive() &&
                !elevatorRun.isAlive() &&
                !scheduler.isAlive()) {
                System.exit(0);
            }
        }
    }
}
