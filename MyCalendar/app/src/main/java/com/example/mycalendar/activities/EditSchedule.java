package com.example.mycalendar.activities;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.app.DatePickerDialog;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import com.example.mycalendar.utils.JTimeUtils;
import com.example.mycalendar.R;
import com.example.mycalendar.Schedule;
import com.example.mycalendar.utils.StringUtils;
import com.example.mycalendar.utils.ActivityStackUtils;

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

public class EditSchedule extends AppCompatActivity implements View.OnClickListener {
    private final static int MSG_NETOUTOFWORK = 0x00001;
    private final static int MSG_DELETESCHEDULEFAULT = 0x00010;
    private final static int MSG_DELETESCHEDULESUCC = 0x00100;
    private final static int MSG_EDITSCHEDULEFAULT = 0x01000;
    private final static int MSG_EDITSCHEDULESUCC = 0x10000;

    private Schedule schedule;
    private EditText theme;
    private EditText content;
    private TextView date;
    private ImageButton deleteButton;
    private ImageButton timePicker;
    private ImageButton completeButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Bundle receive = getIntent().getExtras();
        if (receive != null) schedule = (Schedule) receive.getSerializable(StringUtils.BundleScheduleKey); // 获取接受的Schedule对象
        setContentView(R.layout.activity_edit_schedule);
        ActivityStackUtils.addActivity(this);
        initView();
    }

    private void initView() {
        theme = findViewById(R.id.edit_schedule_schedule_title);
        content = findViewById(R.id.edit_schedule_schedule_content);
        date = findViewById(R.id.edit_schedule_time_text);
        deleteButton = findViewById(R.id.edit_schedule_delete_button);
        timePicker = findViewById(R.id.edit_schedule_select_time);
        completeButton = findViewById(R.id.edit_schedule_complete_button);

        theme.setText(schedule.getTheme());
        content.setText(schedule.getContent());
        date.setText(schedule.getDate());
        deleteButton.setImageResource(R.mipmap.ic_delete_icon);
        timePicker.setImageResource(R.mipmap.ic_time_icon);
        completeButton.setImageResource(R.mipmap.ic_complete_icon);

        deleteButton.setOnClickListener(this);
        timePicker.setOnClickListener(this);
        completeButton.setOnClickListener(this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityStackUtils.removeActivity(this);
    }

    private void ToastNetWorkOutOfTIme() {
        Toast.makeText(this, "网络连接超时", Toast.LENGTH_LONG).show();
    }

    private void ToastCreateScheduleFault() {
        Toast.makeText(this, "删除日程失败", Toast.LENGTH_LONG).show();
    }

    private void ToastEditScheduleFault() {
        Toast.makeText(this, "编辑日程失败", Toast.LENGTH_LONG).show();
    }


    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_NETOUTOFWORK:
                {
                    ToastNetWorkOutOfTIme();
                }
                break;
                case MSG_DELETESCHEDULEFAULT:
                {
                    ToastCreateScheduleFault();
                }
                break;
                case MSG_EDITSCHEDULEFAULT:
                {
                    ToastEditScheduleFault();
                }
                break;
                case MSG_DELETESCHEDULESUCC:
                case MSG_EDITSCHEDULESUCC:
                {
                    finishThis();
                }
                break;
            }
        }
    };

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.edit_schedule_delete_button: // 删除该日程
            {
                new Thread(() -> {
                    String id = schedule.getId();
                    try{
                        HttpClient httpclient= new DefaultHttpClient();
                        HttpPost httpPost = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/ScheduleDelete");//服务器地址，指向查询用户是否信息的servlet
                        ArrayList<NameValuePair> params= new ArrayList<>();//将id装入list
                        params.add(new BasicNameValuePair(StringUtils.HttpScheduleIdKey, id));
                        final UrlEncodedFormEntity entity = new UrlEncodedFormEntity(params, "utf-8");
                        httpPost.setEntity(entity);
                        HttpResponse response = httpclient.execute(httpPost);
                        Message msg = new Message();
                        if (response.getStatusLine().getStatusCode() == 200) {
                            int ans = Integer.parseInt(EntityUtils.toString(response.getEntity()));
                            if (ans > 0) {
                                msg.what = MSG_DELETESCHEDULESUCC;
                            } else {
                                msg.what = MSG_DELETESCHEDULEFAULT;
                            }
                        } else {
                            msg.what = MSG_NETOUTOFWORK;
                        }
                        handler.sendMessage(msg);
                    }catch (Exception e) {
                        e.printStackTrace();
                    }
                }).start();
            }
                break;
            case R.id.edit_schedule_select_time: // 选择时间
            {
                Calendar calendar = Calendar.getInstance();
                int year = calendar.get(Calendar.YEAR);
                int month = calendar.get(Calendar.MONTH);
                int date = calendar.get(Calendar.DAY_OF_MONTH);
                DatePickerDialog dialog = new DatePickerDialog(this, setDateCallBack, year, month, date);
                dialog.show();
            }
                break;
            case R.id.edit_schedule_complete_button: // 完成编辑
            {
                schedule.setTheme(theme.getText().toString());
                schedule.setContent(content.getText().toString());
                String id = schedule.getId();
                String theme = schedule.getTheme();
                String content = schedule.getContent();
                String date = schedule.getDate();
                new Thread(() -> {
                    try{
                        HttpClient httpclient= new DefaultHttpClient();
                        HttpPost httpPost = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/ScheduleUpdate");//服务器地址，指向查询用户是否信息的servlet
                        ArrayList<NameValuePair> params= new ArrayList<>();//将id装入list
                        params.add(new BasicNameValuePair(StringUtils.HttpScheduleIdKey, id));
                        params.add(new BasicNameValuePair(StringUtils.HttpScheduleThemeKey, theme));
                        params.add(new BasicNameValuePair(StringUtils.HttpScheduleContentKey, content));
                        params.add(new BasicNameValuePair(StringUtils.HttpScheduleDateKey, date));
                        final UrlEncodedFormEntity entity = new UrlEncodedFormEntity(params, "utf-8");
                        httpPost.setEntity(entity);
                        HttpResponse response = httpclient.execute(httpPost);
                        Message msg = new Message();
                        if (response.getStatusLine().getStatusCode() == 200) {
                            int ans = Integer.parseInt(EntityUtils.toString(response.getEntity()));
                            if (ans > 0) {
                                msg.what = MSG_EDITSCHEDULESUCC;
                            } else {
                                msg.what = MSG_EDITSCHEDULEFAULT;
                            }
                        } else {
                            msg.what = MSG_NETOUTOFWORK;
                        }
                        handler.sendMessage(msg);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
//                    DataBaseUtils.updateScheduleById(schedule);
//                    Message message = new Message();
//                    message.what = 0x10;
//                    handler.sendMessage(message);
                }).start();
            }
                break;
        }
    }

    private DatePickerDialog.OnDateSetListener setDateCallBack = (view, year, monthOfYear, dayOfMonth) -> {
        Calendar calendar = Calendar.getInstance();
        calendar.set(year, monthOfYear, dayOfMonth);
        schedule.setDate(JTimeUtils.getDateString(calendar));
        date.setText(schedule.getDate());
    };

    private void finishThis() {
        this.finish();
    }
}
