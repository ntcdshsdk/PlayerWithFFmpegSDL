package com.osshare.player;

import android.Manifest;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import com.osshare.player.base.AbsActivity;
import org.libsdl.app.SDLActivity;

public class MainActivity extends AbsActivity {

    String[] permissions = new String[]{Manifest.permission.RECORD_AUDIO, Manifest.permission.CAMERA, Manifest.permission.WRITE_EXTERNAL_STORAGE};


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        requestPermission(permissions);
    }

    @Override
    protected void onAllPermissionsGranted() {
        Intent intent = new Intent(this, SDLActivity.class);
        startActivity(intent);
    }


}
