package com.example.mycalendar;

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

import java.util.Calendar;

public class EditSchedule extends AppCompatActivity implements View.OnClickListener {
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
            case R.id.edit_schedule_delete_button: // 删除该日程
            {
                new Thread(() -> {
                    DataBaseUtils.deleteScheduleById(schedule.getId());
                    Message message = new Message();
                    message.what = 0x01;
                    handler.sendMessage(message);
                }).start();
            }
                break;
            case R.id.edit_schedule_select_time:
            {
                Calendar calendar = Calendar.getInstance();
                int year = calendar.get(Calendar.YEAR);
                int month = calendar.get(Calendar.MONTH);
                int date = calendar.get(Calendar.DAY_OF_MONTH);
                DatePickerDialog dialog = new DatePickerDialog(this, setDateCallBack, year, month, date);
                dialog.show();
            }
                break;
            case R.id.edit_schedule_complete_button:
            {
                schedule.setTheme(theme.getText().toString());
                schedule.setContent(content.getText().toString());
                new Thread(() -> {
                    DataBaseUtils.updateScheduleById(schedule);
                    Message message = new Message();
                    message.what = 0x10;
                    handler.sendMessage(message);
                }).start();
            }
                break;
        }
    }

    private DatePickerDialog.OnDateSetListener setDateCallBack = (view, year, monthOfYear, dayOfMonth) -> {
        Calendar calendar = Calendar.getInstance();
        calendar.set(year, monthOfYear, dayOfMonth);
        schedule.setDate(JTimeUtils.getDateString(calendar));
    };

    private void finishThis() {
        this.finish();
    }
}
