package elevator;

import com.oocourse.TimableOutput;

import java.util.ArrayList;

class ElevatorRun extends Thread {
    private static final Integer UP = 1;
    private static final Integer IN = 1;
    private static final Integer OUT = 0;
    private static final Integer STILL = 0;
    private static final Integer DOWN = -1;
    private static final long StayTime = 400;

    private Integer curFr = 1;
    private Integer direction = STILL;
    private final EleRunQueue floorRequests;
    private long oneFloorSleepTime;
    private Character id;
    private Integer curNumPs = 0;
    private Integer weightLimit;
    private boolean shutDown = false;

    ElevatorRun(EleRunQueue floorRequests,Character id,Integer weightLimit,
                long oneFloorSleepTime) {
        this.floorRequests = floorRequests;
        this.id = id;
        this.oneFloorSleepTime = oneFloorSleepTime;
        this.weightLimit = weightLimit;
        super.setName("Run ---- " + id);
    }

    @Override
    public void run() {
        while (true) {
            if (floorRequests.CheckQueue()) {
                FloorRequest floorRequest = floorRequests.Peek();
                EleRun(floorRequest);
            } else {
                if (shutDown) {
                    break;
                }
                this.direction = STILL;
            }
            try {
                Thread.sleep(1);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private synchronized void EleRun(FloorRequest floorRequest) {
        FloorRequest floorRequest1 = floorRequest;
        int dstFr = floorRequest1.GetDstFloor();
        //System.out.println(this.id + "
        // dstFr  " + floorRequest.GetDstFloor());
        while (this.curFr != dstFr) {
            SetDirection(dstFr);
            //System.out.println(this.id + "   curFr  " + this.curFr);
            if (this.curFr != 0) {
                try {
                    wait(this.oneFloorSleepTime);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            Arrive();
            floorRequest1 = floorRequests.Peek();
            dstFr = floorRequest1.GetDstFloor();
            //System.out.println(this.id + "
            // dstFr  " + floorRequest.GetDstFloor());
            if (this.curFr == dstFr) {
                break;
            }
        }
        ExPassengers(floorRequest1);
        try {
            wait(StayTime);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        Close();
        /*for (FloorRequest floorRequest2:floorRequests.GetQueue()) {
            System.out.println(this.id);
            for (Passenger passenger:floorRequest2.GetPassengers()) {
                System.out.print(passenger + "   ");
            }
            System.out.print('\n');
            System.out.println(this.id + "    " + this.curNumPs);
        }*/

    }

    private void Arrive() {
        this.curFr += this.direction;
        if (this.curFr == 0) {
            this.curFr += this.direction;
        }
        TimableOutput.println(String.format("ARRIVE-%d-%c",this.curFr,this.id));
    }

    private void Open() {
        TimableOutput.println(String.format("OPEN-%d-%c",this.curFr,this.id));
    }

    private synchronized void ExPassengers(FloorRequest floorRequest) {
        ArrayList<Passenger> passengers = floorRequest.GetPassengers();
        synchronized (floorRequests) {
            Open();
            for (Passenger curPassenger:passengers) {
                if (curPassenger.CanExecute()) {
                    if (curPassenger.GetInOrOut().equals(OUT)) {
                        TimableOutput.println(curPassenger + "-" + this.id);
                        curPassenger.SetHasExecute();
                        this.curNumPs--;
                    }
                } else {
                    curPassenger.SetScheduled(false);
                }
            }
            for (Passenger curPassenger:passengers) {
                if (curPassenger.CanExecute()) {
                    if (curPassenger.GetInOrOut().equals(IN) &&
                            this.curNumPs < weightLimit) {
                        TimableOutput.println(curPassenger + "-" + this.id);
                        curPassenger.SetHasExecute();
                        this.curNumPs++;
                    } else {
                        curPassenger.SetScheduled(false);
                    }
                } else {
                    curPassenger.SetScheduled(false);
                }
            }
            floorRequests.Remove();
        }
    }

    private void Close() {
        TimableOutput.println(String.format("CLOSE-%d-%c",this.curFr,this.id));
    }

    private synchronized void SetDirection(int destinationFr) {
        if (destinationFr >= this.curFr) {
            this.direction = UP;
        } else {
            this.direction = DOWN;
        }
    }

    synchronized Integer GetCurFr() {
        return this.curFr;
    }

    synchronized boolean IsFull() {
        return this.curNumPs >= this.weightLimit;
    }

    synchronized void SetShutDown() {
        this.shutDown = true;
    }

}
