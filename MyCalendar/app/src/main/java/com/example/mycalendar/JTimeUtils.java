package com.example.mycalendar;


import java.util.Calendar;
import java.util.HashMap;

public class JTimeUtils {
    static String getDateString(Calendar calendar) {
        int year = calendar.get(Calendar.YEAR);
        int month = calendar.get(Calendar.MONTH) + 1;
        int day = calendar.get(Calendar.DAY_OF_MONTH);
        return year + "-" + ((month < 10)? "0" + month :String.valueOf(month)) + "-"  + ((day < 10)? "0" + day :String.valueOf(day));
    }

    static String getMonthAbbreviation(int month) {
        HashMap<Integer, String> map = new HashMap<Integer, String>(){
            {
                put(1, "Jan.");
                put(2, "Feb.");
                put(3, "Mar.");
                put(4, "Apr.");
                put(5, "May.");
                put(6, "Jun.");
                put(7, "Jul.");
                put(8, "Aug.");
                put(9, "Sep.");
                put(10, "Oct.");
                put(11, "Nov.");
                put(12, "Dec.");
            }
        };
        return map.get(month);
    }

    static String getWeekAbbreviation(int weekday) {
        HashMap<Integer, String> map = new HashMap<Integer, String>(){
            {
                put(0, "Sun.");
                put(1, "Mon.");
                put(2, "Tue.");
                put(3, "Wed.");
                put(4, "Thu.");
                put(5, "Fri.");
                put(6, "Sat.");
            }
        };
        return map.get(weekday);
    }

    static String getMonthName(int month) {
        HashMap<Integer, String> map = new HashMap<Integer, String>(){
            {
                put(1, "一月");
                put(2, "二月");
                put(3, "三月");
                put(4, "四月");
                put(5, "五月");
                put(6, "六月");
                put(7, "七月");
                put(8, "八月");
                put(9, "九月");
                put(10, "十月");
                put(11, "十一月");
                put(12, "十二月");
            }
        };
        return map.get(month);
    }
}