package com.example.mycalendar.activities;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.example.mycalendar.adapters.AccountDetailsListViewAdapter;
import com.example.mycalendar.utils.ActivityStackUtils;
import com.example.mycalendar.utils.JTimeUtils;
import com.example.mycalendar.R;
import com.example.mycalendar.Schedule;
import com.example.mycalendar.utils.SortUtils;
import com.example.mycalendar.utils.StringUtils;
import com.example.mycalendar.UserAccount;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;

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
import java.util.List;

public class AccountDetails extends AppCompatActivity {
    private final static int MSG_NETWORKOUTOFTIME = 0x001;
    private final static int MSG_GOTSCHEDULELISTSUCC = 0x010;
    private final static int MSG_GOTTOTALNUMBER = 0x100;

    private UserAccount user;
    private TextView userName;
    private TextView totalNumber;
    private ListView listView;
    private List<Schedule> list = new ArrayList<>();
    private AccountDetailsListViewAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_account_details);
        Bundle receive = getIntent().getExtras();
        if (receive != null) user = (UserAccount) receive.getSerializable(StringUtils.BundleAccountKey);
        assert user != null; // 传过来用户信息
        ActivityStackUtils.addActivity(this);
        initView();
    }

    private void initView() {
        userName = findViewById(R.id.account_details_username);
        totalNumber = findViewById(R.id.account_details_all_schedule_number);
        listView = findViewById(R.id.account_details_list_view);
        listView.setDivider(null);
        //
        userName.setText(user.getUserName());
        gotTotalNumberHttp();
        getAllSchedule();
    }

    private void gotTotalNumberHttp() {
        new Thread(() -> {
            try{
                HttpClient httpclient= new DefaultHttpClient();
                HttpPost httpPost = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/GotScheduleNumber");//服务器地址，指向获取salt信息的servlet
                ArrayList<NameValuePair> params= new ArrayList<>();//将id装入list
                params.add(new BasicNameValuePair(StringUtils.HttpUserNameKey,user.getUserName()));
                final UrlEncodedFormEntity saltEntity = new UrlEncodedFormEntity(params, "utf-8");
                httpPost.setEntity(saltEntity);
                HttpResponse response = httpclient.execute(httpPost);
                Message msg = new Message();
                if (response.getStatusLine().getStatusCode() == 200) {
                    String ans = EntityUtils.toString(response.getEntity());
                    Bundle bundle = new Bundle();
                    bundle.putString(StringUtils.BundleStringKey, ans);
                    msg.what = MSG_GOTTOTALNUMBER;
                    msg.setData(bundle);
                } else {
                    msg.what = MSG_NETWORKOUTOFTIME; // 网络超时
                }
                handler.sendMessage(msg); //使用Message传递消息给线程
            }  catch (Exception e) {
                e.printStackTrace();
            }
        }).start();
    }

    private void setTotalNumber(String number) {
        totalNumber.setText(number);
    }

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case MSG_GOTSCHEDULELISTSUCC:
                {
                    Bundle bundle = msg.getData();
                    ArrayList<Schedule> list = bundle.getParcelableArrayList(StringUtils.BundleListKey);
                    initListView(list);
                }
                break;
                case MSG_NETWORKOUTOFTIME:
                {
                    ToastNetWorkOutOfTIme();
                }
                break;
                case MSG_GOTTOTALNUMBER:
                {
                    String ans = msg.getData().getString(StringUtils.BundleStringKey);
                    setTotalNumber(ans);
                }
                break;
            }
        }
    };

    private void setItemClick(List<Schedule> list) {
        listView.setOnItemClickListener((parent, view, position, id) -> {
            Schedule schedule = list.get(position); // 获取被点击的日程
            Bundle bundle = new Bundle();
            bundle.putSerializable(StringUtils.BundleScheduleKey, schedule); // 传递schedule
            Intent intent = new Intent(this, ScheduleDetails.class);
            intent.putExtras(bundle);
            startActivity(intent);
        });
    }

    private void initListView(List<Schedule> allSchedule) {
        this.list.clear();
        this.list = allSchedule;
        ActivityStackUtils.addActivity(this);
        adapter = new AccountDetailsListViewAdapter(allSchedule, this);
        listView.setAdapter(adapter);
        setItemClick(list);
    }

    private void ToastNetWorkOutOfTIme() {
        Toast.makeText(this, "网络连接超时", Toast.LENGTH_LONG).show();
    }

    @Override
    public void onResume() {
        super.onResume();
        getAllSchedule();
        gotTotalNumberHttp();
    }


    private void getAllSchedule() {
        new Thread(() -> {
            try {
                HttpClient httpclient= new DefaultHttpClient();
                HttpPost httpPost = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/GotTimeQuantumSchedule");//服务器地址，指向获取salt信息的servlet
                ArrayList<NameValuePair> params= new ArrayList<>();//将id装入list
                params.add(new BasicNameValuePair(StringUtils.HttpUserNameKey,user.getUserName()));
                Calendar calendar = Calendar.getInstance();
                String now = JTimeUtils.getDateString(calendar);
                calendar.add(Calendar.DAY_OF_MONTH, 5);
                String fiveday = JTimeUtils.getDateString(calendar);
                params.add(new BasicNameValuePair(StringUtils.HttpNowday, now));
                params.add(new BasicNameValuePair(StringUtils.HttpNextFiveDay, fiveday));
                final UrlEncodedFormEntity saltEntity = new UrlEncodedFormEntity(params, "utf-8");
                httpPost.setEntity(saltEntity);
                HttpResponse response = httpclient.execute(httpPost);
                Message msg = new Message();
                if (response.getStatusLine().getStatusCode() == 200) {
                    String jsonStr = EntityUtils.toString(response.getEntity());
                    Gson gson = new Gson();
                    ArrayList<Schedule> schedules = gson.fromJson(jsonStr, new TypeToken<ArrayList<Schedule>>(){}.getType());
                    SortUtils.sortSchedules(schedules); // 按照时间排序
                    Bundle bundle = new Bundle();
                    bundle.putParcelableArrayList(StringUtils.BundleListKey, schedules);
                    msg.what = MSG_GOTSCHEDULELISTSUCC;
                    msg.setData(bundle);
                } else {
                    msg.what = MSG_NETWORKOUTOFTIME; // 网络超时
                }
                handler.sendMessage(msg); //使用Message传递消息给线程
            } catch (Exception e) {
                e.printStackTrace();
            }
        }).start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityStackUtils.removeActivity(this);
    }
}
