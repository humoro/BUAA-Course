package elevator;

import java.util.ArrayList;
import static java.lang.Math.abs;

class EleRunQueue {
    private ArrayList<FloorRequest> floorRequests = new ArrayList<>();

    EleRunQueue() {}

    synchronized void Add(FloorRequest floorRequest,int  curFr,boolean isFull) {
        int i;
        if (isFull) {
            int size = floorRequests.size();
            if (!floorRequests.isEmpty() &&
                    floorRequests.get(size - 1) == null) {
                floorRequests.add(size - 1,floorRequest);
            } else {
                floorRequests.add(floorRequest);
            }
            return;
        }
        int dst2 = abs(floorRequest.GetDstFloor() - curFr);
        for (i = 0; i < floorRequests.size(); i++) {
            FloorRequest curRq = floorRequests.get(i);
            if (curRq != null) {
                int dst1 = abs(curFr - curRq.GetDstFloor());
                if (dst1 >= dst2) {
                    break;
                }
                if (!curRq.CanBeExe()) {
                    break;
                }
            }
        }
        floorRequests.add(i,floorRequest);
    }

    synchronized boolean CheckQueue() {
        return floorRequests.size() > 0;
    }

    synchronized FloorRequest Peek() {
        return floorRequests.get(0);
    }

    synchronized void Remove() {
        floorRequests.remove(0);
    }

    synchronized ArrayList<FloorRequest> GetQueue() {
        return this.floorRequests;
    }
}