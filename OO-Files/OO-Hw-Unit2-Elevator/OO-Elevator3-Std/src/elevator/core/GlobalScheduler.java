package elevator.core;

import java.util.ArrayList;
import java.util.concurrent.LinkedBlockingQueue;

public class GlobalScheduler implements Runnable {
    private static final ElevatorTask STOP_SIGNAL =
        new ElevatorTask(0, 0, 0, true);
    private static final int NUM_ELEVATORS = 1;
    private final ArrayList<Elevator> elevators;
    private LinkedBlockingQueue<ElevatorTask> requests;

    private GlobalScheduler() {
        requests = new LinkedBlockingQueue<>();
        elevators = new ArrayList<>(NUM_ELEVATORS);
        for (int i = 0; i < NUM_ELEVATORS; i++) {
            elevators.add(new Elevator(i));
        }
    }

    public static GlobalScheduler getInstance() {
        return Singleton.SINGLETON;
    }

    public void putRequest(ElevatorTask request)
        throws InterruptedException {
        requests.put(request);
    }

    private void distributeRequest(ElevatorTask request)
        throws InterruptedException {
        Elevator first = elevators.get(0);
        first.putTask(request);
    }

    public void stop() throws InterruptedException {
        putRequest(STOP_SIGNAL);
    }

    private void stopElevators() throws InterruptedException {
        for (Elevator elevator :
            elevators) {
            elevator.stop();
        }
    }

    @Override
    public void run() {
        for (Elevator elevator :
            elevators) {
            new Thread(elevator).start();
        }
        while (true) {
            try {
                ElevatorTask nextRequest = requests.take();
                if (nextRequest == STOP_SIGNAL) {
                    stopElevators();
                    break;
                }
                distributeRequest(nextRequest);
            } catch (InterruptedException e) {
                break;
            }
        }
    }

    private static class Singleton {
        private static final GlobalScheduler SINGLETON = new GlobalScheduler();
    }
}
