package elevator;

import java.util.ArrayList;

class FloorRequest {
    private Integer dstFloor;
    private ArrayList<Passenger> passengers = new ArrayList<>();

    FloorRequest(Integer dstFloor) {
        this.dstFloor = dstFloor;
    }

    Integer GetDstFloor() {
        return this.dstFloor;
    }

    synchronized ArrayList<Passenger> GetPassengers() {
        return this.passengers;
    }

    synchronized void Add(Passenger passenger) {
        passengers.add(passenger);
    }

    synchronized boolean CanBeExe() {
        boolean bool = false;
        for (Passenger passenger:passengers) {
            bool |= passenger.CanExecute();
        }
        return bool;
    }

}
