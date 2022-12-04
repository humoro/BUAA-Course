package com.example.mycalendar.adapters;

import com.example.mycalendar.Schedule;

import java.util.List;

public interface FilterListener {
        void getFilterData(List<Schedule> list);// 获取过滤后的数据
}
