package com.example.mycalendar.activities;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.DatePickerDialog;
import android.app.TimePickerDialog;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import com.example.mycalendar.utils.ActivityStackUtils;
import com.example.mycalendar.utils.JTimeUtils;
import com.example.mycalendar.R;
import com.example.mycalendar.Schedule;
import com.example.mycalendar.utils.StringUtils;
import com.example.mycalendar.UserAccount;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.util.EntityUtils;

import java.util.ArrayList;
import java.util.Calendar;

public class CreateSchedule extends AppCompatActivity implements View.OnClickListener {

    private final static int MSG_NETOUTOFWORK = 0x001;
    private final static int MSG_CREATESCHEDULEFAULT = 0x010;
    private final static int MSG_CREATESCHEDULESUCC = 0x100;

    private ImageButton completeButton;
    private ImageButton dateSelectorButton;
    private ImageButton timeSelectButton;
    private UserAccount user;
    private EditText themeText;
    private EditText contentText;
    private TextView dateText;
    private TextView timeText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Bundle receive = getIntent().getExtras();
        if (receive != null) user = (UserAccount) receive.getSerializable(StringUtils.BundleAccountKey); // 获取对应的账户
        setContentView(R.layout.activity_create_schedule);
        ActivityStackUtils.addActivity(this);
        initView();
    }

    private void initView() {
        // 根据id找到每个View
        completeButton = findViewById(R.id.create_schedule_complete_button);
        dateSelectorButton = findViewById(R.id.create_schedule_select_date);
        timeSelectButton = findViewById(R.id.create_schedule_select_time);
        dateText = findViewById(R.id.create_schedule_date_text);
        timeText = findViewById(R.id.create_schedule_time_text);
        themeText = findViewById(R.id.create_schedule_new_schedule_theme);
        contentText = findViewById(R.id.create_schedule_new_schedule_content);
        //初始化每个控件的样式
        completeButton.setImageResource(R.mipmap.ic_complete_icon);
        dateSelectorButton.setImageResource(R.mipmap.ic_create_date_icon);
        timeSelectButton.setImageResource(R.mipmap.ic_create_time_icon);
        completeButton.setBackgroundColor(Color.TRANSPARENT);
        dateSelectorButton.setBackgroundColor(Color.TRANSPARENT);
        refreshDateText(Calendar.getInstance());
        completeButton.setOnClickListener(this);
        dateSelectorButton.setOnClickListener(this);
        timeSelectButton.setOnClickListener(this);
    }

    private void refreshDateText(Calendar calendar) { // 刷新时间显示字符串
        dateText.setText(JTimeUtils.getDateString(calendar));
    }

    @SuppressLint("SetTextI18n")
    private void refreshTimeText(int hourOfDay, int minute) {
        timeText.setText(JTimeUtils.getTimeString(hourOfDay, minute));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityStackUtils.removeActivity(this);
    }

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case MSG_NETOUTOFWORK:
                {
                    ToastNetWorkOutOfTIme();
                }
                break;
                case MSG_CREATESCHEDULEFAULT:
                {
                    ToastCreateScheduleFault();
                }
                break;
                case MSG_CREATESCHEDULESUCC:
                {
                    finishThis();
                }
                break;
            }
        }
    };

    private void ToastNetWorkOutOfTIme() {
        Toast.makeText(this, "网络连接超时", Toast.LENGTH_LONG).show();
    }

    private void ToastCreateScheduleFault() {
        Toast.makeText(this, "创建日程失败", Toast.LENGTH_LONG).show();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.create_schedule_select_date: // 日期选择按钮
            {
                Calendar calendar = Calendar.getInstance();
                int year = calendar.get(Calendar.YEAR);
                int month = calendar.get(Calendar.MONTH);
                int date = calendar.get(Calendar.DAY_OF_MONTH);
                DatePickerDialog dialog = new DatePickerDialog(this, dateSelectListener, year, month, date);
                dialog.show();
            }
                break;
            case R.id.create_schedule_select_time:
            {
                Calendar calendar = Calendar.getInstance();
                TimePickerDialog dialog = new TimePickerDialog(this, AlertDialog.THEME_HOLO_LIGHT,timeSelectListener,calendar.get(Calendar.HOUR_OF_DAY),calendar.get(Calendar.MINUTE),true);
                dialog.show();
            }
                break;
            case R.id.create_schedule_complete_button: //创建完成通过获得的内容创建一个日程对象加入数据库并且提醒主数据库更新页面
            {
                String theme = themeText.getText().toString();
                String content = contentText.getText().toString();
                String time = dateText.getText().toString();
                Schedule schedule = new Schedule(user.getUserName(), theme, content, time);
                String username = user.getUserName();
                String id = schedule.getId();
                new Thread(() -> {
                    try{
                        HttpClient httpclient= new DefaultHttpClient();
                        HttpPost httpPost = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/ScheduleCreate");//服务器地址，指向查询用户是否信息的servlet
                        ArrayList<NameValuePair> params= new ArrayList<>();//将id装入list
                        params.add(new BasicNameValuePair(StringUtils.HttpScheduleIdKey, id));
                        params.add(new BasicNameValuePair(StringUtils.HttpUserNameKey, username));
                        params.add(new BasicNameValuePair(StringUtils.HttpScheduleThemeKey, theme));
                        params.add(new BasicNameValuePair(StringUtils.HttpScheduleContentKey, content));
                        params.add(new BasicNameValuePair(StringUtils.HttpScheduleDateKey, time));
                        final UrlEncodedFormEntity entity = new UrlEncodedFormEntity(params, "utf-8");
                        httpPost.setEntity(entity);
                        HttpResponse response = httpclient.execute(httpPost);
                        Message msg = new Message();
                        if (response.getStatusLine().getStatusCode() == 200) {
                            int ans = Integer.parseInt(EntityUtils.toString(response.getEntity()));
                            if (ans > 0) {
                                msg.what = MSG_CREATESCHEDULESUCC;
                            } else {
                                msg.what = MSG_CREATESCHEDULEFAULT;
                            }
                        } else {
                            msg.what = MSG_NETOUTOFWORK;
                        }
                        handler.sendMessage(msg);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
//                    DataBaseUtils.insertSchedule(schedule);
//                    Message msg = new Message();
//                    msg.what = 0x0;
//                    handler.sendMessage(msg);
                }).start();
            }
                break;
        }
    }

    private void finishThis() {
        this.finish();
    }

    @SuppressLint("ShowToast")
    private TimePickerDialog.OnTimeSetListener timeSelectListener = (view, hourOfDay, minute) -> {
        refreshTimeText(hourOfDay, minute);
    };

    private DatePickerDialog.OnDateSetListener dateSelectListener = (view, year, monthOfYear, dayOfMonth) -> {
        Calendar calendar = Calendar.getInstance();
        calendar.set(year, monthOfYear, dayOfMonth);
        refreshDateText(calendar);
    };
}
