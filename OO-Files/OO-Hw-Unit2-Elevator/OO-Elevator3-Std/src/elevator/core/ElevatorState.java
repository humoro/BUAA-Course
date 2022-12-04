package elevator.core;

public class ElevatorState {
    private int id;
    private int floor;
    private int targetFloor;
    private int load;
    private boolean doorOpen;
    private RunningState runningState;

    public ElevatorState(int id, int floor, int load, boolean doorOpen,
                         RunningState runningState, int targetFloor) {
        this.id = id;
        this.floor = floor;
        this.load = load;
        this.doorOpen = doorOpen;
        this.runningState = runningState;
        this.targetFloor = targetFloor;
    }

    public ElevatorState(ElevatorState old) {
        this(old.id, old.floor, old.load, old.doorOpen, old.runningState,
            old.targetFloor);
    }

    public int getId() {
        return id;
    }

    public int getFloor() {
        return floor;
    }

    public void setFloor(int floor) {
        this.floor = floor;
    }

    public void upgoingOneFloor() {

    }

    public void downgoingOneFloor() {

    }

    public boolean isDoorOpen() {
        return doorOpen;
    }

    public void setDoorOpen(boolean doorOpen) {
        this.doorOpen = doorOpen;
    }

    public boolean isDoorClosed() {
        return !doorOpen;
    }

    public int getTargetFloor() {
        return targetFloor;
    }

    public void setTargetFloor(int targetFloor) {
        this.targetFloor = targetFloor;
    }

    public int getLoad() {
        return load;
    }

    public void setLoad(int load) {
        this.load = load;
    }

    public boolean isIdle() {
        return runningState.equals(RunningState.IDLE);
    }

    public boolean isLoading() {
        return runningState.equals(RunningState.LOADING);
    }

    private boolean isMoving() {
        return runningState.equals(RunningState.MOVING);
    }

    public boolean isUpgoing() {
        return targetFloor > floor;
    }

    public boolean isDowngoing() {
        return targetFloor < floor;
    }

    public RunningState getRunningState() {
        return runningState;
    }

    public void setRunningState(RunningState runningState) {
        this.runningState = runningState;
    }

    public enum RunningState {
        IDLE,
        MOVING,
        LOADING
    }
}
