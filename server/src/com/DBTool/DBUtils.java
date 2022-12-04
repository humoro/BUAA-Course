package com.DBTool;


import java.sql.*;

public class DBUtils {
    //其中mysql是数据库名称，在mysql57版本的数据库中已经预先新建完成；3306是mysql数据库的端口号。
    private static String url="jdbc:mysql://localhost:3306/" + StringUtils.mDataBase + "?useSSL=false&allowPublicKeyRetrieval=true&autoReconnect=true";
    //com.mysql.jdbc.Driver是mysql-connector-java-5.1.40中的驱动包路径
    private static String driverClass="com.mysql.jdbc.Driver";
    //mysql的账号和密码是在安装mysql中进行设置的，这里拿来用即可。
    private static String username="humoro";
    private static String password="990907";
    private static Connection conn;
    //装载驱动
    static{
        try{
            Class.forName(driverClass);
        }
        catch(ClassNotFoundException e){
            e.printStackTrace();
        }
    }
    //获取数据库连接
    public static Connection getConnection(){
        try{
            conn=DriverManager.getConnection(url,username,password);
        }
        catch(SQLException e){
            e.printStackTrace();
        }
        return conn;
    }
    //建立数据库连接
    public static void main(String[] args){
        Connection conn= DBUtils.getConnection();
        if(conn!=null){
            System.out.println("数据库连接成功");
        }
        else{
            System.out.println("数据库连接失败");
        }
    }
    //关闭数据库连接
    public static void Close(){
        if(conn!=null){
            try{
                conn.close();
            }
            catch(SQLException e){
                e.printStackTrace();
            }
        }
    }
}
