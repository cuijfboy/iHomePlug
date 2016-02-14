package name.ilab.ihome;

import android.content.Context;
import android.content.pm.PackageManager;
import android.net.wifi.WifiManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import java.util.logging.ConsoleHandler;
import java.util.logging.Handler;
import java.util.logging.LogManager;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

/**
 * Created by cuijfboy on 16/1/21.
 */
public class Utils {

    public static Logger logger;
    private static final String TAG = "UpCloud_Utils";
    public static final String LOGGER_NAME = "com.haier.uhome.upcloud";

    protected synchronized static void initLogger() {
        logger = createLogger(LOGGER_NAME);
    }

    /**
     * 创建Logger,同时配置RootLogger.
     *
     * @param name Logger名称
     * @return Logger对象
     */
    private static Logger createLogger(String name) {
        Logger logger = LogManager.getLogManager().getLogger(name);
        if (logger != null) {
            return logger;
        }

        Logger rootLogger = LogManager.getLogManager().getLogger("");
        Handler[] handlers = rootLogger.getHandlers();
        Log.d(TAG, "createLogger : rootLogger : handlers.length = " + handlers.length);
        for (Handler handler : handlers) {
            String handlerName = handler.getClass().getName();
            Log.d(TAG, "createLogger : rootLogger : handler = " + handlerName);
            if ("com.android.internal.logging.AndroidHandler".equals(handlerName)) {
                rootLogger.removeHandler(handler);
                Log.d(TAG, "createLogger : rootLogger : remove AndroidHandler");
                break;
            }
        }

        logger = Logger.getLogger(LOGGER_NAME);
        logger.addHandler(new ConsoleHandler() {
            {
                setOutputStream(System.out);
                setFormatter(new SimpleFormatter());
            }
        });

        Log.d(TAG, "createLogger : Logger \"" + name + "\" created !");
        logger.info("The logger \"" + name + "\" says : \"Hello world ! O(∩_∩)O~\"");

        return logger;
    }

    /**
     * 获取APP的元数据
     *
     * @param key     元数据项的KEY
     * @param context 程序上下文
     * @return 元数据的VALUE
     */
    public static String getApplicationMetaData(String key, Context context) {
        try {
            return context.getPackageManager()
                    .getApplicationInfo(context.getPackageName(), PackageManager.GET_META_DATA)
                    .metaData.getString(key);
        } catch (PackageManager.NameNotFoundException e) {
            Utils.logger.severe("!!! FATAL ERROR !!!");
            e.printStackTrace();
        }
        return null;
    }

    /**
     * 获取APP的版本号
     *
     * @param context 程序上下文
     * @return APP的版本号
     */
    public static String getApplicationVersionName(Context context) {
        try {
            return context.getPackageManager().getPackageInfo(context.getPackageName(), 0)
                    .versionName;
        } catch (PackageManager.NameNotFoundException e) {
            Utils.logger.severe("!!! FATAL ERROR !!!");
            e.printStackTrace();
        }
        return null;
    }

    public static String getPhoneDeviceId(Context context) {
        return ((TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE))
                .getDeviceId();
    }

    public static String getPhoneDeviceMac(Context context) {
        return ((WifiManager) context.getSystemService(Context.WIFI_SERVICE))
                .getConnectionInfo().getMacAddress();
    }
}
