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

public class SearchListViewAdapter extends BaseAdapter implements Filterable { // 实现过滤器接口的方法来过滤通过搜索过滤掉的数据
    private List<Schedule> list = new ArrayList<>();
    private Context context;
    private MyFilter filter = null;
    private FilterListener listener = null;

    public SearchListViewAdapter(List<Schedule> list, Context context, FilterListener filterListener){
        this.list = list;
        this.context = context;
        this.listener = filterListener;
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
            convertView = LayoutInflater.from(context).inflate(R.layout.schedule_item, null);
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

    @Override
    public Filter getFilter() {
        if (filter == null) {
            filter = new MyFilter(list);
        }
        return filter;
    }

    public class MyFilter extends Filter{
        private List<Schedule> original = new ArrayList<>();

        public MyFilter(List<Schedule> list) {
            this.original = list;
        }

        @Override
        protected FilterResults performFiltering(CharSequence constraint) { //这是搜索过滤的主要执行方法
            FilterResults results = new FilterResults();
            if (TextUtils.isEmpty(constraint)) {
                results.values = original;
                results.count = original.size();
            } else {
                List<Schedule> mList = new ArrayList<>();
                for (Schedule schedule : original) {
                        if (schedule.getTheme().trim().toLowerCase().contains(constraint.toString().trim().toLowerCase())) {
                            mList.add(schedule);
                        } else {
                            if (schedule.getContent().trim().toLowerCase().contains(constraint.toString().trim().toLowerCase())) {
                                mList.add(schedule);
                            }
                        }
                }
                results.values = mList;
                results.count = mList.size();
            }
            return results;
        }

        //刷新列表
        @Override
        protected void publishResults(CharSequence constraint, FilterResults results) {
            // 获取过滤后的数据
            list = (List<Schedule>) results.values;
            // 如果接口对象不为空，那么调用接口中的方法获取过滤后的数据，具体的实现在new这个接口的时候重写的方法里执行
            if(listener != null){
                listener.getFilterData(list);
            }
            // 刷新数据源显示
            notifyDataSetChanged();
        }
    }
}
