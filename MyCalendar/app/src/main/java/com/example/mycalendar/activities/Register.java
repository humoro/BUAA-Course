package com.example.mycalendar.activities;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.example.mycalendar.R;
import com.example.mycalendar.utils.StringUtils;
import com.example.mycalendar.UserAccount;
import com.example.mycalendar.utils.ActivityStackUtils;
import com.example.mycalendar.utils.EncryptUtils;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.util.EntityUtils;

import java.io.IOException;
import java.util.ArrayList;

public class Register extends AppCompatActivity implements View.OnClickListener {
    private final static int MSG_DUPLICATEUSERNAME  = 0x00001;
    private final static int MSG_LEAGALUSERNAME     = 0x00010;
    private final static int MSG_NETOUTOFTIME       = 0x00100;
    private final static int MSG_REGISTERSUCC       = 0x01000;
    private final static int MSG_REGISTERFAULT      = 0x10000;


    private ImageView mRegisterImage;
    private EditText accountText;
    private EditText passWordText;
    private EditText confirmPassWord;
    private TextView userNameHint;
    private TextView passwordHint;
    private TextView confirmHint;

    private boolean leagalUserName = false;
    private boolean leagalPassword = false;
    private boolean leagalConfirm = false;
    private boolean accessNetWork = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);
        mRegisterImage = findViewById(R.id.stc_reg_account_pic);
        accountText = findViewById(R.id.stc_register_account_text);
        passWordText = findViewById(R.id.stc_register_password_text);
        confirmPassWord = findViewById(R.id.stc_register_confirm_passWord_text);
        userNameHint = findViewById(R.id.stc_register_name_hint);
        passwordHint = findViewById(R.id.stc_register_password_hint);
        confirmHint = findViewById(R.id.stc_register_confirm_hint);

        ActivityStackUtils.addActivity(this);
        initView();
    }

    private void initView() {
        mRegisterImage.setImageResource(R.mipmap.ic_register_account_icon);
        mRegisterImage.setOnClickListener(this);
        accountText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                if (s.toString().isEmpty()) { // 输入用户名为空
                    userNameHint.setText("请输入用户名");
                    leagalUserName = false;
                } else if (!StringUtils.checkEmailFormat(s.toString())){
                    userNameHint.setText("邮件格式错误");
                    leagalUserName = false;
                }
                else  {
                    new Thread(()->{
                        try {
                            HttpClient httpclient= new DefaultHttpClient();
                            HttpPost httpPost = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/UserExists");//服务器地址，指向查询用户是否信息的servlet
                            ArrayList<NameValuePair> params= new ArrayList<>();//将id装入list
                            params.add(new BasicNameValuePair(StringUtils.HttpUserNameKey,s.toString()));
                            final UrlEncodedFormEntity entity = new UrlEncodedFormEntity(params, "utf-8");
                            httpPost.setEntity(entity);
                            HttpResponse response = httpclient.execute(httpPost);
                            Message msg = new Message();
                            if (response.getStatusLine().getStatusCode() == 200) {
                                String ans = EntityUtils.toString(response.getEntity());
                                if (ans.equals("true")) {
                                    msg.what = MSG_DUPLICATEUSERNAME; // 用户名重复
                                } else {
                                    msg.what = MSG_LEAGALUSERNAME;
                                }
                            } else { // 网络请求超时
                                msg.what = MSG_NETOUTOFTIME;
                            }
                            handler.sendMessage(msg);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
//                        boolean haveName = DataBaseUtils.isDuplicateUserName(s.toString());
//                        Message msg = new Message();
//                        if (haveName) {
//                            msg.what = 0x01;
//                        } else {
//                            msg.what = 0x10;
//                        }
//                        handler.sendMessage(msg);
                    }).start();
                }
            }
        });

        passWordText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                if (s.toString().isEmpty()) {
                    leagalPassword = false;
                    passwordHint.setText("请输入密码");
                } else {
                    leagalPassword = true;
                    passwordHint.setText("");
                }
            }
        });

        confirmPassWord.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                String password = passWordText.getText().toString();
                if (s.toString().isEmpty()) {
                    leagalConfirm = false;
                    confirmHint.setText("请确认您的密码");
                } else if (!s.toString().equals(password)){
                    leagalConfirm = false;
                    confirmHint.setText("确认密码不同");
                } else {
                    leagalConfirm = true;
                    confirmHint.setText("");
                }
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityStackUtils.removeActivity(this);
    }

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_DUPLICATEUSERNAME:
                {
                    userNameHint.setText("用户名已注册");
                    leagalUserName = false;
                }
                break;
                case MSG_LEAGALUSERNAME:
                {
                    userNameHint.setText(""); // 清空提示
                    leagalUserName = true;
                }
                break;
                case MSG_NETOUTOFTIME:
                {
                    networkOutOfTimeToast();
                    accessNetWork = false;
                }
                break;
                case MSG_REGISTERSUCC: // 注册成功
                {
                    Bundle data = msg.getData();
                    UserAccount account = (UserAccount) data.getSerializable(StringUtils.BundleAccountKey);
                    registerSuccessfully(account);
                }
                break;
                case MSG_REGISTERFAULT: //注册失败
                {
                    registerFaultToast();
                }
                break;
            }
        }
    };

    private void networkOutOfTimeToast() {
        Toast.makeText(this, "网络连接超时", Toast.LENGTH_LONG).show();
    }

    private void registerFaultToast() {
        Toast.makeText(this, "注册失败", Toast.LENGTH_LONG).show();
    }

    @Override
    public void onClick(View v) {
        if (leagalUserName && leagalPassword && leagalConfirm && v.getId() == R.id.stc_reg_account_pic) {
            String userName = accountText.getText().toString();
            String passWord = passWordText.getText().toString();
            String salt = new EncryptUtils().getSalt();
            String stordPassword = EncryptUtils.MD5SaltEncryptor(passWord, salt);
            new Thread(() -> {
               try{
                   HttpClient httpclient= new DefaultHttpClient();
                   HttpPost httpPost = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/InsertNewUser");//服务器地址，指向查询用户是否信息的servlet
                   ArrayList<NameValuePair> params= new ArrayList<>();
                   params.add(new BasicNameValuePair(StringUtils.HttpUserNameKey,userName));
                   params.add(new BasicNameValuePair(StringUtils.HttpPassWordKey, stordPassword));
                   params.add(new BasicNameValuePair(StringUtils.HttpSaltKey, salt));
                   final UrlEncodedFormEntity entity = new UrlEncodedFormEntity(params, "utf-8");
                   httpPost.setEntity(entity);
                   HttpResponse response = httpclient.execute(httpPost);
                   Message msg = new Message();
                   if (response.getStatusLine().getStatusCode() == 200) {
                        int insert = Integer.parseInt(EntityUtils.toString(response.getEntity()));
                       Log.d("register : ", "onClick: register " + insert + " " + userName + " " + passWord + " " + salt);
                        if (insert > 0) {
                            Bundle bundle = new Bundle();
                            UserAccount account = new UserAccount(userName, passWord);
                            bundle.putSerializable(StringUtils.BundleAccountKey,account);
                            msg.setData(bundle);
                            msg.what = MSG_REGISTERSUCC;
                        } else {
                            msg.what = MSG_REGISTERFAULT;
                        }
                   } else {
                       msg.what = MSG_NETOUTOFTIME;
                   }
                   handler.sendMessage(msg);
               } catch (Exception e) {
                   e.printStackTrace();
               }
            }).start();
        }
    }

    private void registerSuccessfully(UserAccount accout) {
        Bundle data = new Bundle();
        data.putSerializable(StringUtils.BundleAccountKey, accout);
        Log.d("registe", "registerSuccessfully: hahaha");
        Toast.makeText(this, "注册成功!", Toast.LENGTH_LONG).show();
        Intent intent = new Intent(this, MainActivity.class);
        intent.putExtras(data);
        startActivity(intent);
        this.finish();
    }
}
