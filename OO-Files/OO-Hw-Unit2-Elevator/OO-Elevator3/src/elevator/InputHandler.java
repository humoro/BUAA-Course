package elevator;

import com.oocourse.elevator3.ElevatorInput;
import com.oocourse.elevator3.PersonRequest;

import java.io.IOException;

class InputHandler extends Thread {
    private ElevatorInput elevatorInput;
    private RequestQueue requests;

    InputHandler(RequestQueue requests) {
        super.setName("Input");
        elevatorInput = new ElevatorInput(System.in);
        this.requests = requests;
    }

    @Override
    public void run() {
        super.run();
        while (true) {
            PersonRequest request = elevatorInput.nextPersonRequest();
            if (request == null) {
                requests.ShutDown();
                break;
            }
            requests.Add(request);
        }
        try {
            elevatorInput.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
