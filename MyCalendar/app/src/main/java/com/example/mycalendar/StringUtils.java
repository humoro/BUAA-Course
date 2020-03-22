package com.example.mycalendar;

import java.util.regex.Pattern;

public class StringUtils {
    public static String BundleScheduleKey = "schedule";
    public static String BundleAccountKey = "account";
    public static String BundleListKey = "list";

    public static boolean checkEmailFormat(String userName) {
        String regex = "^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$";
        return Pattern.matches(regex, userName);
    }
}
