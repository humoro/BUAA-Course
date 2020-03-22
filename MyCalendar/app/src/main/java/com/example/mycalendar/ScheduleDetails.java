package com.example.mycalendar;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.util.EntityUtils;

import java.util.ArrayList;

public class ScheduleDetails extends AppCompatActivity implements View.OnClickListener{
    private final static int MSG_NETOUTOFWORK = 0x001;
    private final static int MSG_DELETESCHEDULEFAULT = 0x010;
    private final static int MSG_DELETESCHEDULESUCC = 0x100;

    private TextView detailTheme;
    private TextView detailContent;
    private TextView detailDate;

    private ImageButton editButton;
    private ImageButton deleteButton;
    private Schedule schedule;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Bundle receive = getIntent().getExtras();
        if (receive != null) schedule = (Schedule) receive.getSerializable(StringUtils.BundleScheduleKey); // 获取接受的Schedule对象
        setContentView(R.layout.activity_schedule_details);
        ActivityStackUtils.addActivity(this);
        initView();
    }

    @SuppressLint("SetTextI18n")
    private void initView() {
        detailTheme = findViewById(R.id.schedule_details_theme);
        detailContent = findViewById(R.id.schedule_details_content);
        detailDate = findViewById(R.id.schedule_details_date);
        editButton = findViewById(R.id.schedule_details_edit_button);
        deleteButton = findViewById(R.id.schedule_details_delete_button);
        editButton.setOnClickListener(this);
        deleteButton.setOnClickListener(this);

        Log.d("Schedules Details :", "initView  " + schedule.getContent());
        editButton.setImageResource(R.mipmap.ic_edit_icon);
        deleteButton.setImageResource(R.mipmap.ic_delete_icon);
        detailTheme.setText("主题: " + schedule.getTheme());
        detailContent.setText("内容: " + schedule.getContent());
        detailDate.setText("日期: " + schedule.getDate());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityStackUtils.removeActivity(this);
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
                case MSG_DELETESCHEDULESUCC:
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
        Toast.makeText(this, "删除日程失败", Toast.LENGTH_LONG).show();
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.schedule_details_edit_button: // 点击编辑按钮进入编辑页面
            {
                Bundle bundle = new Bundle();
                Intent intent = new Intent(this, EditSchedule.class);
                bundle.putSerializable(StringUtils.BundleScheduleKey, schedule);
                intent.putExtras(bundle);
                startActivity(intent);
                this.finish();
            }
                break;
            case R.id.schedule_details_delete_button:
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
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
//                    DataBaseUtils.deleteScheduleById(schedule.getId());
//                    Message message = new Message();
//                    message.what = 1;
//                    handler.sendMessage(message);
                }).start();
                break;
        }
    }

    private void finishThis() {
        Toast.makeText(this, "删除成功", Toast.LENGTH_LONG).show();
        this.finish();
    }
}
