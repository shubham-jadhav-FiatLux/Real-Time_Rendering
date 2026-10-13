package com.example.window;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import android.graphics.Color;
import android.content.pm.ActivityInfo;

// For Fullscreen
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.core.view.WindowInsetsCompat;

public class MainActivity extends AppCompatActivity {

    private MyView myView;                                          // Class Variable or Field Variable/Member

    @Override
    protected void onCreate(Bundle savedInstanceState) {            
        super.onCreate(savedInstanceState);                         // onCreate => WM_CREATE
                                                                    // onDestroy => WM_DESTROY
                                                                    // onPaint => WM_PAINT

        // Hide action bar/title bar
        getSupportActionBar().hide();

        // Fullscreen
        WindowCompat.setDecorFitsSystemWindows(getWindow(), false);

        // Hiding system bars and ime
        WindowInsetsControllerCompat windowInsetsControllerCompat = WindowCompat.getInsetsController(getWindow(), getWindow().getDecorView());
        windowInsetsControllerCompat.hide(WindowInsetsCompat.Type.systemBars() | WindowInsetsCompat.Type.ime());

        // Forced Landscape Orientation
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        // Setting Background Color
        getWindow().getDecorView().setBackgroundColor(Color.rgb(0, 0, 0));
        
        myView = new MyView(this);
        setContentView(myView);
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }
}
