package com.Servlet;

import com.DBTool.DBUtils;
import com.DBTool.StringUtils;
import com.Entity.Schedule;
import com.alibaba.fastjson.JSON;
import com.google.gson.Gson;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

public class ScheduleGetByUserNameAndDate extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        String userName = request.getParameter(StringUtils.HttpUserNameKey);
        String time = request.getParameter(StringUtils.HttpScheduleDateKey);
        response.setContentType("text/html; charset=UTF-8");
        PrintWriter out = response.getWriter();
        String type = "";
        try {
            Connection con= DBUtils.getConnection();
            Statement stmt=con.createStatement();
            //mysql数据库中的数据表，表名叫：demotable ，需要自己预先在数据库中进行创建，包含相应的字段和记录。
            String sql = "select * from " + StringUtils.schedulesTableName + " where "
                    + StringUtils.scheduleTableUserNameKey + " = '" + userName + "' and "
                    + StringUtils.scheduleTableDateKey + " = '" + time + "'";
            ResultSet rs = stmt.executeQuery(sql);
            ArrayList<Schedule> schedules = new ArrayList<>();
            if (rs != null) {
                while (rs.next()) {
                    schedules.add(new Schedule( rs.getString(StringUtils.scheduleTableIdKey),
                                                rs.getString(StringUtils.scheduleTableUserNameKey),
                                                rs.getString(StringUtils.scheduleTableThemeKey),
                                                rs.getString(StringUtils.scheduleTableContentKey),
                                                rs.getString(StringUtils.scheduleTableDateKey),
                                                rs.getString(StringUtils.scheduleTableTimeKey)));
                }
            }
            Gson gson = new Gson();
            type = gson.toJson(schedules);
        } catch(Exception ex) {
            ex.printStackTrace();
        }  finally {
            DBUtils.Close();
            out.print(type);
            out.flush();
            out.close();
        }
    }
}
