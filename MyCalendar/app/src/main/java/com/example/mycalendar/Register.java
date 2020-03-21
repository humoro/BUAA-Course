package com.example.mycalendar;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

public class Register extends AppCompatActivity implements View.OnClickListener {
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
                } else {
                    new Thread(()->{
                        boolean haveName = DataBaseUtils.isDuplicateUserName(s.toString());
                        Message msg = new Message();
                        if (haveName) {
                            msg.what = 0x01;
                        } else {
                            msg.what = 0x10;
                        }
                        handler.sendMessage(msg);
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
                case 0x01:
                    {
                        userNameHint.setText("用户名已注册");
                        leagalUserName = false;
                    }
                    break;
                case 0x10:
                    {
                        userNameHint.setText(""); // 清空提示
                        leagalUserName = true;
                    }
                    break;
            }
        }
    };

    @Override
    public void onClick(View v) {
        if (leagalUserName && leagalPassword && leagalConfirm && v.getId() == R.id.stc_reg_account_pic) {
            String accout = accountText.getText().toString();
            String passWord = passWordText.getText().toString();
            String salt = new EncryptUtils().getSalt();
            new Thread(() -> DataBaseUtils.insertUserAccount(accout, passWord, salt)).start();
            Bundle data = new Bundle();
            data.putSerializable(StringUtils.BundleAccountKey, new UserAccount(accout, passWord));
            Toast.makeText(this, "Regist successfully!", Toast.LENGTH_LONG).show();
            Intent intent = new Intent(this, MainActivity.class);
            intent.putExtras(data);
            startActivity(intent);
            this.finish();
        }
    }
}
