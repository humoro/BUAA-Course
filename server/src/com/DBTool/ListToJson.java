package com.DBTool;

import com.Entity.Schedule;

import java.util.List;

public class ListToJson {
    public static String toJson(List<Schedule> schedules) {
        String ans = "";
        for (Schedule schedule : schedules) {
            ans +=  "\"" + schedule.getId() + "\"," +
                    "\"" + schedule.getUserName() + "\"," +
                    "\"" + schedule.getTheme() + "\"," +
                    "\"" + schedule.getContent() + "\"," +
                    "\"" + schedule.getDate() + "\";";
        }
        return ans;
    }
}
