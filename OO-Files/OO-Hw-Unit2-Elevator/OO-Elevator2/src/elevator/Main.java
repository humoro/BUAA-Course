package elevator;

import com.oocourse.TimableOutput;

class Main {

    public static void main(String[] args) {
        TimableOutput.initStartTimestamp();
        RequestQueue requests = new RequestQueue();
        Floor floor = new Floor();

        ElevatorRun elevatorRuns = new ElevatorRun(floor);
        InputHandler inputHandle = new InputHandler(requests);
        ElevatorScheduler scheduler = new ElevatorScheduler(requests,
                                                            floor,
                                                            elevatorRuns);

        inputHandle.start();
        scheduler.start();
        elevatorRuns.start();
    }
}
