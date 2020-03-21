package com.example.mycalendar;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Filter;
import android.widget.ListView;
import android.widget.SearchView;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class AllSchedule extends AppCompatActivity {
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
            Bundle bundle = msg.getData();
            ArrayList<Schedule> list = bundle.getParcelableArrayList(StringUtils.BundleListKey);
            initListView(list);
        }
    };

    private void getAllSchedule() {
        new Thread(() -> {
            ArrayList<Schedule> list = (ArrayList<Schedule>) DataBaseUtils.searchScheduleByUserName(this.user.getUserName()); // 获取该用户的所有日程
            Bundle bundle = new Bundle();
            bundle.putParcelableArrayList(StringUtils.BundleListKey, list);
            Message msg = new Message();
            msg.setData(bundle);
            handler.sendMessage(msg);
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
