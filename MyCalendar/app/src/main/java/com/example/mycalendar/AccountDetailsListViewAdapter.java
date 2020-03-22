package com.example.mycalendar;

import android.content.Context;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Filter;
import android.widget.Filterable;

import java.util.ArrayList;
import java.util.List;

public class AccountDetailsListViewAdapter extends BaseAdapter { // 实现过滤器接口的方法来过滤通过搜索过滤掉的数据
    private List<Schedule> list = new ArrayList<>();
    private Context context;

    public AccountDetailsListViewAdapter(List<Schedule> list, Context context){
        this.list = list;
        this.context = context;
    }

    @Override
    public int getCount() {
        return this.list.size();
    }

    @Override
    public Object getItem(int position) {
        return this.list.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ScheduleViewHolder holder = null;
        if (convertView == null) {
            convertView = LayoutInflater.from(context).inflate(R.layout.all_schedule_item, null);
            holder = new ScheduleViewHolder();
            holder.dateHolder = convertView.findViewById(R.id.schedule_list_view_date);
            holder.themeHoler = convertView.findViewById(R.id.schedule_list_view_theme);
            holder.contentHolder = convertView.findViewById(R.id.schedule_list_view_content);
            convertView.setTag(holder);


        }
        holder = (ScheduleViewHolder) convertView.getTag();
        Schedule schedule = (Schedule) getItem(position);
        holder.dateHolder.setText("时间: " + schedule.getDate());
        holder.themeHoler.setText("主题: " +schedule.getTheme());
        holder.contentHolder.setText("内容: " + schedule.getContent());
        return convertView;
    }
}
