package elevator;

import com.oocourse.elevator2.PersonRequest;
import java.util.ArrayList;

class RequestQueue {
    private ArrayList<PersonRequest> arrayList = new ArrayList<>();

    RequestQueue() {}

    synchronized void Add(PersonRequest request) {
        arrayList.add(request);
    }

    synchronized PersonRequest Peek() {
        return arrayList.get(0);
    }

    synchronized void Remove() {
        arrayList.remove(0);
    }

    synchronized boolean CheckQueue() {
        return arrayList.size() > 0;
    }
}
