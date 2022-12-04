package elevator;

import com.oocourse.elevator2.PersonRequest;

class ElevatorScheduler extends Thread {
    private static final Integer UP = 1;
    private static final Integer STILL = 0;
    private static final Integer DOWN = -1;
    private static final Integer IN = 1;
    private static final Integer OUT = 0;

    private RequestQueue wholeQueue;
    private ElevatorRun elevatorRuns;
    private Floor floor;

    ElevatorScheduler(RequestQueue wholeQueue,
                      Floor floor,
                      ElevatorRun elevatorRuns) {
        super.setName("Schedule");
        this.wholeQueue = wholeQueue;
        this.floor = floor;
        this.elevatorRuns = elevatorRuns;
    }

    @Override
    public void run() {
        while (true) {
            if (wholeQueue.CheckQueue()) {
                if (wholeQueue.Peek() == null) {
                    floor.ShutDown();
                    break;
                }
                PersonRequest curPerson = wholeQueue.Peek();
                int fromFloor = curPerson.getFromFloor();
                int toFloor = curPerson.getToFloor();
                int curFloor = elevatorRuns.GetCurFr();
                int curDirection = elevatorRuns.GetCurDr();
                int direction = DOWN;
                if (fromFloor < toFloor) {
                    direction = UP;
                }
                Integer[] hasMatch = isMatchAls(direction,fromFloor,
                                                toFloor,curDirection,
                                                curFloor);
                int curId = curPerson.getPersonId();
                Passenger fromServe = new Passenger(fromFloor,IN,curId);
                Passenger toServe = new Passenger(toFloor,OUT,curId);
                if (curDirection == UP ||
                        (curDirection == STILL && ((fromFloor > curFloor) ||
                                (fromFloor == curFloor &&
                                        toFloor > fromFloor)))) {
                    floor.InsertUp(fromServe,hasMatch[0],1);
                    floor.InsertUp(toServe,hasMatch[1],0);
                } else {
                    floor.InsertDn(fromServe,hasMatch[0],1);
                    floor.InsertDn(toServe,hasMatch[1],0);
                }
                wholeQueue.Remove();
            } else {
                try {
                    Thread.sleep(1);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private Integer[] isMatchAls(int direction,int fromFloor,
                                 int toFloor,int curDirection,
                                 int curFloor) {
        int hasMatchIn = 0;
        int hasMatchOut = 0;
        Integer[] hasMatch = {0,0};
        if (((curDirection == UP && fromFloor >= curFloor) ||
                (curDirection == DOWN  && fromFloor <= curFloor)) ||
                (curDirection == STILL)) {
            hasMatchIn = 1;
            if (direction == curDirection ||
                    (curDirection == STILL &&
                            ((fromFloor >= curFloor && toFloor > fromFloor) ||
                            (fromFloor <= curFloor && toFloor < fromFloor)))) {
                hasMatchOut = 1;
            }
        } else if (direction == curDirection) {
            hasMatchOut = 2;
        }
        hasMatch[0] = hasMatchIn;
        hasMatch[1] = hasMatchOut;
        return hasMatch;
    }
}