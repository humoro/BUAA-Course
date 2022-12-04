import elevator.core.GlobalScheduler;
import elevator.io.InputMgr;
import elevator.io.OutputMgr;

public class Main {

    public static void main(String[] args) {
        new Thread(GlobalScheduler.getInstance()).start();
        OutputMgr.getInstance().init();
        new Thread(InputMgr.getInstance()).start();
    }
}
