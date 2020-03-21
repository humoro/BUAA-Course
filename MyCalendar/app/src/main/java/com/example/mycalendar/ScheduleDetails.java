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

public class ScheduleDetails extends AppCompatActivity implements View.OnClickListener{

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
            finishThis();
        }
    };

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
                    DataBaseUtils.deleteScheduleById(schedule.getId());
                    Message message = new Message();
                    message.what = 1;
                    handler.sendMessage(message);
                }).start();
                break;
        }
    }

    private void finishThis() {
        Toast.makeText(this, "删除成功", Toast.LENGTH_LONG).show();
        this.finish();
    }
}
