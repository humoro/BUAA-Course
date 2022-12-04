package elevator;

import com.oocourse.elevator1.PersonRequest;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

class RequestQueue {
    private List<PersonRequest> arrayList =
            Collections.synchronizedList(new ArrayList<>());

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
