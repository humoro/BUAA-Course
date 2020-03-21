package com.example.mycalendar;

import java.io.Serializable;

class UserAccount  implements Serializable {
    private String userName;
    private String passWord;
    private String salt;
    private String encryptPassword;

    UserAccount(String userName, String passWord) {
        this.userName = userName;
        this.passWord = passWord;
    }

    UserAccount(String userName, String encryptPassword, String salt) {
        this.userName = userName;
        this.encryptPassword = encryptPassword;
        this.salt = salt;
    }

    void setEncryptPassword(String passWord) {
        this.encryptPassword = passWord;
    }

    void setSalt(String salt) {
        this.salt = salt;
    }

    String getUserName() {
        return this.userName;
    }

    String getPassWord() {
        return this.passWord;
    }

    String getSalt() {
        return this.salt;
    }

    String getEncryptPassword() {
        return this.encryptPassword;
    }
}
