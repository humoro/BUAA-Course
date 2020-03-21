package com.example.mycalendar;

import org.apache.commons.codec.digest.DigestUtils;
import org.apache.commons.lang3.RandomStringUtils;

public class EncryptUtils {
    private static final String str = "qwertyuioplkjhgfdsazxcvbnm1234567890QWERTYUIOPLKJHGFDSAZXCVBNM";
    public String MD5SaltEncryptor(String password, String salt) {
        String concat = password + salt;
        return DigestUtils.md5Hex(concat.getBytes());
    }

    public String getSalt() {
        return RandomStringUtils.random(16, str);
    }

    public boolean matchPassword(String passwordInput, String salt, String password) {
        return password.equals(MD5SaltEncryptor(passwordInput, salt));
    }
}
