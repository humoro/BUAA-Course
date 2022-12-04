package elevator;

import com.oocourse.TimableOutput;
import com.oocourse.elevator1.PersonRequest;

import static java.lang.Math.abs;

class ElevatorRun extends Thread {
    private Integer curFloor = 1;
    private RequestQueue requestQueue;

    ElevatorRun(RequestQueue requestQueue) {
        super.setName("Run");
        this.requestQueue = requestQueue;
    }

    @Override
    public void run() {
        int startFloor;
        int endFloor;
        int id;
        long sleepTime1;
        long sleepTime2;
        while (true) {
            if (requestQueue.CheckQueue()) {
                PersonRequest personRequest = requestQueue.Peek();
                if (personRequest == null) {
                    // System.out.println("exit Run");
                    break;
                }
                System.out.println("Run request :  " + requestQueue.Peek());
                startFloor = personRequest.getFromFloor();
                endFloor = personRequest.getToFloor();
                id = personRequest.getPersonId();
                sleepTime1 = abs(startFloor - this.curFloor) * 500;
                this.curFloor = endFloor;
                sleepTime2 = abs(endFloor - startFloor) * 500;
                try {
                    Thread.sleep(sleepTime1);
                    TimableOutput.println(
                            String.format("OPEN-%d", startFloor));
                    Thread.sleep(10);
                    TimableOutput.println(
                            String.format("IN-%d-%d", id, startFloor));
                    Thread.sleep(490);
                    TimableOutput.println(
                            String.format("CLOSE-%d", startFloor));
                    Thread.sleep(sleepTime2);
                    TimableOutput.println(
                            String.format("OPEN-%d", endFloor));
                    Thread.sleep(10);
                    TimableOutput.println(
                            String.format("OUT-%d-%d", id, endFloor));
                    Thread.sleep(490);
                    TimableOutput.println(
                            String.format("CLOSE-%d", endFloor));
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                requestQueue.Remove();
            }
        }
        // System.out.println("exit out of true in run");
    }
}
