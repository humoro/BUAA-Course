package com.example.mycalendar.utils;

import android.util.Log;

import com.example.mycalendar.Schedule;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.ArrayList;
import java.util.List;

public class DataBaseUtils {
    private static String mDriver = "com.mysql.jdbc.Driver";
    private static String mDataBaseName = "mcalendar_db";
    private static String mUserName = "humoro";// 用户名
    private static String mUserPassword = "990907";// 密码
    private static String mAccountTableName = "ACCOUNTS";
    private static String mScheduleTableName = "SCHEDULES";
    private static String mIp = "192.168.1.106";
    public static int QPASSWORD = 0;
    public static int QSALT = 1;
    private static String qPasswordKey = "password";
    private static String qSaltKey = "salt";
    private static String qUserNameKey = "userName";
    private static String ScheduleIdKey = "id";
    private static String ScheduleUserNameKey = "userName";
    private static String ScheduleThemeKey = "theme";
    private static String ScheduleContentKey = "content";
    private static String ScheduleTimeKey = "time";

    private static Connection connectToDB(String dbName){
        Connection connection = null;
        try{
            Class.forName(mDriver);// 动态加载类
            // 写成本机地址，不能写成localhost，同时手机和电脑连接的网络必须是同一个
            // 尝试建立到给定数据库URL的连接
            connection = DriverManager.getConnection("jdbc:mysql://" + mIp + ":3306/" + dbName, mUserName, mUserPassword);
        }catch (Exception e){
            e.printStackTrace();
        }
        return connection;
    }

    public static String getUserAccountInfo(String userName, int qType) {
        String answer = "";
        // 根据数据库名称，建立连接
        Connection connection = connectToDB(mDataBaseName);
        try {
            if (qType != QPASSWORD && qType != QSALT) throw new Exception("invalid query type!");
            String sql = "select * from " + mAccountTableName +  " where " + qUserNameKey + " = " + userName;
            if (connection != null){// connection不为null表示与数据库建立了连接
                PreparedStatement ps = connection.prepareStatement(sql);
                if (ps != null){
                    ResultSet resultSet = ps.executeQuery();
                    if (resultSet != null){
                        int count = resultSet.getMetaData().getColumnCount();
                        Log.e("DataBaseUtils","列总数：" + count);
                        while (resultSet.next()){
                            if (qType == QPASSWORD)
                                answer = resultSet.getString(qPasswordKey);
                            else if (qType == QSALT)
                                answer = resultSet.getString(qSaltKey);
                            else throw new Exception("invalid query type!");
                        }
                        ps.close();
                        connection.close();
                        resultSet.close();
                        return answer;
                    }else {
                        connection.close();
                        ps.close();
                        return null;
                    }
                }else {
                    connection.close();
                    throw new Exception("Prepare Statement Failed!");
                }
            }else {
                throw new Exception("Connect DataBase Failed!");
            }
        } catch (Exception e){
            e.printStackTrace();
            Log.e("DataBaseUtils","异常：" + e.getMessage());
            return null;
        }
    }

    public static boolean isDuplicateUserName(String userName) { // 注册用户名是否重复
        Connection connection = connectToDB(mDataBaseName);
        try{
            String sql = "select * from " + mAccountTableName + " where " + qUserNameKey + " = " + userName;
            if (connection != null) {
                PreparedStatement ps = connection.prepareStatement(sql);
                if (ps != null) {
                    boolean ans = false;
                    ResultSet resultSet = ps.executeQuery();
                    while (resultSet.next()) {
                        if (resultSet.getString(qUserNameKey) != null) ans = true;
                    }
                    connection.close();
                    ps.close();
                    return ans;
                } else {
                    connection.close();
                    throw new Exception("Prepare Statement Failed!");
                }
            } else {
                throw new Exception("Connect DataBase Failed!");
            }
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    public static void insertUserAccount(String userName, String userPassword, String userSalt) { // 插入新的账户
        Connection connection = connectToDB(mDataBaseName);
        try{
            String sql = "INSERT INTO " + mAccountTableName + " VALUES(\'" + userName + "\',\'" + userPassword + "\',\'" + userSalt + "\')";
            if (connection != null) {
                PreparedStatement ps = connection.prepareStatement(sql);
                if (ps != null) {
                    boolean insertAns = ps.execute();
                    if (!insertAns) throw new Exception("Insert Item Failed!");
                    ps.close();
                    connection.close();
                } else {
                    connection.close();
                    throw new Exception("Prepare Statement Failed!");
                }
            } else {
                throw new Exception("Connect DataBase Failed!");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // 日程的增删改查函数
    public  static void insertSchedule(Schedule schedule) { // 插入日程
        Connection connection = connectToDB(mDataBaseName);
        try {
            if (connection != null) {
                String id = schedule.getId();
                String userName = schedule.getUserName();
                String theme = schedule.getTheme() == null? "NULL" : schedule.getTheme();
                String content = schedule.getContent();
                String date = schedule.getDate();
                String sql = "INSERT INTO " + mScheduleTableName + " VALUES(\'" + id + "\',\'" +
                                userName + "\',\'" + theme + "\',\'" + content + "\',\'" + date + "\')";
                PreparedStatement ps = connection.prepareStatement(sql);
                if (ps != null) {
                    boolean succ = ps.execute();
                    if (!succ) throw new Exception("Insert Failed!");
                    ps.close();
                    connection.close();
                } else {
                    connection.close();
                    throw new Exception("Prepare Statement Failed!");
                }
            } else {
                throw new Exception("Connect DataBase Failed!");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    } // 增加新的日程

    public  static List<Schedule> searchScheduleByUserName(String userName) {
        Connection connection = connectToDB(mDataBaseName);
        List<Schedule> list = null;
        try {
            if (connection != null) {
                String sql = "SELECT * FROM " + mScheduleTableName + " WHERE "  +
                            ScheduleUserNameKey + " = \'" + userName + "\'";
                PreparedStatement ps = connection.prepareStatement(sql);
                if (ps != null) {
                    ResultSet resultSet = ps.executeQuery();
                    if (resultSet != null) {
                        list = new ArrayList<>();
                        while (resultSet.next()) {
                            list.add(new Schedule(resultSet.getString(ScheduleIdKey), resultSet.getString(ScheduleUserNameKey),
                                    resultSet.getString(ScheduleThemeKey),resultSet.getString(ScheduleContentKey),resultSet.getString(ScheduleTimeKey)));
                        }
                        resultSet.close();
                    } else throw new Exception("Query Failed!");
                    ps.close();
                    connection.close();
                } else {
                    connection.close();
                    throw new Exception("Prepare Statement Failed!");
                }
            } else {
                throw new Exception("Connect DataBase Failed!");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return list;
    } // 查找某用户的全部日程

    public  static List<Schedule> searchScheduleByUserNameADate(String userName, String time) {
        Connection connection = connectToDB(mDataBaseName);
        List<Schedule> list = null;
        try {
            if (connection != null) {
                String sql = "SELECT * FROM " + mScheduleTableName + " WHERE " +
                            ScheduleUserNameKey + " = \'" + userName + "\' AND " +
                            ScheduleTimeKey + " = \'" + time + "\'";
                PreparedStatement ps = connection.prepareStatement(sql);
                if (ps != null) {
                    ResultSet resultSet = ps.executeQuery();
                    if (resultSet != null) {
                        list = new ArrayList<>();
                        while (resultSet.next()) {
                            list.add(new Schedule(resultSet.getString(ScheduleIdKey), resultSet.getString(ScheduleUserNameKey),
                                    resultSet.getString(ScheduleThemeKey),resultSet.getString(ScheduleContentKey),resultSet.getString(ScheduleTimeKey)));
                        }
                        resultSet.close();
                    } else throw new Exception("Query Failed!");
                    ps.close();
                    connection.close();
                } else {
                    connection.close();
                    throw new Exception("Prepare Statement Failed!");
                }
            } else {
                throw new Exception("Connect DataBase Failed!");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return list;
    } // 查找某一天的某用户的全部日程

    public static void deleteScheduleById(String id) {
        Connection connection = connectToDB(mDataBaseName);
        try {
            if (connection != null) {
                String sql = "DELETE FROM " + mScheduleTableName + " WHERE ID = \'" + id + "\'";
                PreparedStatement ps = connection.prepareStatement(sql);
                if (ps != null) {
                    boolean succ = ps.execute();
                    if (!succ) throw new Exception("Delete Failed!");
                    ps.close();
                    connection.close();
                } else {
                    connection.close();
                    throw new Exception("Prepare Statement Failed!");
                }
            } else {
                throw new Exception("Connect DataBase Failed!");
            }
        }catch (Exception e) {
            e.printStackTrace();
        }
    } // 删除某个日程

    public  static void updateScheduleById(Schedule schedule) {
        Connection connection = connectToDB(mDataBaseName);
        try {
            if (connection != null) {
                String sql = "UPDATE " + mScheduleTableName + " SET " +
                            ScheduleThemeKey + " = \'" + schedule.getTheme() + "\'," +
                            ScheduleContentKey + " = \'" + schedule.getContent() + "\'," +
                            ScheduleTimeKey + " = \'" + schedule.getDate() + "\' WHERE " +
                            ScheduleIdKey + "= \'" + schedule.getId() + "\'";
                PreparedStatement ps = connection.prepareStatement(sql);
                if (ps != null) {
                    boolean succ = ps.execute();
                    if (!succ) throw new Exception("Update Failed!");
                    ps.close();
                    connection.close();
                } else {
                    connection.close();
                    throw new Exception("Prepare Statement Failed!");
                }
            } else {
                throw new Exception("Connect DataBase Failed!");
            }
        }catch (Exception e) {
            e.printStackTrace();
        }
    } //更新某个日程的内容
}

