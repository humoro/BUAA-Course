package com.example.mycalendar;

import java.util.regex.Pattern;

public class StringUtils {
    public static String mDataBase = "mcalendar_db";
    public static String accountTableName = "accounts";
    public static String schedulesTableName = "schedules";
    public static String UserNotExists = "User not exists!";
    public static String WrongPassWord = "Wrong passWord!";
    public static String LoginSuccessfully = "Login successfully!";
    public static String HttpUserNameKey = "UserName";
    public static String HttpPassWordKey = "PassWord";
    public static String HttpSaltKey = "salt";
    public static String accoutTableUserNameKey = "username";
    public static String accoutTablePassWordKey = "password";
    public static String accoutTableSaltKey = "salt";
    public static String BundleScheduleKey = "schedule";
    public static String BundleAccountKey = "account";
    public static String BundleListKey = "list";

    public static boolean checkEmailFormat(String userName) {
        String regex = "^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$";
        return Pattern.matches(regex, userName);
    }
}
