package com.example.mycalendar.utils;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;

import java.util.ArrayList;
import java.util.List;

public class ActivityStackUtils extends Application {
    public static List<Activity> activities = new ArrayList<>();

    public static void addActivity(Activity activity) {
        activities.add(activity);
    }

    public static void finishAllDead() {
        for (Activity activity : activities) {
            if(!activity.isFinishing()){
                activity.finish();
            }
        }
    }

    public static void removeActivity(Activity activity) {
        activities.remove(activity);
    }
}
