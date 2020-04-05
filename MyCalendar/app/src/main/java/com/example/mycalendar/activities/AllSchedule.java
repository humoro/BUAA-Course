package com.example.mycalendar.activities;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.ListView;
import android.widget.SearchView;
import android.widget.Toast;

import com.example.mycalendar.adapters.FilterListener;
import com.example.mycalendar.R;
import com.example.mycalendar.Schedule;
import com.example.mycalendar.adapters.SearchListViewAdapter;
import com.example.mycalendar.utils.SortUtils;
import com.example.mycalendar.utils.StringUtils;
import com.example.mycalendar.UserAccount;
import com.example.mycalendar.utils.ActivityStackUtils;
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
import java.util.List;

public class AllSchedule extends AppCompatActivity {
    private final static int MSG_NETWORKOUTOFTIME = 0x001;
    private final static int MSG_GOTSCHEDULELISTSUCC = 0x010;

    private SearchView mSearchView;
    private ListView mListView;
    private List<Schedule> list = new ArrayList<>();
    private SearchListViewAdapter mAdapter;
    private UserAccount user;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Bundle receive = getIntent().getExtras();
        if (receive != null) user = (UserAccount) receive.getSerializable(StringUtils.BundleAccountKey); // 获取账户信息
        initView();
        getAllSchedule();
    }

    private void initView() {
        setContentView(R.layout.activity_all_schedule);
        mSearchView = findViewById(R.id.stc_search_edit_text);
        mListView = findViewById(R.id.stc_all_schedule_list);
        mListView.setDivider(null);
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
            }
        }
    };

    private void ToastNetWorkOutOfTIme() {
        Toast.makeText(this, "网络连接超时", Toast.LENGTH_LONG).show();
    }

    private void getAllSchedule() {
        new Thread(() -> {
            try {
                HttpClient httpclient= new DefaultHttpClient();
                HttpPost httpPost = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/ScheduleGetByUserName");//服务器地址，指向获取salt信息的servlet
                ArrayList<NameValuePair> params= new ArrayList<>();//将id装入list
                params.add(new BasicNameValuePair(StringUtils.HttpUserNameKey,user.getUserName()));
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
            }
            catch (Exception e) {
                e.printStackTrace();
            }
//            ArrayList<Schedule> list = (ArrayList<Schedule>) DataBaseUtils.searchScheduleByUserName(this.user.getUserName()); // 获取该用户的所有日程
//            Bundle bundle = new Bundle();
//            bundle.putParcelableArrayList(StringUtils.BundleListKey, list);
//            Message msg = new Message();
//            msg.setData(bundle);
//            handler.sendMessage(msg);
        }).start();
    }

    private void initListView(List<Schedule> allSchedule) {
        this.list.clear();
        this.list = allSchedule;
        ActivityStackUtils.addActivity(this);
        mAdapter = new SearchListViewAdapter(allSchedule, this, new FilterListener() {
            @Override
            public void getFilterData(List<Schedule> list) {
                setItemClick(list);
            }
        });
        mListView.setAdapter(mAdapter);
        setListeners();
    }

    private void setItemClick(List<Schedule> list) {
        mListView.setOnItemClickListener((parent, view, position, id) -> {
            Schedule schedule = list.get(position); // 获取被点击的日程
            Bundle bundle = new Bundle();
            bundle.putSerializable(StringUtils.BundleScheduleKey, schedule); // 传递schedule
            Intent intent = new Intent(this, ScheduleDetails.class);
            intent.putExtras(bundle);
            startActivity(intent);
        });
    }

    @Override
    public void onResume() {
        super.onResume();
        getAllSchedule();
    }

    private void setListeners() {
        setItemClick(list);
        mSearchView.setOnQueryTextListener(new SearchView.OnQueryTextListener() {
            @Override
            public boolean onQueryTextSubmit(String query) {
                return true;
            }

            @Override
            public boolean onQueryTextChange(String newText) {
                if(mAdapter != null){
                    mAdapter.getFilter().filter(newText);
                }
                return true;
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityStackUtils.removeActivity(this);
    }
}
