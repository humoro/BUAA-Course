package elevator.io;

import com.oocourse.TimableOutput;

public class OutputMgr {

    private OutputMgr() {
    }

    public static OutputMgr getInstance() {
        return Singleton.SINGLETON;
    }

    public void init() {
        TimableOutput.initStartTimestamp();
    }

    private void putMsgToQueue(String msg) {
        TimableOutput.println(msg);
    }

    private void personStateUpdate(int pid, int floor, String state) {
        putMsgToQueue(String.format("%s-%d-%d", state, pid, floor));
    }

    private void elevatorStateUpdate(int eid, int floor, String state) {
        putMsgToQueue(String.format("%s-%d", state, floor));
    }

    public void personOut(int pid, int floor) {
        personStateUpdate(pid, floor, "OUT");
    }

    public void personIn(int pid, int floor) {
        personStateUpdate(pid, floor, "IN");
    }

    public void elevatorOpenBegin(int eid, int floor) {
        elevatorStateUpdate(eid, floor, "OPEN");
    }

    public void elevatorCloseEnd(int eid, int floor) {
        elevatorStateUpdate(eid, floor, "CLOSE");
    }

    public void elevatorArrive(int eid, int floor) {
        elevatorStateUpdate(eid, floor, "ARRIVE");
    }

    private static class Singleton {
        private static final OutputMgr SINGLETON = new OutputMgr();
    }
}
