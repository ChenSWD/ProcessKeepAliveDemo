package com.example.chen.servicealivetest;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Process;
import android.util.Log;

import java.util.Timer;
import java.util.TimerTask;

public class KeepService extends Service {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private static final String TAG = "KEEP_ALIVE";

    private int count = 0;

    public KeepService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "uid = " + String.valueOf(Process.myUid()));
        createWatcher(String.valueOf(Process.myUid()));
        connectServer();
        Timer timer = new Timer();
        //随便做一个操作，循环
        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                Log.d(TAG, "服务正在运行 " + count);
                count++;
            }
        }, 0, 1000);
    }

    public native void createWatcher(String uid);

    //连接服务端
    public native void connectServer();
}
