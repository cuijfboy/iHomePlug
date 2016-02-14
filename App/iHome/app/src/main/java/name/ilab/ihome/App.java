package name.ilab.ihome;

import android.app.Application;

import name.ilab.http.HttpApiHelper;

/**
 * Created by cuijfboy on 16/2/12.
 */
public class App extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        Utils.initLogger();

        HttpApiHelper.getInstance()
                .registerDefaultHttpClient(new HttpClientAdapter(getApplicationContext()));
    }
}
