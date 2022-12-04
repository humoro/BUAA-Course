package com.example.mycalendar.adapters;

import android.annotation.SuppressLint;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;

import com.example.mycalendar.R;
import com.example.mycalendar.Schedule;
import com.example.mycalendar.viewholders.ScheduleViewHolder;

import java.util.ArrayList;
import java.util.List;

public class MainPageScheduleListViewAdapter extends BaseAdapter{ // 实现过滤器接口的方法来过滤通过搜索过滤掉的数据
    private List<Schedule> data = new ArrayList<>();
    private Context context;

    public MainPageScheduleListViewAdapter(List<Schedule> list, Context context){
        this.data = list;
        this.context = context;
    }

    @Override
    public int getCount() {
        return this.data.size();
    }

    @Override
    public Object getItem(int position) {
        return this.data.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @SuppressLint("SetTextI18n")
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ScheduleViewHolder holder = null;
        if (convertView == null) {
            convertView = LayoutInflater.from(context).inflate(R.layout.main_page_schedule_item, null);
            holder = new ScheduleViewHolder();
            holder.timeHolder = convertView.findViewById(R.id.main_page_schedule_list_view_time);
            holder.themeHoler = convertView.findViewById(R.id.main_page_schedule_list_view_theme);
            holder.contentHolder = convertView.findViewById(R.id.main_page_schedule_list_view_content);
            convertView.setTag(holder);
        }
        holder = (ScheduleViewHolder) convertView.getTag();
        Schedule schedule = (Schedule) getItem(position);
        holder.timeHolder.setText("时间：" + schedule.getTime());
        holder.themeHoler.setText("主题：" +schedule.getTheme());
        holder.contentHolder.setText("内容：" + schedule.getContent());
        return convertView;
    }
}