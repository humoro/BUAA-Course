package com.Entity;

import java.io.Serializable;

public class Schedule {
    private String id;
    private String userName;
    private String theme;
    private String content;
    private String date;
    private String time;
    public Schedule() {}

    public Schedule(String userName, String theme, String content, String date, String time) {
        this.id = String.valueOf(this.hashCode());
        this.userName = userName;
        this.content = content;
        this.date = date;
        this.theme = theme;
        this.time = time;
    }

    public Schedule(String id, String userName,String theme,  String content, String date, String time) {
        this.id = id;
        this.userName = userName;
        this.theme = theme;
        this.content = content;
        this.date = date;
        this.time = time;
    }

    public void setTheme(String theme) {
        this.theme = theme;
    }

    public void setContent(String content) {
        this.content = content;
    }

    public void setDate(String date) {
        this.date = date;
    }

    public void setTime(String time) {
        this.time = time;
    }

    public String getId() {
        return this.id;
    }

    public String getUserName() {
        return this.userName;
    }

    public String getTheme() {
        return this.theme;
    }

    public String getContent() {
        return this.content;
    }

    public String getDate() {
        return this.date;
    }

    public String getTime() {
        return this.time;
    }
}
