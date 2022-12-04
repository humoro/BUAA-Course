package elevator;

class Passenger {
    private Integer floor;
    private Integer inOrOut; // 1 in ;0 out
    private Integer id;

    Passenger(Integer destinationFr,
                    Integer inOrOut,
                    Integer id) {
        this.floor = destinationFr;
        this.inOrOut = inOrOut;
        this.id = id;
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

    Integer GetFloor() {
        return this.floor;
    }
}
