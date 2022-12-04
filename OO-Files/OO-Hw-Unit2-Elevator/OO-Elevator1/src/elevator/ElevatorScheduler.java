package elevator;

class ElevatorScheduler extends Thread {
    private RequestQueue wholeQueue;
    private RequestQueue schedulerQueue;

    ElevatorScheduler(RequestQueue wholeQueue,
                      RequestQueue schedulerQueue) {
        super.setName("Schedule");
        this.wholeQueue = wholeQueue;
        this.schedulerQueue = schedulerQueue;
    }

    @Override
    public void run() {
        while (true) {
            if (wholeQueue.CheckQueue()) {
                schedulerQueue.Add(wholeQueue.Peek());
                if (wholeQueue.Peek() == null) {
                    // System.out.println("exit scheduler");
                    break;
                }
                System.out.println("scheduler :   " + wholeQueue.Peek());
                wholeQueue.Remove();
            }
        }
        // System.out.println("exit scheduler of true in schedule!");
    }
}
