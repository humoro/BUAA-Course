package elevator;

import com.oocourse.TimableOutput;
import java.util.ArrayList;

class ElevatorRun extends Thread {
    private static final Integer UP = 1;
    private static final Integer STILL = 0;
    private static final Integer DOWN = -1;

    private Integer curFloor = 1;
    private Integer direction = STILL;
    private Floor requests;

    ElevatorRun(Floor requests) {
        this.requests = requests;
        super.setName("Run");
    }

    @Override
    public void run() {
        while (true) {
            if (this.requests.CheckQueue()) {
                if (this.requests.Peek() == null) {
                    break;
                }
                RunEle();
            } else {
                this.direction = STILL;
                try {
                    Thread.sleep(1);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private synchronized void RunEle() {
        Destination destination = this.requests.Peek();
        int destFloor = destination.GetDestination();
        SetDirection(destFloor);
        while (this.curFloor != destFloor) {
            if (this.curFloor != 0) {
                try {
                    wait(400);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            destination = this.requests.Peek();
            destFloor = destination.GetDestination();
            if (this.curFloor == destFloor) {
                break;
            }
            this.curFloor += this.direction;
            if (this.curFloor != 0) {
                TimableOutput.println(
                        String.format("ARRIVE-%d",this.curFloor));
            }
        }
        TimableOutput.println(
                String.format("OPEN-%d",this.curFloor));
        try {
            wait(400);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        destination = this.requests.Peek();
        ArrayList<Passenger> passengers = destination.GetPassengers();
        for (Passenger curPassenger:passengers) {
            TimableOutput.println(curPassenger);
        }
        TimableOutput.println(
                String.format("CLOSE-%d",this.curFloor));
        this.requests.Remove();
    }

    private synchronized void SetDirection(int destinationFr) {
        if (destinationFr >= this.curFloor) {
            this.direction = UP;
        } else {
            this.direction = DOWN;
        }
    }

    synchronized Integer GetCurFr() {
        return this.curFloor;
    }

    synchronized Integer GetCurDr() {
        return this.direction;
    }
}
