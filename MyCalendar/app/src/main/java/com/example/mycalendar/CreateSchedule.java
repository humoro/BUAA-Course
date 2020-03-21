package com.example.mycalendar;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.app.DatePickerDialog;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.ContactsContract;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;

import java.util.Calendar;

public class CreateSchedule extends AppCompatActivity implements View.OnClickListener {

    private ImageButton completeButton;
    private ImageButton timeSelectorButton;
    private UserAccount user;
    private EditText themeText;
    private EditText contentText;
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
        timeSelectorButton = findViewById(R.id.create_schedule_select_time);
        timeText = findViewById(R.id.create_schedule_time_text);
        themeText = findViewById(R.id.create_schedule_new_schedule_theme);
        contentText = findViewById(R.id.create_schedule_new_schedule_content);
        //初始化每个控件的样式
        completeButton.setImageResource(R.mipmap.ic_complete_icon);
        timeSelectorButton.setImageResource(R.mipmap.ic_time_icon);
        completeButton.setBackgroundColor(Color.TRANSPARENT);
        timeSelectorButton.setBackgroundColor(Color.TRANSPARENT);
        refreshTimeText(Calendar.getInstance());
        completeButton.setOnClickListener(this);
        timeSelectorButton.setOnClickListener(this);
    }

    private void refreshTimeText(Calendar calendar) { // 刷新时间显示字符串
        timeText.setText(JTimeUtils.getDateString(calendar));
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
            if (msg.what == 0x0) {
                finishThis();
            }
        }
    };

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.create_schedule_select_time:
            {
                Calendar calendar = Calendar.getInstance();
                int year = calendar.get(Calendar.YEAR);
                int month = calendar.get(Calendar.MONTH);
                int date = calendar.get(Calendar.DAY_OF_MONTH);
                DatePickerDialog dialog = new DatePickerDialog(this, setDateCallBack, year, month, date);
                dialog.show();
            }
                break;
            case R.id.create_schedule_complete_button: //创建完成通过获得的内容创建一个日程对象加入数据库并且提醒主数据库更新页面
            {
                String theme = themeText.getText().toString();
                String content = contentText.getText().toString();
                String time = timeText.getText().toString();
                Schedule schedule = new Schedule(user.getUserName(), theme, content, time);
                new Thread(() -> {
                    DataBaseUtils.insertSchedule(schedule);
                    Message msg = new Message();
                    msg.what = 0x0;
                    handler.sendMessage(msg);
                }).start();
            }
                break;
        }
    }

    private void finishThis() {
        this.finish();
    }

    private DatePickerDialog.OnDateSetListener setDateCallBack = (view, year, monthOfYear, dayOfMonth) -> {
        Calendar calendar = Calendar.getInstance();
        calendar.set(year, monthOfYear, dayOfMonth);
        refreshTimeText(calendar);
    };
}
