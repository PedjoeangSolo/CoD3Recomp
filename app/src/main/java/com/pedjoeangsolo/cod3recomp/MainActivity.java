package com.pedjoeangsolo.cod3recomp;

import android.app.Activity;
import android.os.Bundle;
import android.graphics.Color;
import android.view.Gravity;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

public final class MainActivity extends Activity {
    static {
        System.loadLibrary("cod3android");
    }

    private static native String nativeStatus();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        LinearLayout content = new LinearLayout(this);
        content.setOrientation(LinearLayout.VERTICAL);
        content.setPadding(28, 36, 28, 28);
        content.setBackgroundColor(Color.rgb(18, 24, 32));

        TextView title = new TextView(this);
        title.setText("CoD3Recomp · Android ARM64");
        title.setTextColor(Color.WHITE);
        title.setTextSize(24);
        title.setGravity(Gravity.CENTER_VERTICAL);
        content.addView(title);

        TextView subtitle = new TextView(this);
        subtitle.setText("Android native build diagnostic");
        subtitle.setTextColor(Color.rgb(150, 190, 220));
        subtitle.setTextSize(15);
        subtitle.setPadding(0, 12, 0, 28);
        content.addView(subtitle);

        TextView result = new TextView(this);
        result.setTextColor(Color.rgb(220, 235, 245));
        result.setTextSize(16);
        result.setText(nativeStatus());
        content.addView(result);

        TextView notice = new TextView(this);
        notice.setTextColor(Color.rgb(255, 205, 120));
        notice.setTextSize(14);
        notice.setPadding(0, 28, 0, 0);
        notice.setText("STATUS: SMOKE TEST ONLY\nThis APK verifies Android packaging and ARM64 JNI loading. It does not run Call of Duty 3 yet.");
        content.addView(notice);

        ScrollView scroll = new ScrollView(this);
        scroll.setFillViewport(true);
        scroll.addView(content);
        setContentView(scroll);
    }
}
