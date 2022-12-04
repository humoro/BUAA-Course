package com.example.mycalendar.adapters;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.LinearLayout;

import androidx.annotation.NonNull;

import com.example.mycalendar.R;
import com.example.mycalendar.Schedule;
import com.example.mycalendar.UserAccount;
import com.example.mycalendar.utils.JTimeUtils;
import com.example.mycalendar.utils.SortUtils;
import com.example.mycalendar.utils.StringUtils;
import com.example.mycalendar.viewholders.DayViewHolder;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.util.EntityUtils;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

import static android.content.ContentValues.TAG;


public class MainPageCalendarGridViewAdapter extends BaseAdapter {
    private final static int HAVESCHEDULE = 0b0;
    private final static int NOSCHEDULE = 0b1;
    private final static String posKey = "pos";

    private Activity activity;
    private Calendar calendar = Calendar.getInstance();
    private Calendar calStartDate = Calendar.getInstance();
    private List<Date> showDates = new ArrayList<Date>();
    private UserAccount user;
    private int iMonthViewCurrentMonth = 0;
    private int mGridWidth = 0;
    public int selectPos = -1;
    public int selectDay = -1;
    public int todayPos = -1;
    // construct
    public MainPageCalendarGridViewAdapter(Activity a, Calendar showCalendar, UserAccount user) {
        activity = a;
        calStartDate = showCalendar;
        calendar.setTime(showCalendar.getTime());
        getDates();
        getGridWidth();
        this.user = user;
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
            holder.itemDay = convertView.findViewById(R.id.stc_main_page_day_item);
            holder.itemLayout =  convertView.findViewById(R.id.stc_cal_grid_item_layout);
            holder.haveSchedule = convertView.findViewById(R.id.stc_main_page_have_schedule);
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
            holder.itemLayout.setBackgroundResource(R.drawable.main_page_day_solid_ovel);
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
        new Thread(() -> {
            int have = 0;
            try {
                HttpClient httpclient= new DefaultHttpClient();
                HttpPost httpPost = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/ExistsScheduleInDate");//服务器地址，指向获取salt信息的servlet
                ArrayList<NameValuePair> param= new ArrayList<>();//将id装入list
                param.add(new BasicNameValuePair(StringUtils.HttpUserNameKey,user.getUserName()));
                param.add(new BasicNameValuePair(StringUtils.HttpScheduleDateKey, JTimeUtils.getDateString(calCalendar)));
                final UrlEncodedFormEntity saltEntity;
                saltEntity = new UrlEncodedFormEntity(param, "utf-8");
                httpPost.setEntity(saltEntity);
                HttpResponse response = httpclient.execute(httpPost);
                if (response.getStatusLine().getStatusCode() == 200) {
                    String ans = EntityUtils.toString(response.getEntity());
                    if (ans.equals("true")) have = 1;
                }
            }  catch (Exception e) {
                e.printStackTrace();
            }
            if (have > 0)
                holder.haveSchedule.setBackgroundResource(R.drawable.main_page_have_schedule_solid_ovel);
        }).start();
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
