package com.DBTool;

public class StringUtils {
    // 数据库名称
    public static String mDataBase = "mcalendar_db";
    //表名称
    public static String accountTableName = "accounts";
    public static String schedulesTableName = "schedules";
    //返回状态字符串
    public static String UserNotExists = "User not exists!";
    public static String WrongPassWord = "Wrong passWord!";
    public static String LoginSuccessfully = "Login successfully!";
    //http关键字索引
    public static String HttpUserNameKey = "UserName";
    public static String HttpPassWordKey = "PassWord";
    public static String HttpSaltKey = "salt";
    public static String HttpScheduleIdKey = "ScheduleId";
    public static String HttpScheduleThemeKey = "ScheduleTheme";
    public static String HttpScheduleContentKey = "ScheduleContent";
    public static String HttpScheduleDateKey = "ScheduleDate";
    public static String HttpScheduleTimeKey = "ScheduleTime";
    public static String HttpNowday = "today";
    public static String HttpNextFiveDay = "nextFiveDay";
    //表关键字索引
    public static String accoutTableUserNameKey = "username";
    public static String accoutTablePassWordKey = "password";
    public static String accoutTableSaltKey = "salt";
    public static String scheduleTableUserNameKey = "username";
    public static String scheduleTableIdKey = "id";
    public static String scheduleTableThemeKey = "theme";
    public static String scheduleTableContentKey = "content";
    public static String scheduleTableDateKey = "date";
    public static String scheduleTableTimeKey = "time";
    //bundle数据关键字
    public static String BundleScheduleKey = "schedule";
    public static String BundleAccountKey = "account";
    public static String BundleListKey = "list";
    public static String BundleStringKey = "string";
}
