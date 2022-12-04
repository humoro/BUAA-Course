import com.oocourse.specs1.AppRunner;

public class Main {
    public static void main(String[] args) throws Exception {
        long start = System.currentTimeMillis();
        AppRunner runner = AppRunner.newInstance(MyPath.class,
                MyPathContainer.class);
        runner.run(args);
        long end = System.currentTimeMillis();
        System.out.println(start);
        System.out.println(end);
        System.out.println(end - start);
    }
}
