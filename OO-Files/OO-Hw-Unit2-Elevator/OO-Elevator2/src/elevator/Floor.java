package elevator;

import java.util.ArrayList;

class Floor {
    private ArrayList<Destination> destinations = new ArrayList<>();

    Floor() {}

    synchronized void InsertUp(Passenger passenger,int isMatch,int isIn) {
        int size = destinations.size();
        if (size == 0) {
            InsertEmptyList(passenger);
        }
        else {
            int i;
            for (i = 1; i < destinations.size(); i++) {
                if (destinations.get(i).GetDestination() <
                        destinations.get(i - 1).GetDestination()) {
                    break;
                }
            }
            int maxIndex0 = i - 1;
            int maxFr0 = destinations.get(i - 1).GetDestination();
            for (; i < destinations.size(); i++) {
                if (destinations.get(i).GetDestination() >
                        destinations.get(i - 1).GetDestination()) {
                    break;
                }
            }
            int maxIndex1 = i - 1;
            int maxFr1 = destinations.get(i - 1).GetDestination();
            if (isMatch == 1) {
                InsertUpList(0, maxIndex0 + 1, maxFr0, passenger);
            } else if ((isIn == 1) || ((isIn == 0) && isMatch == 0)) {
                InsertDnList(maxIndex0, maxIndex1 + 1, maxFr1, passenger);
            } else {
                InsertUpList(maxIndex1, size,
                        destinations.get(size - 1).GetDestination(), passenger);
            }
        }
        /*for (Destination destination:destinations) {
            for (Passenger passenger1:destination.GetPassengers()) {
                System.out.println("InsertUp :  " + passenger1);
            }
        }
        System.out.println("InsertUp is Match and is In: "
            + isMatch + "    " + isIn + "  " + passenger);
        System.out.print('\n');*/
    }

    synchronized void InsertDn(Passenger passenger,int isMatch,int isIn) {
        int size = destinations.size();
        if (size == 0) {
            InsertEmptyList(passenger);
        }
        else {
            int i;
            for (i = 1;i < destinations.size();i++) {
                if (destinations.get(i).GetDestination() >
                        destinations.get(i - 1).GetDestination()) {
                    break;
                }
            }
            int minIndex0 = i - 1;
            int minFr0 = destinations.get(i - 1).GetDestination();
            for (;i < destinations.size();i++) {
                if (destinations.get(i).GetDestination() <
                        destinations.get(i - 1).GetDestination()) {
                    break;
                }
            }
            int minIndex1 = i - 1;
            int minFr1 = destinations.get(i - 1).GetDestination();
            if (isMatch == 1) {
                InsertDnList(0,minIndex0 + 1,minFr0,passenger);
            } else if ((isIn == 1) || (isIn == 0 && isMatch == 0)) {
                InsertUpList(minIndex0,minIndex1 + 1,minFr1,passenger);
            } else {
                InsertDnList(minIndex1, size,
                        destinations.get(size - 1).GetDestination(), passenger);
            }
        }
        /*for (Destination destination:destinations) {
            for (Passenger passenger1:destination.GetPassengers()) {
                System.out.println("InsertDown :  " + passenger1);
            }
        }
        System.out.println("InsertDn is Match and is In: "
            + isMatch + "    " + isIn + "  " + passenger);
        System.out.print('\n');*/
    }

    private void InsertEmptyList(Passenger passenger) {
        Destination destIn = new Destination(passenger.GetFloor());
        destIn.Add(passenger);
        destinations.add(destIn);
        //System.out.println("EmptyInsert " + passenger);
    }

    private void InsertUpList(int start,int end,int maxFr,
                                           Passenger passenger) {
        if (maxFr < passenger.GetFloor()) {
            Destination destination = new Destination(passenger.GetFloor());
            destination.Add(passenger);
            destinations.add(end,destination);
            return;
        }
        int indexEq = end;
        int indexBigger = start;
        /*System.out.println("Up List start : " + start + " end : "
            + end + " passenger : " + passenger);*/
        for (int i = start;i < end;i++) {
            if (destinations.get(i).GetDestination()
                    .equals(passenger.GetFloor())) {
                indexEq = i;
            }
            if (destinations.get(i).GetDestination() < passenger.GetFloor()) {
                indexBigger = i + 1;
            }
        }
        if (indexEq < end) {
            //System.out.println("Up rep passenger : " + passenger);
            destinations.get(indexEq).Add(passenger);
        } else {
            Destination destination = new Destination(passenger.GetFloor());
            destination.Add(passenger);
            destinations.add(indexBigger,destination);
        }
    }

    private void InsertDnList(int start,int end,int minFr,
                                           Passenger passenger) {
        if (minFr > passenger.GetFloor()) {
            Destination destination = new Destination(passenger.GetFloor());
            destination.Add(passenger);
            destinations.add(end,destination);
            return;
        }
        int indexEq = end;
        int indexSmaller = start;
        /*System.out.println("Dn List start : " + start + " end : " +
             end + " passenger : " + passenger);*/
        for (int i = start;i < end;i++) {
            if (destinations.get(i).GetDestination()
                    .equals(passenger.GetFloor())) {
                indexEq = i;
            }
            if (destinations.get(i).GetDestination() > passenger.GetFloor()) {
                indexSmaller = i + 1;
            }
        }
        if (indexEq < end) {
            //System.out.println("Down rep passenger : " + passenger);
            destinations.get(indexEq).Add(passenger);
        } else {
            Destination destination = new Destination(passenger.GetFloor());
            destination.Add(passenger);
            destinations.add(indexSmaller,destination);
        }
    }

    synchronized boolean CheckQueue() {
        return destinations.size() > 0;
    }

    synchronized Destination Peek() {
        return destinations.get(0);
    }

    synchronized void Remove() {
        destinations.remove(0);
    }

    synchronized void ShutDown() {
        this.destinations.add(null);
    }
}
