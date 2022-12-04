package elevator;

import java.util.Collections;
import java.util.Vector;

import static java.lang.Math.abs;

class EleRunSch extends Thread  {
    private final EleRunQueue floorRequests;
    private final Vector<Passenger> passengersPool;
    private ElevatorRun elevatorRun;
    private boolean shutDown = false;

    EleRunSch(EleRunQueue floorRequests,
              Vector<Passenger> passengersPool,
              ElevatorRun elevatorRun) {
        this.floorRequests = floorRequests;
        this.passengersPool = passengersPool;
        this.elevatorRun = elevatorRun;
    }

    @Override
    public void run() {
        while (true) {
            if (!haveNonFinishedRq()) {
                if (shutDown) {
                    elevatorRun.SetShutDown();
                    break;
                }
            } else {
                Schedule();
            }
            try {
                Thread.sleep(1);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private void Schedule() {
        int cuFr = elevatorRun.GetCurFr();
        synchronized (passengersPool) {
            Collections.sort(passengersPool);
            synchronized (floorRequests) {
                for (FloorRequest floorRequest:floorRequests.GetQueue()) {
                    for (Passenger passenger:passengersPool) {
                        if (passenger.CanBeScheduled() &&
                                !passenger.GetExeState() &&
                                passenger.GetFr() ==
                                        floorRequest.GetDstFloor()) {
                            floorRequest.Add(passenger);
                            passenger.SetScheduled(true);
                        }
                    }
                }
            }
            int minDistance = 24;
            int minIndex = -1;
            for (int i = 0; i < passengersPool.size(); i++) {
                Passenger curPs = passengersPool.get(i);
                int curTask = abs(curPs.GetFr() - cuFr);
                if (curPs.CanBeScheduled() &&
                        curPs.CanExecute() &&
                        !curPs.GetExeState()) {
                    if (curTask < minDistance) {
                        minDistance = curTask;
                        minIndex = i;
                    }
                }
            }
            if (minIndex != -1) {
                FloorRequest floorRequest = new FloorRequest(
                    passengersPool.get(minIndex).GetFr());
                for (Passenger curPs:passengersPool) {
                    if (curPs.CanBeScheduled() &&
                            !curPs.GetExeState() &&
                            curPs.GetFr() ==
                                    passengersPool.get(minIndex).GetFr()) {
                        floorRequest.Add(curPs);
                        curPs.SetScheduled(true);
                    }
                }
                floorRequests.Add(floorRequest,cuFr,elevatorRun.IsFull());
            }
        }
    }

    private boolean haveNonFinishedRq() {
        boolean bool = true;
        if (passengersPool.isEmpty()) {
            return false;
        }
        synchronized (passengersPool) {
            for (Passenger passenger:passengersPool) {
                if (passenger != null) {
                    bool &= passenger.GetExeState();
                }
            }
        }
        return !bool;
    }

    void ShutDown() {
        this.shutDown = true;
    }
}
