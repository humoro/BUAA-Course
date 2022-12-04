package com.example.mycalendar.activities;

import androidx.annotation.NonNull;
import androidx.viewpager.widget.PagerAdapter;
import androidx.viewpager.widget.ViewPager;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.DatePickerDialog;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.example.mycalendar.adapters.MainPageCalendarGridViewAdapter;
import com.example.mycalendar.adapters.MainPageScheduleListViewAdapter;
import com.example.mycalendar.adapters.MainPagerTitleGridViewAdapter;
import com.example.mycalendar.R;
import com.example.mycalendar.Schedule;
import com.example.mycalendar.utils.SortUtils;
import com.example.mycalendar.utils.StringUtils;
import com.example.mycalendar.UserAccount;
import com.example.mycalendar.utils.ActivityStackUtils;
import com.example.mycalendar.utils.JTimeUtils;
import com.example.mycalendar.viewholders.DayViewHolder;
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
import java.util.Date;
import java.util.List;

import static android.content.ContentValues.TAG;

public class MainPage extends Activity  implements View.OnClickListener {
    private final static int MSG_NETWORKOUTOFTIME = 0x001;
    private final static int MSG_GOTSCHEDULELISTSUCC = 0x010;

    //文本view
    private TextView mYearTx;
    private TextView mMonthTx;
    private LinearLayout mCalendarLayout;

    private ImageView todayIcon;
    private ImageView plusIcon;
    private ImageView scheduleIcon;
    private ImageView accountIcon;

    private Calendar calendarShow = Calendar.getInstance(); // 显示的日期
    private Calendar calendarNow = Calendar.getInstance(); // 选择的基准日期

    private GridView titleView;

    private PagerAdapter pagerAdapter = null;
    private ViewPager viewPager = null;
    private GridView currentView = null;
    private int currPager = 500;
    private MainPageCalendarGridViewAdapter adapter = null;
    private ListView listView;
    private UserAccount user;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "onCreate: main page");
        super.onCreate(savedInstanceState);
        Bundle receive = getIntent().getExtras();
        if (receive != null) user = (UserAccount) receive.getSerializable(StringUtils.BundleAccountKey);
        assert user != null; // 传过来用户信息
        setContentView(R.layout.activity_main_page);
        ActivityStackUtils.finishAllDead();
        ActivityStackUtils.addActivity(this);
        initView();
    }

    @Override
    protected void onRestart() {
        Log.d(TAG, "onRestart: ");
        super.onRestart();
    }

    @Override
    protected void onStart() {
        Log.d(TAG, "onStart: ");
        super.onStart();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityStackUtils.removeActivity(this);
    }

    private void initView() {
        //根据id找到对应的view
        viewPager = findViewById(R.id.stc_calendar_viewpager);
        mYearTx = findViewById(R.id.stc_main_page_year_tx);
        mMonthTx = findViewById(R.id.stc_main_page_month_tx);
        mYearTx.getPaint().setFakeBoldText(true);
        mMonthTx.getPaint().setFakeBoldText(true);
        mMonthTx.setOnClickListener(this);
        mCalendarLayout = findViewById(R.id.stc_calendar_layout);
        todayIcon = findViewById(R.id.stc_main_page_today_icon);
        todayIcon.setImageResource(R.mipmap.ic_calendar_view);
        plusIcon = findViewById(R.id.stc_main_page_plus_icon);
        scheduleIcon = findViewById(R.id.stc_main_page_all_schedule_icon);
        accountIcon = findViewById(R.id.stc_main_page_account_icon);
        //初始化控件的样式
        mYearTx.setOnClickListener(this);
        mMonthTx.setOnClickListener(this);
        todayIcon.setOnClickListener(this);
        plusIcon.setOnClickListener(this);
        scheduleIcon.setOnClickListener(this);
        accountIcon.setOnClickListener(this);
        plusIcon.setImageResource(R.mipmap.ic_plus_sign);


        setTitleGirdView();
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        mCalendarLayout.addView(titleView, params);
        listView = findViewById(R.id.stc_main_page_list_view);
        listView.setDivider(null);
    }

    private void initTodayList(List<Schedule> list) {
        MainPageScheduleListViewAdapter adapter = new MainPageScheduleListViewAdapter(list, this);
        listView.setAdapter(adapter);
    }

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case MSG_GOTSCHEDULELISTSUCC:
                {
                    List<Schedule> list = msg.getData().getParcelableArrayList(StringUtils.BundleListKey);
                    initTodayList(list);
                    setListViewItemClick(list);
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

    private void setListViewItemClick(List<Schedule> list) {
        listView.setOnItemClickListener((parent, view, position, id) -> {
            Schedule schedule = list.get(position); // 获取被点击的日程
            Bundle bundle = new Bundle();
            bundle.putSerializable(StringUtils.BundleScheduleKey, schedule); // 传递schedule
            Intent intent = new Intent(this, ScheduleDetails.class);
            intent.putExtras(bundle);
            startActivity(intent);
        });
    }

    private void refreshList(UserAccount user) {
        new Thread(() -> {
            try {
                HttpClient httpclient= new DefaultHttpClient();
                HttpPost httpPost = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/ScheduleGetByUserNameAndDate");//服务器地址，指向获取salt信息的servlet
                ArrayList<NameValuePair> params= new ArrayList<>();//将id装入list
                params.add(new BasicNameValuePair(StringUtils.HttpUserNameKey,user.getUserName()));
                params.add(new BasicNameValuePair(StringUtils.HttpScheduleDateKey, JTimeUtils.getDateString(calendarShow)));
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
//            ArrayList<Schedule> list = (ArrayList<Schedule>) DataBaseUtils.searchScheduleByUserNameADate(user.getUserName(), JTimeUtils.getDateString(calendarShow));
//            Bundle bundle = new Bundle();
//            bundle.putParcelableArrayList(StringUtils.BundleListKey, list);
//            Message msg = new Message();
//            msg.setData(bundle);
//            handler.sendMessage(msg);
        }).start();
    }

    private void initData() {
        setDateUI(calendarShow);
        initCalendar();
    }

    private void setDateUI(Calendar cal) {
        mYearTx.setText(String.valueOf(cal.get(Calendar.YEAR)));
        mMonthTx.setText(JTimeUtils.getMonthName(cal.get(Calendar.MONTH) + 1));
        scheduleIcon.setImageResource(R.mipmap.ic_schedule_gray);
        accountIcon.setImageResource(R.mipmap.ic_account_icon);
    }

    @Override
    public void onResume() { // 页面重回焦点的时候调用
        super.onResume();
        calendarShow = Calendar.getInstance();
        Log.d(TAG, "onResume: flush the main page" );
        initData();
        refreshList(user);
    }

    private void setTitleGirdView() {
        titleView = generateTitleGirdView();
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        titleView.setLayoutParams(params);
        titleView.setVerticalSpacing(1);// 垂直间隔
        titleView.setHorizontalSpacing(1);// 水平间隔
        MainPagerTitleGridViewAdapter titleAdapter = new MainPagerTitleGridViewAdapter(this);
        titleView.setAdapter(titleAdapter);// 设置菜单Adapter
    }

    private GridView generateTitleGirdView() {
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        GridView gridView = new GridView(this);
        gridView.setLayoutParams(params);
        gridView.setNumColumns(7);// 设置每行列数
        gridView.setGravity(Gravity.CENTER_VERTICAL);// 位置居中
        gridView.setVerticalSpacing(1);// 垂直间隔
        gridView.setHorizontalSpacing(1);// 水平间隔
        gridView.setBackgroundColor(Color.TRANSPARENT);
        gridView.setSelector(new ColorDrawable(Color.TRANSPARENT));
        return gridView;
    }

    @Override
    public void onClick(View v) {
        Log.d(TAG, "onClick: " + v.getId());
        switch (v.getId()) {
            case R.id.stc_main_page_all_schedule_icon: // 跳转到所有日程的界面
            {
                Bundle bundle = new Bundle();
                bundle.putSerializable(StringUtils.BundleAccountKey, this.user);
                Intent intent = new Intent(this, AllSchedule.class);
                intent.putExtras(bundle); // 传递过去账户信息
                startActivity(intent);
            }
            break;
            case R.id.stc_main_page_today_icon: // 跳转到今天日期页面
            {
                Log.d(TAG, "onClick: click the icon of today" );
                Calendar calendarNow = Calendar.getInstance();
                calendarShow.set(calendarNow.get(Calendar.YEAR), calendarNow.get(Calendar.MONTH), calendarNow.get(Calendar.DAY_OF_MONTH));
                this.calendarNow.set(calendarNow.get(Calendar.YEAR), calendarNow.get(Calendar.MONTH), calendarNow.get(Calendar.DAY_OF_MONTH));
                initData();
                refreshList(user);
            }
            break;
            case R.id.stc_main_page_plus_icon:// 创建新日程
            {
                Bundle bundle = new Bundle();
                bundle.putSerializable(StringUtils.BundleAccountKey, this.user); // 传递账户信息
                Intent intent = new Intent(this, CreateSchedule.class);
                intent.putExtras(bundle);
                startActivity(intent);
            }
            break;
            case R.id.stc_main_page_month_tx:
            case R.id.stc_main_page_year_tx:
            {
                Calendar calendar = Calendar.getInstance();
                int year = calendar.get(Calendar.YEAR);
                int month = calendar.get(Calendar.MONTH);
                int date = calendar.get(Calendar.DAY_OF_MONTH);
                DatePickerDialog dialog = new DatePickerDialog(this, setDateCallBack, year, month, date);
                dialog.show();
            }
            break;
            case R.id.stc_main_page_account_icon:
            {
                Intent intent = new Intent(this, AccountDetails.class);
                Bundle bundle = new Bundle();
                bundle.putSerializable(StringUtils.BundleAccountKey, user);
                intent.putExtras(bundle);
                startActivity(intent);
            }
            break;
        }
    }

    private DatePickerDialog.OnDateSetListener setDateCallBack = (view, year, monthOfYear, dayOfMonth) -> {
        calendarShow.set(year, monthOfYear, dayOfMonth);
        calendarNow.set(year, monthOfYear, dayOfMonth);
        Log.d(TAG, "onDateSet: " + year + " : " + monthOfYear + " : " + dayOfMonth);
        initData();
    };

    private void initCalendar() {
        pagerAdapter = new MainPageViewPagerAdapter();
        viewPager.setAdapter(pagerAdapter);
        viewPager.setCurrentItem(500);// 设置当前页位置为500
        viewPager.setPageMargin(0);

        viewPager.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            public void onPageScrollStateChanged(int arg0) {
                if (arg0 == 1) {
                }
                if (arg0 == 0) {
                    currentView = viewPager.findViewById(currPager);
                    if (currentView != null) {
                        adapter = (MainPageCalendarGridViewAdapter) currentView.getAdapter();
                    }
                }
            }

            public void onPageScrolled(int arg0, float arg1, int arg2) {

            }

            public void onPageSelected(int position) {
                // 滑动到某页后更新数据、UI
                Calendar tempSelected = Calendar.getInstance(); // 临时
                tempSelected.setTime(calendarNow.getTime());
                int count = position - 500;
                tempSelected.add(Calendar.MONTH, count);
                Log.d(TAG, "onPageSelected: " + tempSelected.get(Calendar.MONTH) + " " + tempSelected.get(Calendar.DAY_OF_MONTH));
                setDateUI(tempSelected);
                currPager = position;
                calendarShow.set(tempSelected.get(Calendar.YEAR), tempSelected.get(Calendar.MONTH), tempSelected.get(Calendar.DAY_OF_MONTH));
                refreshList(user);
            }
        });
    }

    private class MainPageViewPagerAdapter extends PagerAdapter {

        @Override
        public void setPrimaryItem(ViewGroup container, int position,
                                   Object object) {
        }

        @Override
        public int getCount() {
            return 1000; // viewPager 中一共有1000个page
        }

        @Override
        public boolean isViewFromObject(View arg0, Object arg1) {
            return arg0 == arg1;
        }

        @Override
        public void destroyItem(ViewGroup container, int position, Object object) {
            container.removeView((View) object);
        }

        @NonNull
        @Override
        public Object instantiateItem(ViewGroup container, int position) {
            GridView gv = initCalendarView(position);
            gv.setId(position);
            container.addView(gv);
            return gv;
        }
    }

    private GridView setGirdView() {
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        GridView gridView = new GridView(this);
        gridView.setLayoutParams(params);
        gridView.setNumColumns(7);// 设置每行列数
        gridView.setGravity(Gravity.CENTER_VERTICAL);// 位置居中
        gridView.setVerticalSpacing(1);// 垂直间隔
        gridView.setHorizontalSpacing(1);// 水平间隔
        gridView.setBackgroundColor(Color.TRANSPARENT);
        gridView.setSelector(new ColorDrawable(Color.TRANSPARENT));
        return gridView;
    }

    public GridView initCalendarView(int position) {
        int count = position - 500;
        final Calendar tempSelected = Calendar.getInstance(); // 临时

		/*if (position == 500) {
			int month = tempSelected.get(Calendar.MONTH) + 1;
			mSelectedMonthTx.setText(TimeUtils.getShowTimeTx(this, month));
		}*/

        tempSelected.setTime(calendarNow.getTime());

        GridView gView = setGirdView();;
        tempSelected.add(Calendar.MONTH, count);
        final MainPageCalendarGridViewAdapter gAdapter = new MainPageCalendarGridViewAdapter(this, tempSelected, this.user);
        gView.setAdapter(gAdapter);// 设置菜单Adapter
        gView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @SuppressLint("ResourceAsColor")
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                // item点击事件
                // do somethings
                calendarShow.setTime((Date) parent.getAdapter().getItem(position));
                Log.d(TAG, "onItemClick: " + calendarShow.get(Calendar.YEAR));
                setDateUI(calendarShow);
                MainPageCalendarGridViewAdapter mAdapter = (MainPageCalendarGridViewAdapter) parent.getAdapter();
                //先把原先选定的view的样式还原
                View selectView = parent.getChildAt(mAdapter.selectPos);
                Log.d(TAG, "onItemClick2: " + mAdapter.selectPos);
                DayViewHolder selectHolder = (DayViewHolder) selectView.getTag();
                selectHolder.itemLayout.setBackgroundColor(Color.TRANSPARENT);
                selectHolder.itemDay.setTextColor(R.color.ltGray);
                //如果是当月，那么今天用空心圆标出来
                if (calendarNow.get(Calendar.YEAR) == calendarShow.get(Calendar.YEAR) &&
                        calendarNow.get(Calendar.MONTH) == calendarShow.get(Calendar.MONTH)) {
                    Log.d(TAG, "onItemClick3: " + calendarShow.get(Calendar.MONTH) + 1);
                    View todayView = parent.getChildAt(mAdapter.todayPos);
                    DayViewHolder todayHolder = (DayViewHolder) todayView.getTag();
                    todayHolder.itemLayout.setBackgroundResource(R.drawable.stroke_ovel);
                    todayHolder.itemDay.setTextColor(R.color.ltGray);
                }
                // 选中的那一天紫色背景
                mAdapter.setSelectPos(position);
                selectView = parent.getChildAt(mAdapter.selectPos);
                selectHolder = (DayViewHolder) selectView.getTag();
                selectHolder.itemLayout.setBackgroundResource(R.drawable.main_page_day_solid_ovel);
                selectHolder.itemDay.setTextColor(Color.WHITE);
                refreshList(user);
            }
        });
        return gView;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode==0x11) {
            if (resultCode==RESULT_OK) {
                user = (UserAccount) data.getExtras().getSerializable(StringUtils.BundleAccountKey);
            }
        }
    }
}
