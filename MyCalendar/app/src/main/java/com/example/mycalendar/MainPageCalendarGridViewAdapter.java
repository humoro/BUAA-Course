package com.example.mycalendar;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Color;
import android.util.Log;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

import static android.content.ContentValues.TAG;


public class MainPageCalendarGridViewAdapter extends BaseAdapter {

    private Activity activity;
    private Calendar calendar = Calendar.getInstance();
    private Calendar calStartDate = Calendar.getInstance();
    private List<Date> showDates = new ArrayList<Date>();
    private int iMonthViewCurrentMonth = 0;
    private int mGridWidth = 0;
    public int selectPos = -1;
    public int selectDay = -1;
    public int todayPos = -1;
    // construct
    public MainPageCalendarGridViewAdapter(Activity a, Calendar showCalendar) {
        activity = a;
        calStartDate = showCalendar;
        calendar.setTime(showCalendar.getTime());
        getDates();
        getGridWidth();
    }

    private void UpdateStartDateForMonth() {
        calStartDate.set(Calendar.DATE, 1); // 设置成当月第一天
        iMonthViewCurrentMonth = calStartDate.get(Calendar.MONTH);// 得到当前日历显示的月
        // 星期一是2 星期天是1 填充剩余天数
        int iDay = calStartDate.get(Calendar.DAY_OF_WEEK) - Calendar.SUNDAY;
        calStartDate.add(Calendar.DAY_OF_WEEK, -iDay);
    }

    private void getDates() {
        UpdateStartDateForMonth();

        for (int i = 1; i <= 42; i++) {
            showDates.add(calStartDate.getTime());
            calStartDate.add(Calendar.DAY_OF_MONTH, 1);
        }
    }

    @SuppressWarnings("deprecation")
    private void getGridWidth() {

        Display display = activity.getWindowManager().getDefaultDisplay();
        int mDisplayWidth = display.getWidth();
        float scale = activity.getResources().getDisplayMetrics().density;

        mGridWidth = (mDisplayWidth - (int) (46 * scale)) / 7;// 46dp位距离两边的margin
    }

    @Override
    public int getCount() {
        return showDates.size();
    }

    @Override
    public Object getItem(int position) {
        return showDates.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @SuppressLint("ResourceAsColor")
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        DayViewHolder holder;
        if (convertView == null) {
            convertView = LayoutInflater.from(activity).inflate(R.layout.main_page_day_item, null);
            holder = new DayViewHolder();
            holder.itemDay = convertView.findViewById(R.id.stc_cal_grid_item_day);
            holder.itemLayout =  convertView.findViewById(R.id.stc_cal_grid_item_layout);
            convertView.setTag(holder);
        } else {
            holder = (DayViewHolder) convertView.getTag();
        }
        final Date myDate = (Date) getItem(position);
        Calendar calCalendar = Calendar.getInstance();
        calCalendar.setTime(myDate);

        final int iMonth = calCalendar.get(Calendar.MONTH);
        // final int iDay = calCalendar.get(Calendar.DAY_OF_WEEK);
        int day = myDate.getDate(); // 日期
        //Log.d(TAG, "getView: " + position);
        holder.itemDay.setText(String.valueOf(day));
        int todayMonth = Calendar.getInstance().get(Calendar.MONTH);
        if (todayMonth == calendar.get(Calendar.MONTH)) {
            if (iMonth == iMonthViewCurrentMonth && equalsDate(Calendar.getInstance().getTime(), myDate) && todayPos < 0) {
                todayPos = position;
                //Log.d(TAG, "getView: this is adapter initial today layout");
                holder.itemLayout.setBackgroundResource(R.drawable.stroke_ovel);
                holder.itemDay.setTextColor(R.color.ltGray);
            }
        }
        if (iMonth == iMonthViewCurrentMonth && equalsDate(calendar.getTime(), myDate)) {
            // 当前日期
            selectPos = position;
            //Log.d(TAG, "getView: this is adapter select date" + myDate.getDate() + " " + calendar.get(Calendar.DAY_OF_MONTH));
            holder.itemLayout.setBackgroundResource(R.drawable.solid_ovel);
            holder.itemDay.setTextColor(Color.WHITE);
        } else {
            holder.itemLayout.setBackgroundColor(Color.TRANSPARENT);
            holder.itemDay.setTextColor(R.color.ltGray);
        }

        // 判断是否是当前月，不是当前月不显示
        if (iMonth == iMonthViewCurrentMonth) {
            holder.itemLayout.setVisibility(View.VISIBLE);
        } else {
            holder.itemLayout.setVisibility(View.GONE);
        }

        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(mGridWidth, mGridWidth);
        holder.itemLayout.setLayoutParams(params);

        /**
         * convertView设置了setLayoutParams后，如果在此函数中设置某个控件的click事件，会出现第一格点击没有响应的情况，
         * 解决方法是设置GridView的onItemClick事件
         */
        convertView.setLayoutParams(new GridView.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, mGridWidth));

        return convertView;
    }

    private Boolean equalsDate(Date date1, Date date2) {
        Calendar cal1 = Calendar.getInstance();
        cal1.setTimeInMillis(date1.getTime());
        Calendar cal2 = Calendar.getInstance();
        cal2.setTimeInMillis(date2.getTime());

        int day1 = cal1.get(Calendar.DAY_OF_MONTH);
        int day2 = cal2.get(Calendar.DAY_OF_MONTH);
        if (day2 == day1) {
            return true;
        }
        return false;
    }

    public void setSelectPos(int pos) {
        selectPos = pos;
        Date date = (Date) getItem(pos);
        selectDay = date.getDate();
    }
}
