package elevator;

import com.oocourse.elevator3.PersonRequest;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Vector;

import static java.lang.Math.abs;

class Scheduler extends Thread {
    private static int[] SleepTimeOfOneFloor = {400,500,600};
    private static final Integer IN = 1;
    private static final Integer OUT = 0;
    private static final Integer TOTAL = 3;
    private static final Integer[][] reachableFloor = {
            {-3,-2,-1,1,15,16,17,18,19,20},
            {-2,-1,1,2,4,5,6,7,8,9,10,11,12,13,14,15},
            {1,3,5,7,9,11,13,15}
    };
    private static final ArrayList<ArrayList<Integer>> reaches =
            new ArrayList<ArrayList<Integer>>() {{
            add(new ArrayList<>(Arrays.asList(reachableFloor[0])));
            add(new ArrayList<>(Arrays.asList(reachableFloor[1])));
            add(new ArrayList<>(Arrays.asList(reachableFloor[2])));
        }};

    private RequestQueue wholeQueue;
    private ElevatorRun[] elevators;
    private final Vector<Vector<Passenger>> passengersPool;
    private EleRunSch[] eleRunSchedulers;

    Scheduler(RequestQueue wholeQueue,
              ElevatorRun[] elevators,
              Vector<Vector<Passenger>> passengersPool,
              EleRunSch[] eleRunSchedulers) {
        super.setName("Schedule");
        this.wholeQueue = wholeQueue;
        this.elevators = elevators;
        this.passengersPool = passengersPool;
        this.eleRunSchedulers = eleRunSchedulers;
    }

    @Override
    public void run() {
        while (true) {
            if (wholeQueue.CheckQueue()) {
                if (wholeQueue.Peek() == null) {
                    for (int i = 0;i < TOTAL;i++) {
                        eleRunSchedulers[i].ShutDown();
                    }
                    break;
                }
                PersonRequest curRequest = wholeQueue.Peek();
                int[] curFrs = new int[TOTAL];
                for (int i = 0; i < TOTAL; i++) {
                    curFrs[i] = elevators[i].GetCurFr();
                }
                synchronized (passengersPool) {
                    ArrayList<PersonRqWithEle> requests =
                            TransferProc(curRequest,curFrs);
                    ScheduleInsert(requests);
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

    private ArrayList<PersonRqWithEle> TransferProc(PersonRequest request,
                                                  int[] curFrs) {
        ArrayList<PersonRqWithEle> requests = new ArrayList<>();
        ArrayList<Integer> bestEle = BestEle(request,curFrs);
        int fromFr = request.getFromFloor();
        int toFr = request.getToFloor();
        int matchFromEle = 0;
        for (int i = 0; i < TOTAL; i++) {
            matchFromEle = bestEle.get(i);
            if (reaches.get(matchFromEle).contains(fromFr)) {
                break;
            }
        }
        if (reaches.get(matchFromEle).contains(toFr)) {
            requests.add(new PersonRqWithEle(request,matchFromEle));
            return requests;
        } else {
            int matchToEle = 0;
            for (int i = 0; i < TOTAL; i++) {
                matchToEle = bestEle.get(i);
                if (reaches.get(matchToEle).contains(toFr)) {
                    break;
                }
            }
            int commonFr = 0;
            int minTask = 24;
            for (int i = 0; i < reaches.get(matchFromEle).size(); i++) {
                int curFr = reaches.get(matchFromEle).get(i);
                if (reaches.get(matchToEle).contains(curFr)) {
                    int task = abs(fromFr - curFr) + abs(toFr - curFr);
                    if (task < minTask) {
                        minTask = task;
                        commonFr = curFr;
                    }
                }
            }
            int id = request.getPersonId();
            PersonRequest request1 = new PersonRequest(fromFr,commonFr,id);
            PersonRequest request2 = new PersonRequest(commonFr,toFr,id);
            requests.add(new PersonRqWithEle(request1,matchFromEle));
            requests.add(new PersonRqWithEle(request2,matchToEle));
            return requests;
        }
    }

    private ArrayList<Integer> BestEle(PersonRequest request,
                                       int[] curFrs) {
        int fromFr = request.getFromFloor();
        int toFr = request.getToFloor();
        int[] tasks = new int[TOTAL];
        for (int i = 0; i < TOTAL; i++) {
            tasks[i] = (abs(fromFr - curFrs[i]) + abs(fromFr - toFr)) *
                    SleepTimeOfOneFloor[i];
        }
        int i;
        int minIndex = 0;
        int minTask = tasks[0];
        int maxIndex = 0;
        int maxTask = tasks[0];
        for (i = 1;i < TOTAL;i++) {
            if (tasks[i] < minTask) {
                minTask = tasks[i];
                minIndex = i;
            }
            if (tasks[i] >= maxTask) {
                maxTask = tasks[i];
                maxIndex = i;
            }
        }
        ArrayList<Integer> bestEle = new ArrayList<>();
        bestEle.add(minIndex);
        for (i = 0;i < TOTAL;i++) {
            if (i != maxIndex && i != minIndex) {
                break;
            }
        }
        bestEle.add(i);
        bestEle.add(maxIndex);
        ArrayList<Integer> newBest = new ArrayList<>();
        for (i = 0;i < TOTAL;i++) {
            Integer temp = bestEle.get(i);
            if (reaches.get(temp).contains(fromFr) &&
                    reaches.get(temp).contains(toFr)) {
                newBest.add(temp);
            }
        }
        for (int j = 0; j < TOTAL; j++) {
            if (!newBest.contains(bestEle.get(j))) {
                newBest.add(bestEle.get(j));
            }
        }
        return newBest;
    }

    private void ScheduleInsert(ArrayList<PersonRqWithEle> requests) {
        ArrayList<Passenger> passengers = new ArrayList<>();
        for (PersonRqWithEle request:requests) {
            int fromFr = request.GetRequest().getFromFloor();
            Passenger inPs = new Passenger(fromFr,IN,request.GetRequest().
                    getPersonId(),request.GetEle());
            int toFr = request.GetRequest().getToFloor();
            Passenger outPs = new Passenger(toFr,OUT,request.GetRequest().
                    getPersonId(),request.GetEle());
            passengers.add(inPs);
            passengers.add(outPs);
        }
        for (int i = 1;i < passengers.size();i++) {
            passengers.get(i).SetPrePs(passengers.get(i - 1));
        }
        synchronized (passengersPool) {
            for (Passenger passenger:passengers) {
                passengersPool.get(passenger.GetEle()).add(passenger);
            }
        }
    }
}