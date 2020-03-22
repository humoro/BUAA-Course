package com.example.mycalendar;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

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
                case 0x001: // 用户不存在
                {
                    nonUserToast();
                }
                break;
                case 0x010: // 密码错误
                {
                    wrongPasswordToast();
                }
                break;
                case 0x100: // 登录成功
                {
                    loginSuccessfully();
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


    public void clickLogin() { // 登录界面
        String userName = accountText.getText().toString();
        String password = passWordText.getText().toString();
        new Thread(() -> {
            boolean haveUser = DataBaseUtils.isDuplicateUserName(userName);
            Message message = new Message();
            if (!haveUser) { // 用户不存在
                message.what = 0x001;
            } else {
                String passwordS = DataBaseUtils.getUserAccountInfo(userName,DataBaseUtils.QPASSWORD);
                String salt = DataBaseUtils.getUserAccountInfo(userName, DataBaseUtils.QSALT);
                String encryptPassword = EncryptUtils.MD5SaltEncryptor(password, salt);
                assert encryptPassword != null;
                if (encryptPassword.equals(passwordS)) {
                    message.what = 0x100;
                } else {
                    message.what = 0x010;
                }
            }
            handler.sendMessage(message);
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
