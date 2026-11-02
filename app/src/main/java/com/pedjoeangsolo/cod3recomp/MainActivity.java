package com.pedjoeangsolo.cod3recomp;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.graphics.Color;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import java.io.IOException;

public final class MainActivity extends Activity {
    static { System.loadLibrary("cod3android"); }
    private static native String nativeStatus();
    private static native String nativeInspectIso(int fd);
    private static final int PICK_ISO = 7314;
    private TextView isoResult;

    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        LinearLayout content = new LinearLayout(this);
        content.setOrientation(LinearLayout.VERTICAL);
        content.setPadding(24, 32, 24, 28);
        content.setBackgroundColor(Color.rgb(18, 24, 32));

        TextView title = new TextView(this);
        title.setText("CoD3Recomp · Android ARM64");
        title.setTextColor(Color.WHITE); title.setTextSize(23);
        content.addView(title);

        TextView subtitle = new TextView(this);
        subtitle.setText("Xbox 360 ISO boot preparation");
        subtitle.setTextColor(Color.rgb(150, 190, 220));
        subtitle.setTextSize(15); subtitle.setPadding(0, 10, 0, 18);
        content.addView(subtitle);

        Button pick = new Button(this);
        pick.setText("PILIH FILE ISO GAME");
        pick.setOnClickListener(v -> chooseIso());
        content.addView(pick);

        isoResult = new TextView(this);
        isoResult.setTextColor(Color.rgb(220, 235, 245));
        isoResult.setTextSize(14); isoResult.setPadding(0, 18, 0, 22);
        isoResult.setText("Pilih ISO Xbox 360 dari penyimpanan. File dibaca langsung; tidak diunggah ke GitHub.");
        content.addView(isoResult);

        TextView status = new TextView(this);
        status.setTextColor(Color.rgb(190, 205, 215));
        status.setTextSize(13); status.setText(nativeStatus());
        content.addView(status);

        ScrollView scroll = new ScrollView(this);
        scroll.setFillViewport(true); scroll.addView(content); setContentView(scroll);
    }

    private void chooseIso() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        intent.putExtra(Intent.EXTRA_TITLE, "Pilih ISO Call of Duty 3");
        startActivityForResult(intent, PICK_ISO);
    }

    @Override protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode != PICK_ISO || resultCode != RESULT_OK || data == null || data.getData() == null) return;
        try (ParcelFileDescriptor pfd = getContentResolver().openFileDescriptor(data.getData(), "r")) {
            if (pfd == null) throw new IOException("Tidak dapat membuka file ISO.");
            isoResult.setText("Memeriksa XDVDFS dan mencari default.xex...");
            isoResult.setText(nativeInspectIso(pfd.getFd()));
        } catch (Exception e) {
            isoResult.setText("GAGAL MEMBACA ISO: " + e.getMessage());
        }
    }
}
