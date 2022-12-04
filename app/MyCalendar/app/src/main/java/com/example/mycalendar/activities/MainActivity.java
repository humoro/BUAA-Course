package com.example.mycalendar.activities;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;

import com.example.mycalendar.R;
import com.example.mycalendar.utils.StringUtils;
import com.example.mycalendar.UserAccount;
import com.example.mycalendar.utils.ActivityStackUtils;
import com.example.mycalendar.utils.EncryptUtils;

import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.util.EntityUtils;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private final static int MSG_USERNOTEXISTS = 0x0001;
    private final static int MSG_WRONGPASSWORD = 0x0010;
    private final static int MSG_LOGINSUCCESSFULLY = 0x0100;
    private final static int MSG_NETWORKOUTOFTIME = 0x10000;

    private ImageView mLoginImage;
    private ImageView mRegisterImage;

    private EditText accountText;
    private EditText passWordText;

    UserAccount account = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Bundle receive = getIntent().getExtras();

        if (receive != null) account = (UserAccount) receive.getSerializable(StringUtils.BundleAccountKey);
        setContentView(R.layout.activity_main);

        mLoginImage = findViewById(R.id.stc_pic_login);
        mRegisterImage = findViewById(R.id.stc_pic_register);
        accountText = findViewById(R.id.stc_login_account_text);
        passWordText = findViewById(R.id.stc_login_password_text);

        ActivityStackUtils.finishAllDead();
        ActivityStackUtils.addActivity(this);
        initView();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityStackUtils.removeActivity(this);
    }

    public void initView() {
        mLoginImage.setImageResource(R.mipmap.ic_login_light);
        mRegisterImage.setImageResource(R.mipmap.ic_register_light);
        mLoginImage.setOnClickListener(this);
        mRegisterImage.setOnClickListener(this);
        if (account != null) {
            accountText.setText(account.getUserName());
            passWordText.setText(account.getPassWord());
        }
    }

    public void clickRegister() { // 跳转到注册界面
        Intent intent = new Intent(this, Register.class);
        startActivity(intent);
    }

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {

            switch (msg.what) {
                case MSG_USERNOTEXISTS: // 用户不存在
                {
                    nonUserToast();
                }
                break;
                case MSG_WRONGPASSWORD: // 密码错误
                {
                    wrongPasswordToast();
                }
                break;
                case MSG_LOGINSUCCESSFULLY: // 登录成功
                {
                    loginSuccessfully();
                }
                break;
                case MSG_NETWORKOUTOFTIME:
                {
                    netConnectOutOfTime();
                }
                break;
            }
        }
    };

    @SuppressLint("ShowToast")
    private void nonUserToast() {
        Toast.makeText(this, "用户不存在", Toast.LENGTH_LONG).show();
    }

    @SuppressLint("ShowToast")
    private void wrongPasswordToast() {
        Toast.makeText(this, "密码错误", Toast.LENGTH_LONG).show();

    }
    @SuppressLint("ShowToast")
    private void netConnectOutOfTime() {
        Toast.makeText(this, "网络连接超时", Toast.LENGTH_LONG).show();
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public void clickLogin() { // 登录界面
        String userName = accountText.getText().toString();
        String password = passWordText.getText().toString();
        new Thread(() -> {
            try {
                HttpClient httpclient= new DefaultHttpClient();
                HttpPost httpPostGetSalt = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/GetSalt");//服务器地址，指向获取salt信息的servlet
                ArrayList<NameValuePair> params= new ArrayList<>();//将id装入list
                params.add(new BasicNameValuePair(StringUtils.HttpUserNameKey,userName));
                final UrlEncodedFormEntity saltEntity = new UrlEncodedFormEntity(params, "utf-8");
                httpPostGetSalt.setEntity(saltEntity);
                HttpResponse saltResponse = httpclient.execute(httpPostGetSalt);
                Message message = new Message();
                if (saltResponse.getStatusLine().getStatusCode() == 200) {
                    String salt = EntityUtils.toString(saltResponse.getEntity(), "utf-8");
                    Log.d("get salt", "clickLogin: " + salt);
                    if (salt.equals(StringUtils.UserNotExists)) { // 用户不存在
                        message.what = MSG_USERNOTEXISTS;
                    } else {
                        HttpPost httpPostLogin = new HttpPost("http://10.0.2.2:8080/calendarWeb_war_exploded/Login");//服务器地址，指向获取账户信息的servlet
                        String checkPW = EncryptUtils.MD5SaltEncryptor(password, salt);
                        Log.d("get salt", "clickLogin: " + checkPW);
                        params.add(new BasicNameValuePair(StringUtils.HttpPassWordKey,checkPW));
                        final UrlEncodedFormEntity entity = new UrlEncodedFormEntity(params, "utf-8");//以UTF-8格式发送
                        httpPostLogin.setEntity(entity);
                        HttpResponse httpResponse= httpclient.execute(httpPostLogin);
                        if(httpResponse.getStatusLine().getStatusCode() == 200) {//在200毫秒之内接收到返回值
                            HttpEntity entity1=httpResponse.getEntity();
                            String response = EntityUtils.toString(entity1, "utf-8");//以UTF-8格式解析
                            if (response.equals(StringUtils.WrongPassWord)) {
                                message.what = MSG_WRONGPASSWORD;
                            } else {
                                message.what = MSG_LOGINSUCCESSFULLY;
                            }
                        } else {
                            message.what = MSG_NETWORKOUTOFTIME; // 网络超时
                        }
                    }
                } else {
                    message.what = MSG_NETWORKOUTOFTIME; // 网络超时
                }
                handler.sendMessage(message); //使用Message传递消息给线程
            }
            catch (Exception e) {
                e.printStackTrace();
            }
//            boolean haveUser = DataBaseUtils.isDuplicateUserName(userName);
//            Message message = new Message();
//            if (!haveUser) { // 用户不存在
//                message.what = 0x001;
//            } else {
//                String passwordS = DataBaseUtils.getUserAccountInfo(userName,DataBaseUtils.QPASSWORD);
//                String salt = DataBaseUtils.getUserAccountInfo(userName, DataBaseUtils.QSALT);
//                String encryptPassword = EncryptUtils.MD5SaltEncryptor(password, salt);
//                assert encryptPassword != null;
//                if (encryptPassword.equals(passwordS)) {
//                    message.what = 0x100;
//                } else {
//                    message.what = 0x010;
//                }
//            }
//            handler.sendMessage(message);
        }).start();
    }

    public void loginSuccessfully() { //登录成功跳转到主页面并且传递账户信息
        String userName = accountText.getText().toString();
        String password = passWordText.getText().toString();
        Bundle data = new Bundle();
        data.putSerializable(StringUtils.BundleAccountKey, new UserAccount(userName, password));
        Toast.makeText(this, "登录成功!", Toast.LENGTH_LONG).show();
        Intent intent = new Intent(this, MainPage.class);
        intent.putExtras(data);
        startActivity(intent);
        this.finish();
    }


    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.stc_pic_register: // 点击注册
                clickRegister();
                break;
            case R.id.stc_pic_login: // 点击登录
                clickLogin();
                break;
        }
    }
}
