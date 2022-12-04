package com.example.mycalendar.adapters;


import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.example.mycalendar.R;

public class MainPagerTitleGridViewAdapter extends BaseAdapter {

    int[] titles = new int[] { R.string.sunday, R.string.monday, R.string.tuesday,
            R.string.wednesday, R.string.thursday, R.string.friday, R.string.saturday };

    private Activity activity;

    // construct
    public MainPagerTitleGridViewAdapter(Activity a) {
        activity = a;
    }

    @Override
    public int getCount() {
        return titles.length;
    }

    @Override
    public Object getItem(int position) {
        return titles[position];
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        LinearLayout iv = new LinearLayout(activity);
        TextView txtDay = new TextView(activity);
        txtDay.setFocusable(false);
        txtDay.setBackgroundColor(Color.TRANSPARENT);
        iv.setOrientation(LinearLayout.HORIZONTAL);
        txtDay.setGravity(Gravity.CENTER);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
        txtDay.setTextColor(0xff999999);
        txtDay.setText((Integer) getItem(position));
        iv.addView(txtDay, lp);
        return iv;
    }
}