package com.Servlet;

import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import com.DBTool.DBUtils;
import com.DBTool.StringUtils;

@WebServlet("/Servlet")
public class Login extends HttpServlet {
    private static final long serialVersionUID = 1L;

    public Login() {
        super();
        // TODO Auto-generated constructor stub
    }
    /**
     * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
     */
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        // TODO Auto-generated method stub
        response.getWriter().append("Served at: ").append(request.getContextPath());
    }

    /**
     * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
     */
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        String userName = request.getParameter(StringUtils.HttpUserNameKey);
        String password= request.getParameter(StringUtils.HttpPassWordKey);
        String type = StringUtils.WrongPassWord;
        boolean haveUser = false;
        response.setContentType("text/html; charset=UTF-8");
        PrintWriter out = response.getWriter();
        try {
            Connection con= DBUtils.getConnection();
            Statement stmt=con.createStatement();
            //mysql数据库中的数据表，表名叫：demotable ，需要自己预先在数据库中进行创建，包含相应的字段和记录。
            String sql = "select * from mcalendar_db.accounts where username= '" + userName + "'";
            ResultSet rs = stmt.executeQuery(sql);
            while (rs.next()) {
                haveUser = true;
            }
            if (haveUser) { // 用户存在
                sql="select * from mcalendar_db.accounts where username= '"+userName+"' and password = '"+password + "'";
                rs=stmt.executeQuery(sql);
                while(rs.next())
                { //用户密码正确
                    type = StringUtils.LoginSuccessfully;
                }
            } else { // 用户不存在
                type = StringUtils.UserNotExists;
            }
        } catch(Exception ex) {
            ex.printStackTrace();
        } finally {
            DBUtils.Close();
            out.print(type);
            out.flush();
            out.close();
        }
    }
}
