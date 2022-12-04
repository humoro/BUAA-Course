package elevator;

import java.util.ArrayList;

class Destination {
    private Integer destination;
    private ArrayList<Passenger> passengers = new ArrayList<>();

    Destination(Integer destination) {
        this.destination = destination;
    }

    Integer GetDestination() {
        return this.destination;
    }

    synchronized ArrayList<Passenger> GetPassengers() {
        return this.passengers;
    }

    synchronized void Add(Passenger passenger) {
        passengers.add(passenger);
    }
}
