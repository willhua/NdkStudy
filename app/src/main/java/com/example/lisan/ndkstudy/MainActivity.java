package com.example.lisan.ndkstudy;

import android.graphics.Bitmap;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        nativeInit();

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        findViewById(R.id.button_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startThreads(4,6);
            }
        });
    }

    @Override
    protected void onDestroy() {
        nativeFree();
        super.onDestroy();
    }

    private void startThreads(int threads, final int iterations){
        for(int i = 0; i < threads; ++i)
        {
            final int id = i;
            new Thread(new Runnable() {
                @Override
                public void run() {
                    nativeWorker(id, iterations);
                }
            }).start();
        }
    }

    private native void nativeInit();
    private native void nativeGetDark(Bitmap bitmap, int width, int height);
    private native void nativeFree();
    private native void nativeWorker(int id, int iterations);

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    private void onNativeMsg(final String msg){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.d(TAG, msg);
            }
        });
    }
}
