<%--
  Created by IntelliJ IDEA.
  User: humoro
  Date: 2020/3/22
  Time: 16:36
  To change this template use File | Settings | File Templates.
--%>
<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<html>
<head>
    <title>Insert Account</title>
</head>
<body>
<form action="InsertNewUser" method="post">
    <tr><td>用户名</td><td><input type="text" name="UserName"></td></tr>
    <tr><td>密码</td><td><input type="password" name="PassWord"></td></tr>
    <tr><td>salt</td><td><input type="text" name="salt"></td></tr>
    <tr><td colspan="2" align="center"><input type="submit" value="登陆"/></td></tr>
</form>
</body>
</html>
