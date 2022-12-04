package elevator;

import com.oocourse.TimableOutput;
import java.util.Vector;

class Main {
    private static long[] SleepTimeOfOneFloor = {400,500,600};
    private static Integer[] WeightLimit = {6,8,7};

    public static void main(String[] args) {
        //----------------pre definitions-----------------------//
        TimableOutput.initStartTimestamp();
        int totalEle = 3;
        //--------------creat thread classes-------------------//
        RequestQueue requests = new RequestQueue();
        Vector<Vector<Passenger>> passengers = new Vector<>();
        InputHandler inputHandle = new InputHandler(requests);
        EleRunQueue[] runQueues = new EleRunQueue[totalEle];
        ElevatorRun[] elevators = new ElevatorRun[totalEle];
        EleRunSch[] eleRunSchedulers = new EleRunSch[totalEle];
        Character id = 'A';//the id of every elevator
        for (int i = 0; i < totalEle; i++,id++) {
            Vector<Passenger> curPs = new Vector<>();
            passengers.add(curPs);
            runQueues[i] = new EleRunQueue();
            elevators[i] = new ElevatorRun(runQueues[i],id,WeightLimit[i],
                                            SleepTimeOfOneFloor[i]);
            eleRunSchedulers[i] = new EleRunSch(runQueues[i],curPs,
                                                elevators[i]);
        }
        Scheduler scheduler = new Scheduler(requests,elevators,
                                            passengers,eleRunSchedulers);
        //---------------start all threads----------------------//
        for (int i = 0; i < totalEle; i++) {
            elevators[i].start();
            eleRunSchedulers[i].start();
        }
        inputHandle.start();
        scheduler.start();
    }
}
