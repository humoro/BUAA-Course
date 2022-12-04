package test;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Main {
    private ArrayList<String> Transform(ArrayList<String> strings) {
        ArrayList<String> newStrings = new ArrayList<>();
        double lastTime = 0;
        double newTime;
        double thisTime;
        String regCon = "\\d+\\.\\d+";
        Pattern pattern1 = Pattern.compile(regCon);
        for (String string:strings) {
            Matcher matcher = pattern1.matcher(string);
            if (matcher.find()) {
                thisTime = Double.parseDouble(matcher.group());
                newTime = thisTime - lastTime;
                lastTime = thisTime;
                String repTime = Double.toString(newTime);
                string = string.replaceAll(Double.toString(thisTime),repTime);
                newStrings.add(string);
            }
        }
        return newStrings;
    }
    private void Println(ArrayList<String> strings) {
        String regCon = "\\d+\\.\\d+";
        Pattern pattern = Pattern.compile(regCon);
        for (String string : strings) {
            Matcher matcher = pattern.matcher(string);
            if (matcher.find()) {
                Double time = Double.parseDouble(matcher.group());
                long sleepTime = (new Double(time * 1000)).longValue();
                try {
                    Thread.sleep(sleepTime);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                System.out.println(string.replaceAll("\\[" + time.toString() + "]",""));
            }
        }
    }
    public static void main(String[] args) {
        Main main = new Main();
        ArrayList<String> strings = new ArrayList<>();
        String name = "C:/Users/hp/desktop/Test/test.txt";
        try {
            FileReader fr = new FileReader(name);
            BufferedReader bf = new BufferedReader(fr);
            String str;
            while ((str = bf.readLine())!=null) {
                strings.add(str);
            }
            bf.close();
            fr.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        strings = main.Transform(strings);
        main.Println(strings);
    }
}
