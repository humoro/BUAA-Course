package com.example.mycalendar;

import java.io.Serializable;

public class UserAccount  implements Serializable {
    private String userName;
    private String passWord;
    private String salt;
    private String encryptPassword;

    public UserAccount(String userName, String passWord) {
        this.userName = userName;
        this.passWord = passWord;
    }

    public UserAccount(String userName, String encryptPassword, String salt) {
        this.userName = userName;
        this.encryptPassword = encryptPassword;
        this.salt = salt;
    }

    public void setEncryptPassword(String passWord) {
        this.encryptPassword = passWord;
    }

    public void setSalt(String salt) {
        this.salt = salt;
    }

    public String getUserName() {
        return this.userName;
    }

    public String getPassWord() {
        return this.passWord;
    }

    public String getSalt() {
        return this.salt;
    }

    public String getEncryptPassword() {
        return this.encryptPassword;
    }
}
