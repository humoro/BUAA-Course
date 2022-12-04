
import org.apache.commons.lang3.RandomStringUtils;
import org.springframework.util.DigestUtils;

// 加密的过程是拿到password之后先试用md5加密然后取其中的一段16位长度为slat和password结合md5加密
public class Main {
    private static final String str = "qwertyuioplkjhgfdsazxcvbnm1234567890QWERTYUIOPLKJHGFDSAZXCVBNM";
    public static String md5Encryptor(String password) {
        return DigestUtils.md5DigestAsHex(password.getBytes());
    }

    public static String getSalt() {
        return RandomStringUtils.random(16, str);
    }

    public static void main(String[] args) {
        String password = "12345678";
        String salt = getSalt();
        System.out.println(salt);
        String mdPassword = md5Encryptor(password + salt);
        System.out.println(mdPassword);
    }
}
