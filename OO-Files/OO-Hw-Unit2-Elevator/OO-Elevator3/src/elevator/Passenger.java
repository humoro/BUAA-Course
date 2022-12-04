package elevator;

class Passenger implements Comparable<Passenger> {
    private Integer floor;
    private Integer inOrOut; // 1 in ;0 out
    private Integer id;
    private Passenger prePs = null;
    private boolean hasExecute = false;
    private boolean canBeScheduled = true;
    private Integer eleId;

    Passenger(Integer destinationFr,
                    Integer inOrOut,
                    Integer id,
                    Integer eleId) {
        this.floor = destinationFr;
        this.inOrOut = inOrOut;
        this.id = id;
        this.eleId = eleId;
    }

    @Override
    public int compareTo(Passenger o) {
        if (o.floor.compareTo(this.floor) > 0) {
            return 1;
        } else if (o.floor.equals(this.floor)) {
            return 0;
        }
        return -1;
    }

    @Override
    public String toString() {
        String id = String.valueOf(this.id);
        String floor = String.valueOf(this.floor);
        String string = "";
        if (inOrOut == 1) {
            string = string + "IN-" + id + "-" + floor;
        } else if (inOrOut == 0) {
            string = string +  "OUT-" + id + "-" + floor;
        }
        return string;
    }

    Integer GetInOrOut() {
        return this.inOrOut;
    }

    synchronized void SetPrePs(Passenger passenger) {
        this.prePs = passenger;
    }

    synchronized boolean CanExecute() {
        if (this.prePs != null) {
            return this.prePs.GetExeState();
        } else {
            return true;
        }
    }

    synchronized boolean GetExeState() {
        return this.hasExecute;
    }

    synchronized void SetHasExecute() {
        this.hasExecute = true;
    }

    synchronized void SetScheduled(boolean bool) {
        this.canBeScheduled = !bool;
    }

    synchronized boolean CanBeScheduled() {
        return this.canBeScheduled;
    }

    synchronized int GetFr() {
        return this.floor;
    }

    synchronized Integer GetEle() {
        return this.eleId;
    }
}
