package elevator;

import com.oocourse.elevator1.ElevatorInput;
import com.oocourse.elevator1.PersonRequest;

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
            requests.Add(request);
            if (request == null) {
                // System.out.println("exit InputHandle");
                break;
            }
            System.out.println("Input :    " + request);
        }
        try {
            elevatorInput.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
